/*****************************************************************************

 @(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/06 10:36:10 $

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

 Last Modified $Date: 2006/05/06 10:36:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp_n.c,v $
 Revision 0.9.2.14  2006/05/06 10:36:10  brian
 - minor changes

 Revision 0.9.2.13  2006/03/03 11:47:02  brian
 - 32/64-bit compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/06 10:36:10 $"

static char const ident[] = "$RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/06 10:36:10 $";

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
static char devname[256] = "/dev/sctp_n";

static int exit_on_failure = 0;

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_data = 1;

static int last_event = 0;
static int last_retval = 0;
static int PRIM_type = 0;
static int UNIX_error = 0;
static int NPI_error = 0;
static int CONIND_number = 2;
static int TOKEN_value = 0;
static int SEQ_number = 1;
static int SERV_type = N_CLNS;
static int CURRENT_state = NS_UNBND;
N_info_ack_t last_info = { 0, };
static int last_prio = 0;

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

#define BUFSIZE 5*4096

#define FFLUSH(stream)

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	20000
#define INFINITE_WAIT	-1

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
static int test_timout = 200;
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
	__EVENT_NO_MSG = -6, __EVENT_TIMEOUT = -5, __EVENT_UNKNOWN = -4,
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
		UNIX_error = errno;
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR! couldn't get time!            !  !                    \n");
		fprintf(stdout, "%20s! %-54s\n", __FUNCTION__, strerror(UNIX_error));
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
unsigned short ports[4] = { 10000, 10001, 10002, 10003 };
const char *addr_strings[4] = { "127.0.0.1", "127.0.0.2", "127.0.0.3", "127.0.0.4" };

/*
 *  Options
 */

N_qos_sel_data_sctp_t qos_data = {
	.n_qos_type = N_QOS_SEL_DATA_SCTP,
	.ppi = 10,
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
	.ppi = 10,
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
event_string(int event)
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
service_type(np_ulong type)
{
	switch (type) {
	case 0:
		return ("(none)");
	case N_CLNS:
		return ("N_CLNS");
	case N_CONS:
		return ("N_CONS");
	case N_CLNS|N_CONS:
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

	if (verbose < 3)
		return;
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
void
print_prots(int child, char *pro_ptr, size_t pro_len)
{
	unsigned char *prot;
	if (verbose < 3)
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
prim_string(np_ulong prim)
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
		"  open()      ----->v                                   .                   \n",
		"                    |                                   v<-----     open()  \n",
		"                    |                                v<-+------     open()  \n",
		"                    .                                .  .                   \n",
	};

	if (verbose > 3)
		print_simple(child, msgs);
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

	if (verbose > 3)
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

void print_string_state(int child, const char *msgs[], const char *string);

void
print_failure(int child, const char *string)
{
	static const char *msgs[] = {
		"....................|%-32s|..|                    [%d:%03d]\n",
		"                    |%-32s|  |................... [%d:%03d]\n",
		"                    |%-32s|...................... [%d:%03d]\n",
		"....................|%-32s|..|................... [%d:%03d]\n",
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
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |<---%-14s  [%d:%03d]\n",
		"                    |                                |<-+----%-14s  [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->|- - - - - - - - - - - - - - - ->|->|                    [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|- |<-%16s- [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s- [%d:%03d]\n",
		"                    |                                |  |                    [%d:%03d]\n",
	};

	if (verbose > 0)
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

	if (verbose > 0)
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

	if (verbose > 0)
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

	if (verbose > 0)
		print_long_state(child, msgs, prim);
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
print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - |- |                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|  |--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - |->|--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show_data) || verbose > 1)
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

	if (verbose > 3)
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

	if (verbose > 4)
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

	if (verbose)
		print_triple_int(child, msgs, value, child, state);
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

	if (verbose > 0)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
}

void
print_ioctl(int child, int cmd, intptr_t arg)
{
	if (verbose > 3)
		print_ti_ioctl(child, cmd, arg);
}

void
print_datcall(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"  %1$16s->|- - %2$4d bytes- - - - - - - - ->|->|                    [%3$d:%4$03d]\n",
		"                    |< - %2$4d bytes- - - - - - - - - |- |<-%1$16s  [%3$d:%4$03d]\n",
		"                    |< - %2$4d bytes- - - - - - - - - |<-+--%1$16s  [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show_data) || verbose > 1)
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

	if (verbose > 0)
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

	if (verbose > 0)
		print_string_state(child, msgs, t_errno_string(terror, error));
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

	if (verbose > 1 && show)
		print_string_state(child, msgs, event_string(want));
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
		"/ / / / / / / / / / | / / Waiting %8.4f seconds / |  |                    [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds / |  | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds / |/ | / / / / / / / / /  [%d:%03d]\n",
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
	unsigned char *qos_ptr = (unsigned char *)(cmd_buf + qos_ofs);
	union N_qos_ip_types *qos = (union N_qos_ip_types *) qos_ptr;
	char buf[64];

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

	if (verbose < 4)
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
		print_open(fdp);
		if ((fd = open(name, O_NONBLOCK | O_RDWR)) >= 0) {
			*fdp = fd;
			print_success(fd);
			return (__RESULT_SUCCESS);
		}
		print_errno(fd, (UNIX_error = errno));
		if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
		print_errno(fd, (UNIX_error = errno));
		if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
test_ioctl(int child, int cmd, intptr_t arg)
{
	print_ioctl(child, cmd, arg);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
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
	if (verbose > 4) {
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
	if (test_ioctl(child, I_FDINSERT, (intptr_t) & fdi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	if (flags & MSG_BAND || band) {
		if (verbose > 4) {
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
				print_errno(child, (UNIX_error = errno));
				if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
			print_datcall(child, "M_DATA----------", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
				print_errno(child, (UNIX_error = errno));
				if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
			print_errno(child, (UNIX_error = errno));
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
			print_errno(child, (UNIX_error = errno));
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
			print_errno(child, (UNIX_error = errno));
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
			print_errno(child, (UNIX_error = errno));
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
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		if (verbose > 3)
			print_success_value(child, last_retval);
		break;
	}
	if (cmd == I_STR && verbose > 3) {
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
		if (verbose > 3)
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
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags | O_NONBLOCK)) == -1) {
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
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
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags & ~O_NONBLOCK)) == -1) {
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EINTR || UNIX_error == ERESTART)
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
		print_errno(child, (UNIX_error = errno));
		if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
		print_errno(child, (UNIX_error = errno));
		if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
		print_errno(child, (UNIX_error = errno));
		if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
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
	int offset = 3 * index;
	int i;

	for (i = 0; i < 3; i++) {
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
		for (i = 0; i < 3; i++) {
#ifndef SCTP_VERSION_2
			addrs[child].port = htons(ports[child] + offset);
			inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
			addrs[child][i].sin_family = AF_INET;
			addrs[child][i].sin_port = htons(ports[child] + offset);
			inet_aton(addr_strings[i], &addrs[child][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
		}
		if (test_open(child, devname) != __RESULT_SUCCESS)
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
	char buf[64];

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
		return test_ti_ioctl(child, I_PUSH, (intptr_t) "tirdwr");
	case __TEST_POP:
		return test_ti_ioctl(child, I_POP, (intptr_t) NULL);
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
		p->error_ack.UNIX_error = UNIX_error;
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
do_decode_data(int child, struct strbuf *data)
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
	char buf[64];

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
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_EXDATA_IND:
			event = __TEST_EXDATA_IND;
			print_rx_prim(child, prim_string(p->type));
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
			if (verbose > 3) {
				snprintf(buf, sizeof(buf), "CONIND_number = %d", CONIND_number);
				print_string(child, buf);
				snprintf(buf, sizeof(buf), "TOKEN_value = %x", TOKEN_value);
				print_string(child, buf);
			}
			break;
		case N_ERROR_ACK:
			event = __TEST_ERROR_ACK;
			NPI_error = p->error_ack.NPI_error;
			UNIX_error = p->error_ack.UNIX_error;
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
			if (do_decode_data(child, data) != __TEST_DATA)
				event = __RESULT_FAILURE;
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
		if ((last_event = do_decode_data(child, data)) != __EVENT_UNKNOWN)
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
		if (verbose > 4)
			print_syscall(child, "poll()");
		pfd[0].fd = test_fd[child];
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			print_errno(child, (UNIX_error = errno));
			if (UNIX_error == EAGAIN || UNIX_error == EINTR || UNIX_error == ERESTART)
				break;
			return (__RESULT_FAILURE);
		case 0:
			if (verbose > 4)
				print_success(child);
			print_nothing(child);
			last_event = __EVENT_NO_MSG;
			return time_event(child, __EVENT_NO_MSG);
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
					print_errno(child, (UNIX_error = errno));
					if (UNIX_error == EINTR || UNIX_error == ERESTART)
						continue;
					if (UNIX_error == EAGAIN)
						break;
					return __RESULT_FAILURE;
				}
				if (ret < 0)
					break;
				if (ret == 0) {
					if (verbose > 4)
						print_success(child);
					if (verbose > 4) {
						int i;

						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from %d [%d,%d]:\n", child, ctrl.len, data.len);
						fprintf(stdout, "[");
						for (i = 0; i < ctrl.len; i++)
							fprintf(stdout, "%02X", ctrl.buf[i]);
						fprintf(stdout, "]\n");
						fprintf(stdout, "[");
						for (i = 0; i < data.len; i++)
							fprintf(stdout, "%02X", data.buf[i]);
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
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_0(int child)
{
	if (start_tt(TEST_DURATION) != __RESULT_SUCCESS)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
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
	if (stop_tt() != __RESULT_SUCCESS)
		goto failure;
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
	unsigned short port = htons(10000 + child);
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
	ADDR_length = sizeof(addrs[child]);
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
		failure_string = "bad CONIND_number";
		goto failure;
	}
	state++;
	if (TOKEN_value == 0) {
		failure_string = "bad TOKEN_value";
		goto failure;
	}
	state++;
	if (PROTOID_length != 0 && PROTOID_length != sizeof(prot)) {
		failure_string = "bad PROTOID_length";
		goto failure;
	}
	state++;
	if (PROTOID_length != 0 && PROTOID_buffer[0] != proto) {
		failure_string = "bad PROTOID_buffer";
		goto failure;
	}
	state++;
	if (ADDR_length == 0) {
		failure_string = "zero ADDR_length";
		goto failure;
	}
	state++;
	if (ADDR_length != sizeof(addrs[child])) {
		failure_string = "bad ADDR_length";
		goto failure;
	}
	state++;
	if (ADDR_buffer->sin_family != AF_INET) {
		failure_string = "bad sin_family";
		goto failure;
	}
	state++;
	if (ADDR_buffer->sin_addr.s_addr != addrs[child][0].sin_addr.s_addr) {
		failure_string = "bad sin_addr";
		goto failure;
	}
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.SERV_type != N_CONS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
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

	if (expect(child, SHORT_WAIT, __TEST_DISCON_IND) == __RESULT_SUCCESS)
		goto cannot_disconnect;
	state++;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto cannot_disconnect;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS) {
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
	if (expect(child, SHORT_WAIT, __TEST_DISCON_IND) == __RESULT_SUCCESS)
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
	ADDR_length = sizeof(addrs[2]);
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
preamble_1_data_xfer_resp(int child)
{
	if (preamble_1_wres_cind_resp(child) != __RESULT_SUCCESS)
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
	ADDR_length = sizeof(addrs[child]);
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

int
preamble_2_data_xfer_conn(int child)
{
	if (preamble_1_data_xfer_conn(child) != __RESULT_SUCCESS)
		goto failure;
	if (expect(child, NORMAL_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_2_data_xfer_resp(int child)
{
	if (preamble_1_data_xfer_resp(child) != __RESULT_SUCCESS)
		goto failure;
	if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_2_data_xfer_list(int child)
{
	if (preamble_1_data_xfer_list(child) != __RESULT_SUCCESS)
		goto failure;
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

/*
 *  Open and Close 3 streams.
 */
#define test_group_1 "1. Local management"
#define test_group_1_1 "1.1. Open and close"
#define tgrp_case_1_1 test_group_1
#define sgrp_case_1_1 test_group_1_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 3 streams"
#define sref_case_1_1 "(none)"
#define desc_case_1_1 "\
Checks that three streams can be opened and closed."

int
test_case_1_1(int child)
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
#define test_group_1_2 "1.2. Request information"
#define tgrp_case_1_2 test_group_1
#define sgrp_case_1_2 test_group_1_2
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Request information."
#define sref_case_1_2 "TPI Rev 1.5 Sections 2.1.1.1, 2.1.2.1"
#define desc_case_1_2 "\
Checks that information can be requested on each of three streams,\n\
and that the returned information is appropriate for each stream."

int
test_case_1_2(int child)
{
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.PRIM_type != N_INFO_ACK)
		goto failure;
	state++;
	switch (last_info.SERV_type) {
	case N_CLNS:
	case (N_CLNS | N_CONS):
		if (last_info.NSDU_size != 65515)
			goto failure;
		state++;
		if (last_info.ENSDU_size != 0)
			goto failure;
		state++;
		if (last_info.CDATA_size != 65515)
			goto failure;
		state++;
		if (last_info.DDATA_size != 65515)
			goto failure;
		state++;
		if (last_info.ADDR_size != sizeof(struct sockaddr_in) << 3)
			goto failure;
		state++;
		if (last_info.NODU_size != 536)
			goto failure;
		state++;
		if (last_info.NIDU_size != 65515)
			goto failure;
		state++;
		if (last_info.SERV_type != (N_CLNS | N_CONS))
			goto failure;
		state++;
		break;
	case N_CONS:
		if (last_info.NSDU_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.ENSDU_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.CDATA_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.DDATA_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.ADDR_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.NODU_size != 276)
			goto failure;
		state++;
		if (last_info.NIDU_size != (np_ulong) -1)
			goto failure;
		state++;
		if (last_info.SERV_type != N_CONS)
			goto failure;
		state++;
		break;
	}
	if (last_info.CURRENT_state != NS_UNBND)
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

#define test_group_1_3 "1.3. Non-fatal errors in the NS_UNBND State."
#define tgrp_case_1_3_1 test_group_1
#define sgrp_case_1_3_1 test_group_1_3
#define numb_case_1_3_1 "1.3.1"
#define name_case_1_3_1 "NOUTSTATE for N_CONN_REQ in NS_UNBND state."
#define sref_case_1_3_1 "NPI Rev 2.0.0"
#define desc_case_1_3_1 "\
Checks that NOUTSTATE is returned when N_CONN_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_1(int child)
{
	ADDR_buffer = NULL;
	QOS_buffer = NULL;
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_1_conn	test_case_1_3_1
#define test_case_1_3_1_resp	test_case_1_3_1
#define test_case_1_3_1_list	test_case_1_3_1

#define preamble_1_3_1_conn	preamble_1_unbnd
#define preamble_1_3_1_resp	preamble_1_unbnd
#define preamble_1_3_1_list	preamble_1_unbnd

#define postamble_1_3_1_conn	postamble_1_unbnd
#define postamble_1_3_1_resp	postamble_1_unbnd
#define postamble_1_3_1_list	postamble_1_unbnd

struct test_stream test_1_3_1_conn = { &preamble_1_3_1_conn, &test_case_1_3_1_conn, &postamble_1_3_1_conn };
struct test_stream test_1_3_1_resp = { &preamble_1_3_1_resp, &test_case_1_3_1_resp, &postamble_1_3_1_resp };
struct test_stream test_1_3_1_list = { &preamble_1_3_1_list, &test_case_1_3_1_list, &postamble_1_3_1_list };

#define tgrp_case_1_3_2 test_group_1
#define sgrp_case_1_3_2 test_group_1_3
#define numb_case_1_3_2 "1.3.2"
#define name_case_1_3_2 "NOUTSTATE for N_CONN_RES in NS_UNBND state."
#define sref_case_1_3_2 "NPI Rev 2.0.0"
#define desc_case_1_3_2 "\
Checks that NOUTSTATE is returned when N_CONN_RES is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_2(int child)
{
	TOKEN_value = 0;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	SEQ_number = 0;
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_2_conn	test_case_1_3_2
#define test_case_1_3_2_resp	test_case_1_3_2
#define test_case_1_3_2_list	test_case_1_3_2

#define preamble_1_3_2_conn	preamble_1_unbnd
#define preamble_1_3_2_resp	preamble_1_unbnd
#define preamble_1_3_2_list	preamble_1_unbnd

#define postamble_1_3_2_conn	postamble_1_unbnd
#define postamble_1_3_2_resp	postamble_1_unbnd
#define postamble_1_3_2_list	postamble_1_unbnd

struct test_stream test_1_3_2_conn = { &preamble_1_3_2_conn, &test_case_1_3_2_conn, &postamble_1_3_2_conn };
struct test_stream test_1_3_2_resp = { &preamble_1_3_2_resp, &test_case_1_3_2_resp, &postamble_1_3_2_resp };
struct test_stream test_1_3_2_list = { &preamble_1_3_2_list, &test_case_1_3_2_list, &postamble_1_3_2_list };

#define tgrp_case_1_3_3 test_group_1
#define sgrp_case_1_3_3 test_group_1_3
#define numb_case_1_3_3 "1.3.3"
#define name_case_1_3_3 "NOUTSTATE for N_DISCON_REQ in NS_UNBND state."
#define sref_case_1_3_3 "NPI Rev 2.0.0"
#define desc_case_1_3_3 "\
Checks that NOUTSTATE is returned when N_DISCON_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_3(int child)
{
	ADDR_buffer = NULL;
	SEQ_number = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_3_conn	test_case_1_3_3
#define test_case_1_3_3_resp	test_case_1_3_3
#define test_case_1_3_3_list	test_case_1_3_3

#define preamble_1_3_3_conn	preamble_1_unbnd
#define preamble_1_3_3_resp	preamble_1_unbnd
#define preamble_1_3_3_list	preamble_1_unbnd

#define postamble_1_3_3_conn	postamble_1_unbnd
#define postamble_1_3_3_resp	postamble_1_unbnd
#define postamble_1_3_3_list	postamble_1_unbnd

struct test_stream test_1_3_3_conn = { &preamble_1_3_3_conn, &test_case_1_3_3_conn, &postamble_1_3_3_conn };
struct test_stream test_1_3_3_resp = { &preamble_1_3_3_resp, &test_case_1_3_3_resp, &postamble_1_3_3_resp };
struct test_stream test_1_3_3_list = { &preamble_1_3_3_list, &test_case_1_3_3_list, &postamble_1_3_3_list };

#define tgrp_case_1_3_4 test_group_1
#define sgrp_case_1_3_4 test_group_1_3
#define numb_case_1_3_4 "1.3.4"
#define name_case_1_3_4 "NOUTSTATE for N_UNBIND_REQ in NS_UNBND state."
#define sref_case_1_3_4 "NPI Rev 2.0.0"
#define desc_case_1_3_4 "\
Checks that NOUTSTATE is returned when N_UNBIND_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_4(int child)
{
	if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_4_conn	test_case_1_3_4
#define test_case_1_3_4_resp	test_case_1_3_4
#define test_case_1_3_4_list	test_case_1_3_4

#define preamble_1_3_4_conn	preamble_1_unbnd
#define preamble_1_3_4_resp	preamble_1_unbnd
#define preamble_1_3_4_list	preamble_1_unbnd

#define postamble_1_3_4_conn	postamble_1_unbnd
#define postamble_1_3_4_resp	postamble_1_unbnd
#define postamble_1_3_4_list	postamble_1_unbnd

struct test_stream test_1_3_4_conn = { &preamble_1_3_4_conn, &test_case_1_3_4_conn, &postamble_1_3_4_conn };
struct test_stream test_1_3_4_resp = { &preamble_1_3_4_resp, &test_case_1_3_4_resp, &postamble_1_3_4_resp };
struct test_stream test_1_3_4_list = { &preamble_1_3_4_list, &test_case_1_3_4_list, &postamble_1_3_4_list };

#define tgrp_case_1_3_5 test_group_1
#define sgrp_case_1_3_5 test_group_1_3
#define numb_case_1_3_5 "1.3.5"
#define name_case_1_3_5 "NOUTSTATE for N_RESET_REQ in NS_UNBND state."
#define sref_case_1_3_5 "NPI Rev 2.0.0"
#define desc_case_1_3_5 "\
Checks that NOUTSTATE is returned when N_RESET_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_5(int child)
{
	RESET_reason = N_RESET_UNSPECIFIED;
	if (do_signal(child, __TEST_RESET_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_5_conn	test_case_1_3_5
#define test_case_1_3_5_resp	test_case_1_3_5
#define test_case_1_3_5_list	test_case_1_3_5

#define preamble_1_3_5_conn	preamble_1_unbnd
#define preamble_1_3_5_resp	preamble_1_unbnd
#define preamble_1_3_5_list	preamble_1_unbnd

#define postamble_1_3_5_conn	postamble_1_unbnd
#define postamble_1_3_5_resp	postamble_1_unbnd
#define postamble_1_3_5_list	postamble_1_unbnd

struct test_stream test_1_3_5_conn = { &preamble_1_3_5_conn, &test_case_1_3_5_conn, &postamble_1_3_5_conn };
struct test_stream test_1_3_5_resp = { &preamble_1_3_5_resp, &test_case_1_3_5_resp, &postamble_1_3_5_resp };
struct test_stream test_1_3_5_list = { &preamble_1_3_5_list, &test_case_1_3_5_list, &postamble_1_3_5_list };

#define tgrp_case_1_3_6 test_group_1
#define sgrp_case_1_3_6 test_group_1_3
#define numb_case_1_3_6 "1.3.6"
#define name_case_1_3_6 "NOUTSTATE for N_RESET_RES in NS_UNBND state."
#define sref_case_1_3_6 "NPI Rev 2.0.0"
#define desc_case_1_3_6 "\
Checks that NOUTSTATE is returned when N_RESET_RES is attempted in the\n\
NS_UNBND state."

int
test_case_1_3_6(int child)
{
	if (do_signal(child, __TEST_RESET_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_6_conn	test_case_1_3_6
#define test_case_1_3_6_resp	test_case_1_3_6
#define test_case_1_3_6_list	test_case_1_3_6

#define preamble_1_3_6_conn	preamble_1_unbnd
#define preamble_1_3_6_resp	preamble_1_unbnd
#define preamble_1_3_6_list	preamble_1_unbnd

#define postamble_1_3_6_conn	postamble_1_unbnd
#define postamble_1_3_6_resp	postamble_1_unbnd
#define postamble_1_3_6_list	postamble_1_unbnd

struct test_stream test_1_3_6_conn = { &preamble_1_3_6_conn, &test_case_1_3_6_conn, &postamble_1_3_6_conn };
struct test_stream test_1_3_6_resp = { &preamble_1_3_6_resp, &test_case_1_3_6_resp, &postamble_1_3_6_resp };
struct test_stream test_1_3_6_list = { &preamble_1_3_6_list, &test_case_1_3_6_list, &postamble_1_3_6_list };

#define test_group_1_4 "1.4. Fatal errors in the NS_UNBND State."
#define tgrp_case_1_4_1 test_group_1
#define sgrp_case_1_4_1 test_group_1_4
#define numb_case_1_4_1 "1.4.1"
#define name_case_1_4_1 "EPROTO for N_DATACK_REQ in NS_UNBND state."
#define sref_case_1_4_1 "NPI Rev 2.0.0"
#define desc_case_1_4_1 "\
Checks that EPROTO is returned when N_DATACK_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_4_1(int child)
{
	QOS_length = 0;
	if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_4_1_conn	test_case_1_4_1
#define test_case_1_4_1_resp	test_case_1_4_1
#define test_case_1_4_1_list	test_case_1_4_1

#define preamble_1_4_1_conn	preamble_1_unbnd
#define preamble_1_4_1_resp	preamble_1_unbnd
#define preamble_1_4_1_list	preamble_1_unbnd

#define postamble_1_4_1_conn	postamble_1_unbnd
#define postamble_1_4_1_resp	postamble_1_unbnd
#define postamble_1_4_1_list	postamble_1_unbnd

struct test_stream test_1_4_1_conn = { &preamble_1_4_1_conn, &test_case_1_4_1_conn, &postamble_1_4_1_conn };
struct test_stream test_1_4_1_resp = { &preamble_1_4_1_resp, &test_case_1_4_1_resp, &postamble_1_4_1_resp };
struct test_stream test_1_4_1_list = { &preamble_1_4_1_list, &test_case_1_4_1_list, &postamble_1_4_1_list };

#define tgrp_case_1_4_2 test_group_1
#define sgrp_case_1_4_2 test_group_1_4
#define numb_case_1_4_2 "1.4.2"
#define name_case_1_4_2 "EPROTO for N_DATA_REQ in NS_UNBND state."
#define sref_case_1_4_2 "NPI Rev 2.0.0"
#define desc_case_1_4_2 "\
Checks that EPROTO is returned when N_DATA_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_4_2(int child)
{
	DATA_buffer = "Test case 1.4.2";
	DATA_length = strlen(DATA_buffer);
	QOS_length = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_4_2_conn	test_case_1_4_2
#define test_case_1_4_2_resp	test_case_1_4_2
#define test_case_1_4_2_list	test_case_1_4_2

#define preamble_1_4_2_conn	preamble_1_unbnd
#define preamble_1_4_2_resp	preamble_1_unbnd
#define preamble_1_4_2_list	preamble_1_unbnd

#define postamble_1_4_2_conn	postamble_1_unbnd
#define postamble_1_4_2_resp	postamble_1_unbnd
#define postamble_1_4_2_list	postamble_1_unbnd

struct test_stream test_1_4_2_conn = { &preamble_1_4_2_conn, &test_case_1_4_2_conn, &postamble_1_4_2_conn };
struct test_stream test_1_4_2_resp = { &preamble_1_4_2_resp, &test_case_1_4_2_resp, &postamble_1_4_2_resp };
struct test_stream test_1_4_2_list = { &preamble_1_4_2_list, &test_case_1_4_2_list, &postamble_1_4_2_list };

#define tgrp_case_1_4_3 test_group_1
#define sgrp_case_1_4_3 test_group_1_4
#define numb_case_1_4_3 "1.4.3"
#define name_case_1_4_3 "EPROTO for N_EXDATA_REQ in NS_UNBND state."
#define sref_case_1_4_3 "NPI Rev 2.0.0"
#define desc_case_1_4_3 "\
Checks that EPROTO is returned when N_EXDATA_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_4_3(int child)
{
	DATA_buffer = "Test case 1.4.3";
	DATA_length = strlen(DATA_buffer);
	QOS_length = 0;
	if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_4_3_conn	test_case_1_4_3
#define test_case_1_4_3_resp	test_case_1_4_3
#define test_case_1_4_3_list	test_case_1_4_3

#define preamble_1_4_3_conn	preamble_1_unbnd
#define preamble_1_4_3_resp	preamble_1_unbnd
#define preamble_1_4_3_list	preamble_1_unbnd

#define postamble_1_4_3_conn	postamble_1_unbnd
#define postamble_1_4_3_resp	postamble_1_unbnd
#define postamble_1_4_3_list	postamble_1_unbnd

struct test_stream test_1_4_3_conn = { &preamble_1_4_3_conn, &test_case_1_4_3_conn, &postamble_1_4_3_conn };
struct test_stream test_1_4_3_resp = { &preamble_1_4_3_resp, &test_case_1_4_3_resp, &postamble_1_4_3_resp };
struct test_stream test_1_4_3_list = { &preamble_1_4_3_list, &test_case_1_4_3_list, &postamble_1_4_3_list };

#define tgrp_case_1_4_4 test_group_1
#define sgrp_case_1_4_4 test_group_1_4
#define numb_case_1_4_4 "1.4.4"
#define name_case_1_4_4 "EPROTO for M_DATA in NS_UNBND state."
#define sref_case_1_4_4 "NPI Rev 2.0.0"
#define desc_case_1_4_4 "\
Checks that EPROTO is returned when M_DATA is attempted in the\n\
NS_UNBND state."

int
test_case_1_4_4(int child)
{
	DATA_buffer = "Test case 1.4.4";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_WRITE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_4_4_conn	test_case_1_4_4
#define test_case_1_4_4_resp	test_case_1_4_4
#define test_case_1_4_4_list	test_case_1_4_4

#define preamble_1_4_4_conn	preamble_1_unbnd
#define preamble_1_4_4_resp	preamble_1_unbnd
#define preamble_1_4_4_list	preamble_1_unbnd

#define postamble_1_4_4_conn	postamble_1_unbnd
#define postamble_1_4_4_resp	postamble_1_unbnd
#define postamble_1_4_4_list	postamble_1_unbnd

struct test_stream test_1_4_4_conn = { &preamble_1_4_4_conn, &test_case_1_4_4_conn, &postamble_1_4_4_conn };
struct test_stream test_1_4_4_resp = { &preamble_1_4_4_resp, &test_case_1_4_4_resp, &postamble_1_4_4_resp };
struct test_stream test_1_4_4_list = { &preamble_1_4_4_list, &test_case_1_4_4_list, &postamble_1_4_4_list };

#define test_group_1_5 "1.5. Options Management in the NS_UNBND State."
#define test_group_1_5_1 "1.5.1. Options Management non-fatal errors in the NS_UNBND State."
#define tgrp_case_1_5_1_1 test_group_1
#define sgrp_case_1_5_1_1 test_group_1_5_1
#define numb_case_1_5_1_1 "1.5.1.1"
#define name_case_1_5_1_1 "NBADQOSTYPE for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_1_1 "NPI Rev 2.0.0"
#define desc_case_1_5_1_1 "\
Checks that NBADQOSTYPE is returned when N_OPTMGMT_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_5_1_1(int child)
{
#if 0
	np_ulong type = N_QOS_SEL_UD_IP + 10;
#else
	np_ulong type = N_QOS_SEL_DATA_SCTP + 10;
#endif

	QOS_buffer = &type;
	QOS_length = sizeof(type);
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADQOSTYPE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_1_1_conn	test_case_1_5_1_1
#define test_case_1_5_1_1_resp	test_case_1_5_1_1
#define test_case_1_5_1_1_list	test_case_1_5_1_1

#define preamble_1_5_1_1_conn	preamble_1_unbnd
#define preamble_1_5_1_1_resp	preamble_1_unbnd
#define preamble_1_5_1_1_list	preamble_1_unbnd

#define postamble_1_5_1_1_conn	postamble_1_unbnd
#define postamble_1_5_1_1_resp	postamble_1_unbnd
#define postamble_1_5_1_1_list	postamble_1_unbnd

struct test_stream test_1_5_1_1_conn = { &preamble_1_5_1_1_conn, &test_case_1_5_1_1_conn, &postamble_1_5_1_1_conn };
struct test_stream test_1_5_1_1_resp = { &preamble_1_5_1_1_resp, &test_case_1_5_1_1_resp, &postamble_1_5_1_1_resp };
struct test_stream test_1_5_1_1_list = { &preamble_1_5_1_1_list, &test_case_1_5_1_1_list, &postamble_1_5_1_1_list };

#define tgrp_case_1_5_1_2 test_group_1
#define sgrp_case_1_5_1_2 test_group_1_5_1
#define numb_case_1_5_1_2 "1.5.1.2"
#define name_case_1_5_1_2 "NBADQOSPARAM for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_1_2 "NPI Rev 2.0.0"
#define desc_case_1_5_1_2 "\
Checks that NBADQOSPARAM is returned when N_OPTMGMT_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_5_1_2(int child)
{
#if 0
	N_qos_sel_info_ip_t qos = {
		.n_qos_type = N_QOS_SEL_INFO_IP,
		.protocol = 2000,
		.priority = 1000,
		.ttl = 500,
		.tos = 128,
		.mtu = 128000,
		.saddr = 0,
		.daddr = 0,
	};
#else
	N_qos_sel_info_sctp_t qos = {
		.n_qos_type = N_QOS_SEL_INFO_SCTP,
	};
#endif

	QOS_buffer = &qos;
	QOS_length = sizeof(qos);
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADQOSPARAM)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_1_2_conn	test_case_1_5_1_2
#define test_case_1_5_1_2_resp	test_case_1_5_1_2
#define test_case_1_5_1_2_list	test_case_1_5_1_2

#define preamble_1_5_1_2_conn	preamble_1_unbnd
#define preamble_1_5_1_2_resp	preamble_1_unbnd
#define preamble_1_5_1_2_list	preamble_1_unbnd

#define postamble_1_5_1_2_conn	postamble_1_unbnd
#define postamble_1_5_1_2_resp	postamble_1_unbnd
#define postamble_1_5_1_2_list	postamble_1_unbnd

struct test_stream test_1_5_1_2_conn = { &preamble_1_5_1_2_conn, &test_case_1_5_1_2_conn, &postamble_1_5_1_2_conn };
struct test_stream test_1_5_1_2_resp = { &preamble_1_5_1_2_resp, &test_case_1_5_1_2_resp, &postamble_1_5_1_2_resp };
struct test_stream test_1_5_1_2_list = { &preamble_1_5_1_2_list, &test_case_1_5_1_2_list, &postamble_1_5_1_2_list };

#define tgrp_case_1_5_1_3 test_group_1
#define sgrp_case_1_5_1_3 test_group_1_5_1
#define numb_case_1_5_1_3 "1.5.1.3"
#define name_case_1_5_1_3 "EINVAL for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_1_3 "NPI Rev 2.0.0"
#define desc_case_1_5_1_3 "\
Checks that EINVAL is returned when N_OPTMGMT_REQ is attempted in the\n\
NS_UNBND state."

int
test_case_1_5_1_3(int child)
{
	PRIM_type = N_OPTMGMT_REQ;
	if (do_signal(child, __TEST_PRIM_TOO_SHORT) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NSYSERR)
		goto failure;
	state++;
	if (UNIX_error != EINVAL)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_1_3_conn	test_case_1_5_1_3
#define test_case_1_5_1_3_resp	test_case_1_5_1_3
#define test_case_1_5_1_3_list	test_case_1_5_1_3

#define preamble_1_5_1_3_conn	preamble_1_unbnd
#define preamble_1_5_1_3_resp	preamble_1_unbnd
#define preamble_1_5_1_3_list	preamble_1_unbnd

#define postamble_1_5_1_3_conn	postamble_1_unbnd
#define postamble_1_5_1_3_resp	postamble_1_unbnd
#define postamble_1_5_1_3_list	postamble_1_unbnd

struct test_stream test_1_5_1_3_conn = { &preamble_1_5_1_3_conn, &test_case_1_5_1_3_conn, &postamble_1_5_1_3_conn };
struct test_stream test_1_5_1_3_resp = { &preamble_1_5_1_3_resp, &test_case_1_5_1_3_resp, &postamble_1_5_1_3_resp };
struct test_stream test_1_5_1_3_list = { &preamble_1_5_1_3_list, &test_case_1_5_1_3_list, &postamble_1_5_1_3_list };

#define test_group_1_5_2 "1.5.2. Options Management correct operation in the NS_UNBND State."
#define tgrp_case_1_5_2_1 test_group_1
#define sgrp_case_1_5_2_1 test_group_1_5_2
#define numb_case_1_5_2_1 "1.5.2.1"
#define name_case_1_5_2_1 "N_QOS_SEL_INFO_IP for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_2_1 "NPI Rev 2.0.0"
#define desc_case_1_5_2_1 "\
Checks that the N_QOS_SEL_INFO_IP structure can be used with N_OPTMGMT_REQ\n\
correctly in the NS_UNBND state.  Check that SERV_type is unaffected."

int
test_case_1_5_2_1(int child)
{
#if 0
	N_qos_sel_info_ip_t test_qos = {
		.n_qos_type = N_QOS_SEL_INFO_IP,
		.protocol = QOS_UNKNOWN,
		.priority = 0,
		.ttl = 64,
		.tos = N_ROUTINE|N_NOTOS,
		.mtu = 536,
		.saddr = QOS_UNKNOWN,
		.daddr = QOS_UNKNOWN,
	};
#else
	N_qos_sel_info_sctp_t test_qos = {
		.n_qos_type = N_QOS_SEL_INFO_SCTP,
		.i_streams = -1,
		.o_streams = -1,
		.ppi = -1,
		.sid = -1,
		.max_inits = -1,
		.max_retrans = -1,
		.max_sack = -1,
		.ck_life = -1,
		.ck_inc = -1,
		.hmac = -1,
		.throttle = -1,
		.rto_ini = -1,
		.rto_min = -1,
		.rto_max = -1,
		.rtx_path = -1,
		.hb_itvl = -1,
		.options = -1,
	};
#endif

	QOS_buffer = &test_qos;
	QOS_length = sizeof(test_qos);
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
	if (last_info.SERV_type != N_CONS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_2_1_conn	test_case_1_5_2_1
#define test_case_1_5_2_1_resp	test_case_1_5_2_1
#define test_case_1_5_2_1_list	test_case_1_5_2_1

#define preamble_1_5_2_1_conn	preamble_1_unbnd
#define preamble_1_5_2_1_resp	preamble_1_unbnd
#define preamble_1_5_2_1_list	preamble_1_unbnd

#define postamble_1_5_2_1_conn	postamble_1_unbnd
#define postamble_1_5_2_1_resp	postamble_1_unbnd
#define postamble_1_5_2_1_list	postamble_1_unbnd

struct test_stream test_1_5_2_1_conn = { &preamble_1_5_2_1_conn, &test_case_1_5_2_1_conn, &postamble_1_5_2_1_conn };
struct test_stream test_1_5_2_1_resp = { &preamble_1_5_2_1_resp, &test_case_1_5_2_1_resp, &postamble_1_5_2_1_resp };
struct test_stream test_1_5_2_1_list = { &preamble_1_5_2_1_list, &test_case_1_5_2_1_list, &postamble_1_5_2_1_list };

#define tgrp_case_1_5_2_2 test_group_1
#define sgrp_case_1_5_2_2 test_group_1_5_2
#define numb_case_1_5_2_2 "1.5.2.2"
#define name_case_1_5_2_2 "N_QOS_SEL_UD_IP for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_2_2 "NPI Rev 2.0.0"
#define desc_case_1_5_2_2 "\
Checks that the N_QOS_SEL_UD_IP structure can be used with N_OPTMGMT_REQ\n\
correctly in the NS_UNBND state.  Check that SERV_type becomes N_CLNS."

int
test_case_1_5_2_2(int child)
{
#if 0
	N_qos_sel_ud_ip_t test_qos = {
		.n_qos_type = N_QOS_SEL_UD_IP,
		.protocol = QOS_UNKNOWN,
		.priority = 0,
		.ttl = 63,
		.tos = N_ROUTINE|N_NOTOS,
		.saddr = QOS_UNKNOWN,
	};
#else
	N_qos_sel_data_sctp_t test_qos = {
		.n_qos_type = N_QOS_SEL_DATA_SCTP,
	};
#endif

	return (__RESULT_NOTAPPL);
	QOS_buffer = &test_qos;
	QOS_length = sizeof(test_qos);
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
	if (last_info.SERV_type != N_CLNS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_2_2_conn	test_case_1_5_2_2
#define test_case_1_5_2_2_resp	test_case_1_5_2_2
#define test_case_1_5_2_2_list	test_case_1_5_2_2

#define preamble_1_5_2_2_conn	preamble_1_unbnd
#define preamble_1_5_2_2_resp	preamble_1_unbnd
#define preamble_1_5_2_2_list	preamble_1_unbnd

#define postamble_1_5_2_2_conn	postamble_1_unbnd
#define postamble_1_5_2_2_resp	postamble_1_unbnd
#define postamble_1_5_2_2_list	postamble_1_unbnd

struct test_stream test_1_5_2_2_conn = { &preamble_1_5_2_2_conn, &test_case_1_5_2_2_conn, &postamble_1_5_2_2_conn };
struct test_stream test_1_5_2_2_resp = { &preamble_1_5_2_2_resp, &test_case_1_5_2_2_resp, &postamble_1_5_2_2_resp };
struct test_stream test_1_5_2_2_list = { &preamble_1_5_2_2_list, &test_case_1_5_2_2_list, &postamble_1_5_2_2_list };

#define tgrp_case_1_5_2_3 test_group_1
#define sgrp_case_1_5_2_3 test_group_1_5_2
#define numb_case_1_5_2_3 "1.5.2.3"
#define name_case_1_5_2_3 "N_QOS_SEL_CONN_IP for N_OPTMGMT_REQ in NS_UNBND state."
#define sref_case_1_5_2_3 "NPI Rev 2.0.0"
#define desc_case_1_5_2_3 "\
Checks that the N_QOS_SEL_CONN_IP structure can be used with N_OPTMGMT_REQ\n\
correctly in the NS_UNBND state.  Check that SERV_type becomes N_CONS."

int
test_case_1_5_2_3(int child)
{
#if 0
	N_qos_sel_conn_ip_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_IP,
		.protocol = QOS_UNKNOWN,
		.priority = 0,
		.ttl = 64,
		.tos = N_ROUTINE|N_NOTOS,
		.mtu = 536,
		.saddr = QOS_UNKNOWN,
		.daddr = QOS_UNKNOWN,
	};
#else
	N_qos_sel_conn_sctp_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_SCTP,
	};
#endif

	return (__RESULT_NOTAPPL);
	QOS_buffer = &test_qos;
	QOS_length = sizeof(test_qos);
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
	if (last_info.SERV_type != N_CONS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_5_2_3_conn	test_case_1_5_2_3
#define test_case_1_5_2_3_resp	test_case_1_5_2_3
#define test_case_1_5_2_3_list	test_case_1_5_2_3

#define preamble_1_5_2_3_conn	preamble_1_unbnd
#define preamble_1_5_2_3_resp	preamble_1_unbnd
#define preamble_1_5_2_3_list	preamble_1_unbnd

#define postamble_1_5_2_3_conn	postamble_1_unbnd
#define postamble_1_5_2_3_resp	postamble_1_unbnd
#define postamble_1_5_2_3_list	postamble_1_unbnd

struct test_stream test_1_5_2_3_conn = { &preamble_1_5_2_3_conn, &test_case_1_5_2_3_conn, &postamble_1_5_2_3_conn };
struct test_stream test_1_5_2_3_resp = { &preamble_1_5_2_3_resp, &test_case_1_5_2_3_resp, &postamble_1_5_2_3_resp };
struct test_stream test_1_5_2_3_list = { &preamble_1_5_2_3_list, &test_case_1_5_2_3_list, &postamble_1_5_2_3_list };

#define test_group_1_6_1 "1.6.1. Bind request non-fatal errors in the NS_UNBND State."
#define tgrp_case_1_6_1_1 test_group_1
#define sgrp_case_1_6_1_1 test_group_1_6_1
#define numb_case_1_6_1_1 "1.6.1.1"
#define name_case_1_6_1_1 "EINVAL for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_1 "NPI Rev 2.0.0"
#define desc_case_1_6_1_1 "\
Checks that EINVAL is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_1(int child)
{
	PRIM_type = N_BIND_REQ;
	if (do_signal(child, __TEST_PRIM_TOO_SHORT) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NSYSERR)
		goto failure;
	state++;
	if (UNIX_error != EINVAL)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_1_conn	test_case_1_6_1_1
#define test_case_1_6_1_1_resp	test_case_1_6_1_1
#define test_case_1_6_1_1_list	test_case_1_6_1_1

#define preamble_1_6_1_1_conn	preamble_1_unbnd
#define preamble_1_6_1_1_resp	preamble_1_unbnd
#define preamble_1_6_1_1_list	preamble_1_unbnd

#define postamble_1_6_1_1_conn	postamble_1_unbnd
#define postamble_1_6_1_1_resp	postamble_1_unbnd
#define postamble_1_6_1_1_list	postamble_1_unbnd

struct test_stream test_1_6_1_1_conn = { &preamble_1_6_1_1_conn, &test_case_1_6_1_1_conn, &postamble_1_6_1_1_conn };
struct test_stream test_1_6_1_1_resp = { &preamble_1_6_1_1_resp, &test_case_1_6_1_1_resp, &postamble_1_6_1_1_resp };
struct test_stream test_1_6_1_1_list = { &preamble_1_6_1_1_list, &test_case_1_6_1_1_list, &postamble_1_6_1_1_list };

#define tgrp_case_1_6_1_2 test_group_1
#define sgrp_case_1_6_1_2 test_group_1_6_1
#define numb_case_1_6_1_2 "1.6.1.2"
#define name_case_1_6_1_2 "NBADADDR for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_2 "NPI Rev 2.0.0"
#define desc_case_1_6_1_2 "\
Checks that NBADADDR is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_2(int child)
{
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin) - 1;
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_2_conn	test_case_1_6_1_2
#define test_case_1_6_1_2_resp	test_case_1_6_1_2
#define test_case_1_6_1_2_list	test_case_1_6_1_2

#define preamble_1_6_1_2_conn	preamble_1_unbnd
#define preamble_1_6_1_2_resp	preamble_1_unbnd
#define preamble_1_6_1_2_list	preamble_1_unbnd

#define postamble_1_6_1_2_conn	postamble_1_unbnd
#define postamble_1_6_1_2_resp	postamble_1_unbnd
#define postamble_1_6_1_2_list	postamble_1_unbnd

struct test_stream test_1_6_1_2_conn = { &preamble_1_6_1_2_conn, &test_case_1_6_1_2_conn, &postamble_1_6_1_2_conn };
struct test_stream test_1_6_1_2_resp = { &preamble_1_6_1_2_resp, &test_case_1_6_1_2_resp, &postamble_1_6_1_2_resp };
struct test_stream test_1_6_1_2_list = { &preamble_1_6_1_2_list, &test_case_1_6_1_2_list, &postamble_1_6_1_2_list };

#define tgrp_case_1_6_1_3 test_group_1
#define sgrp_case_1_6_1_3 test_group_1_6_1
#define numb_case_1_6_1_3 "1.6.1.3"
#define name_case_1_6_1_3 "NBOUND for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_3 "NPI Rev 2.0.0"
#define desc_case_1_6_1_3 "\
Checks that NBOUND is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_3(int child)
{
#if 0
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBOUND)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
#else
	return (__RESULT_SKIPPED);
#endif
}

#define test_case_1_6_1_3_conn	test_case_1_6_1_3
#define test_case_1_6_1_3_resp	test_case_1_6_1_3
#define test_case_1_6_1_3_list	test_case_1_6_1_3

#define preamble_1_6_1_3_conn	preamble_1_unbnd
#define preamble_1_6_1_3_resp	preamble_1_unbnd
#define preamble_1_6_1_3_list	preamble_1_unbnd

#define postamble_1_6_1_3_conn	postamble_1_unbnd
#define postamble_1_6_1_3_resp	postamble_1_unbnd
#define postamble_1_6_1_3_list	postamble_1_unbnd

struct test_stream test_1_6_1_3_conn = { &preamble_1_6_1_3_conn, &test_case_1_6_1_3_conn, &postamble_1_6_1_3_conn };
struct test_stream test_1_6_1_3_resp = { &preamble_1_6_1_3_resp, &test_case_1_6_1_3_resp, &postamble_1_6_1_3_resp };
struct test_stream test_1_6_1_3_list = { &preamble_1_6_1_3_list, &test_case_1_6_1_3_list, &postamble_1_6_1_3_list };

#define tgrp_case_1_6_1_4 test_group_1
#define sgrp_case_1_6_1_4 test_group_1_6_1
#define numb_case_1_6_1_4 "1.6.1.4"
#define name_case_1_6_1_4 "NNOADDR for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_4 "NPI Rev 2.0.0"
#define desc_case_1_6_1_4 "\
Checks that NNOADDR is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_4(int child)
{
#if 0
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
#else
	return (__RESULT_NOTAPPL);
#endif
}

#define test_case_1_6_1_4_conn	test_case_1_6_1_4
#define test_case_1_6_1_4_resp	test_case_1_6_1_4
#define test_case_1_6_1_4_list	test_case_1_6_1_4

#define preamble_1_6_1_4_conn	preamble_1_unbnd
#define preamble_1_6_1_4_resp	preamble_1_unbnd
#define preamble_1_6_1_4_list	preamble_1_unbnd

#define postamble_1_6_1_4_conn	postamble_1_unbnd
#define postamble_1_6_1_4_resp	postamble_1_unbnd
#define postamble_1_6_1_4_list	postamble_1_unbnd

struct test_stream test_1_6_1_4_conn = { &preamble_1_6_1_4_conn, &test_case_1_6_1_4_conn, &postamble_1_6_1_4_conn };
struct test_stream test_1_6_1_4_resp = { &preamble_1_6_1_4_resp, &test_case_1_6_1_4_resp, &postamble_1_6_1_4_resp };
struct test_stream test_1_6_1_4_list = { &preamble_1_6_1_4_list, &test_case_1_6_1_4_list, &postamble_1_6_1_4_list };

#define tgrp_case_1_6_1_5 test_group_1
#define sgrp_case_1_6_1_5 test_group_1_6_1
#define numb_case_1_6_1_5 "1.6.1.5"
#define name_case_1_6_1_5 "NACCESS for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_5 "NPI Rev 2.0.0"
#define desc_case_1_6_1_5 "\
Checks that NACCESS is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_5(int child)
{
#if 0
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NACCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
#else
	return (__RESULT_NOTAPPL);
#endif
}

#define test_case_1_6_1_5_conn	test_case_1_6_1_5
#define test_case_1_6_1_5_resp	test_case_1_6_1_5
#define test_case_1_6_1_5_list	test_case_1_6_1_5

#define preamble_1_6_1_5_conn	preamble_1_unbnd
#define preamble_1_6_1_5_resp	preamble_1_unbnd
#define preamble_1_6_1_5_list	preamble_1_unbnd

#define postamble_1_6_1_5_conn	postamble_1_unbnd
#define postamble_1_6_1_5_resp	postamble_1_unbnd
#define postamble_1_6_1_5_list	postamble_1_unbnd

struct test_stream test_1_6_1_5_conn = { &preamble_1_6_1_5_conn, &test_case_1_6_1_5_conn, &postamble_1_6_1_5_conn };
struct test_stream test_1_6_1_5_resp = { &preamble_1_6_1_5_resp, &test_case_1_6_1_5_resp, &postamble_1_6_1_5_resp };
struct test_stream test_1_6_1_5_list = { &preamble_1_6_1_5_list, &test_case_1_6_1_5_list, &postamble_1_6_1_5_list };

#define tgrp_case_1_6_1_6 test_group_1
#define sgrp_case_1_6_1_6 test_group_1_6_1
#define numb_case_1_6_1_6 "1.6.1.6"
#define name_case_1_6_1_6 "NNOPROTOID for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_6 "NPI Rev 2.0.0"
#define desc_case_1_6_1_6 "\
Checks that NNOPROTOID is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_6(int child)
{
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	return (__RESULT_NOTAPPL);
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = 0;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOPROTOID)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_6_conn	test_case_1_6_1_6
#define test_case_1_6_1_6_resp	test_case_1_6_1_6
#define test_case_1_6_1_6_list	test_case_1_6_1_6

#define preamble_1_6_1_6_conn	preamble_1_unbnd
#define preamble_1_6_1_6_resp	preamble_1_unbnd
#define preamble_1_6_1_6_list	preamble_1_unbnd

#define postamble_1_6_1_6_conn	postamble_1_unbnd
#define postamble_1_6_1_6_resp	postamble_1_unbnd
#define postamble_1_6_1_6_list	postamble_1_unbnd

struct test_stream test_1_6_1_6_conn = { &preamble_1_6_1_6_conn, &test_case_1_6_1_6_conn, &postamble_1_6_1_6_conn };
struct test_stream test_1_6_1_6_resp = { &preamble_1_6_1_6_resp, &test_case_1_6_1_6_resp, &postamble_1_6_1_6_resp };
struct test_stream test_1_6_1_6_list = { &preamble_1_6_1_6_list, &test_case_1_6_1_6_list, &postamble_1_6_1_6_list };

#define tgrp_case_1_6_1_7 test_group_1
#define sgrp_case_1_6_1_7 test_group_1_6_1
#define numb_case_1_6_1_7 "1.6.1.7"
#define name_case_1_6_1_7 "NBADFLAG for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_7 "NPI Rev 2.0.0"
#define desc_case_1_6_1_7 "\
Checks that NBADFLAG is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state."

int
test_case_1_6_1_7(int child)
{
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0xff;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADFLAG)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_7_conn	test_case_1_6_1_7
#define test_case_1_6_1_7_resp	test_case_1_6_1_7
#define test_case_1_6_1_7_list	test_case_1_6_1_7

#define preamble_1_6_1_7_conn	preamble_1_unbnd
#define preamble_1_6_1_7_resp	preamble_1_unbnd
#define preamble_1_6_1_7_list	preamble_1_unbnd

#define postamble_1_6_1_7_conn	postamble_1_unbnd
#define postamble_1_6_1_7_resp	postamble_1_unbnd
#define postamble_1_6_1_7_list	postamble_1_unbnd

struct test_stream test_1_6_1_7_conn = { &preamble_1_6_1_7_conn, &test_case_1_6_1_7_conn, &postamble_1_6_1_7_conn };
struct test_stream test_1_6_1_7_resp = { &preamble_1_6_1_7_resp, &test_case_1_6_1_7_resp, &postamble_1_6_1_7_resp };
struct test_stream test_1_6_1_7_list = { &preamble_1_6_1_7_list, &test_case_1_6_1_7_list, &postamble_1_6_1_7_list };

#define tgrp_case_1_6_1_8 test_group_1
#define sgrp_case_1_6_1_8 test_group_1_6_1
#define numb_case_1_6_1_8 "1.6.1.8"
#define name_case_1_6_1_8 "NBADADDR for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_8 "NPI Rev 2.0.0"
#define desc_case_1_6_1_8 "\
Checks that NBADADDR is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state.  Address specified with DEFAULT_LISTENER flag\n\
set."

int
test_case_1_6_1_8(int child)
{
	struct sockaddr_in sin = { AF_INET, 10000, { 0x0000007f } };
	unsigned char prot[] = { TEST_PROTOCOL };

	return (__RESULT_NOTAPPL);
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = DEFAULT_LISTENER;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_8_conn	test_case_1_6_1_8
#define test_case_1_6_1_8_resp	test_case_1_6_1_8
#define test_case_1_6_1_8_list	test_case_1_6_1_8

#define preamble_1_6_1_8_conn	preamble_1_unbnd
#define preamble_1_6_1_8_resp	preamble_1_unbnd
#define preamble_1_6_1_8_list	preamble_1_unbnd

#define postamble_1_6_1_8_conn	postamble_1_unbnd
#define postamble_1_6_1_8_resp	postamble_1_unbnd
#define postamble_1_6_1_8_list	postamble_1_unbnd

struct test_stream test_1_6_1_8_conn = { &preamble_1_6_1_8_conn, &test_case_1_6_1_8_conn, &postamble_1_6_1_8_conn };
struct test_stream test_1_6_1_8_resp = { &preamble_1_6_1_8_resp, &test_case_1_6_1_8_resp, &postamble_1_6_1_8_resp };
struct test_stream test_1_6_1_8_list = { &preamble_1_6_1_8_list, &test_case_1_6_1_8_list, &postamble_1_6_1_8_list };

#define tgrp_case_1_6_1_9 test_group_1
#define sgrp_case_1_6_1_9 test_group_1_6_1
#define numb_case_1_6_1_9 "1.6.1.9"
#define name_case_1_6_1_9 "NBADADDR for N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_1_9 "NPI Rev 2.0.0"
#define desc_case_1_6_1_9 "\
Checks that NBADADDR is returned under proper conditions when N_BIND_REQ is\n\
issued in the NS_UNBND state.  Default destination with specified port."

int
test_case_1_6_1_9(int child)
{
	struct sockaddr_in sin = { AF_INET, 10000, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL };

	return (__RESULT_NOTAPPL);
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = DEFAULT_DEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NBADADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_1_9_conn	test_case_1_6_1_9
#define test_case_1_6_1_9_resp	test_case_1_6_1_9
#define test_case_1_6_1_9_list	test_case_1_6_1_9

#define preamble_1_6_1_9_conn	preamble_1_unbnd
#define preamble_1_6_1_9_resp	preamble_1_unbnd
#define preamble_1_6_1_9_list	preamble_1_unbnd

#define postamble_1_6_1_9_conn	postamble_1_unbnd
#define postamble_1_6_1_9_resp	postamble_1_unbnd
#define postamble_1_6_1_9_list	postamble_1_unbnd

struct test_stream test_1_6_1_9_conn = { &preamble_1_6_1_9_conn, &test_case_1_6_1_9_conn, &postamble_1_6_1_9_conn };
struct test_stream test_1_6_1_9_resp = { &preamble_1_6_1_9_resp, &test_case_1_6_1_9_resp, &postamble_1_6_1_9_resp };
struct test_stream test_1_6_1_9_list = { &preamble_1_6_1_9_list, &test_case_1_6_1_9_list, &postamble_1_6_1_9_list };

#define test_group_1_6_2 "1.6.2. Successful bind request in the NS_UNBND State."
#define tgrp_case_1_6_2_1 test_group_1
#define sgrp_case_1_6_2_1 test_group_1_6_2
#define numb_case_1_6_2_1 "1.6.2.1"
#define name_case_1_6_2_1 "Missing address N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_2_1 "NPI Rev 2.0.0"
#define desc_case_1_6_2_1 "\
Checks that the bind is successful when N_BIND_REQ is issued with no address\n\
in the NS_UNBND state."

int
test_case_1_6_2_1(int child)
{
	unsigned char prot[] = { TEST_PROTOCOL + child };

	ADDR_buffer = NULL;
	ADDR_length = 0;
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
#if 0
	if (TOKEN_value != 0)
		goto failure;
	state++;
	if (PROTOID_length != 1)
		goto failure;
	state++;
	if (PROTOID_buffer[0] != TEST_PROTOCOL + child)
		goto failure;
	state++;
#endif
	if (ADDR_length != sizeof(struct sockaddr_in))
		goto failure;
	state++;
	if (ADDR_buffer->sin_family != AF_INET)
		goto failure;
	state++;
#if 0
	if (ADDR_buffer->sin_port != 0)
		goto failure;
	state++;
#endif
	if (ADDR_buffer->sin_addr.s_addr != INADDR_ANY)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
#if 0
	if (last_info.SERV_type != N_CLNS)
		goto failure;
#else
	if (last_info.SERV_type != N_CONS)
		goto failure;
#endif
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_2_1_conn	test_case_1_6_2_1
#define test_case_1_6_2_1_resp	test_case_1_6_2_1
#define test_case_1_6_2_1_list	test_case_1_6_2_1

#define preamble_1_6_2_1_conn	preamble_1_unbnd
#define preamble_1_6_2_1_resp	preamble_1_unbnd
#define preamble_1_6_2_1_list	preamble_1_unbnd

#define postamble_1_6_2_1_conn	postamble_1_idle
#define postamble_1_6_2_1_resp	postamble_1_idle
#define postamble_1_6_2_1_list	postamble_1_idle

struct test_stream test_1_6_2_1_conn = { &preamble_1_6_2_1_conn, &test_case_1_6_2_1_conn, &postamble_1_6_2_1_conn };
struct test_stream test_1_6_2_1_resp = { &preamble_1_6_2_1_resp, &test_case_1_6_2_1_resp, &postamble_1_6_2_1_resp };
struct test_stream test_1_6_2_1_list = { &preamble_1_6_2_1_list, &test_case_1_6_2_1_list, &postamble_1_6_2_1_list };

#define tgrp_case_1_6_2_2 test_group_1
#define sgrp_case_1_6_2_2 test_group_1_6_2
#define numb_case_1_6_2_2 "1.6.2.2"
#define name_case_1_6_2_2 "Wildcard N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_2_2 "NPI Rev 2.0.0"
#define desc_case_1_6_2_2 "\
Checks that the bind is successful when N_BIND_REQ is issued with a wildcard\n\
address in the NS_UNBND state."

int
test_case_1_6_2_2(int child)
{
	struct sockaddr_in sin = { AF_INET, 0, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL + child };

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
#if 0
	if (TOKEN_value != 0)
		goto failure;
	state++;
	if (PROTOID_length != 1)
		goto failure;
	state++;
	if (PROTOID_buffer[0] != TEST_PROTOCOL + child)
		goto failure;
	state++;
#endif
	if (ADDR_length != sizeof(struct sockaddr_in))
		goto failure;
	state++;
	if (ADDR_buffer->sin_family != AF_INET)
		goto failure;
	state++;
#if 0
	if (ADDR_buffer->sin_port != 0)
		goto failure;
	state++;
#endif
	if (ADDR_buffer->sin_addr.s_addr != INADDR_ANY)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
#if 0
	if (last_info.SERV_type != N_CLNS)
		goto failure;
#else
	if (last_info.SERV_type != N_CONS)
		goto failure;
#endif
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_2_2_conn	test_case_1_6_2_2
#define test_case_1_6_2_2_resp	test_case_1_6_2_2
#define test_case_1_6_2_2_list	test_case_1_6_2_2

#define preamble_1_6_2_2_conn	preamble_1_unbnd
#define preamble_1_6_2_2_resp	preamble_1_unbnd
#define preamble_1_6_2_2_list	preamble_1_unbnd

#define postamble_1_6_2_2_conn	postamble_1_idle
#define postamble_1_6_2_2_resp	postamble_1_idle
#define postamble_1_6_2_2_list	postamble_1_idle

struct test_stream test_1_6_2_2_conn = { &preamble_1_6_2_2_conn, &test_case_1_6_2_2_conn, &postamble_1_6_2_2_conn };
struct test_stream test_1_6_2_2_resp = { &preamble_1_6_2_2_resp, &test_case_1_6_2_2_resp, &postamble_1_6_2_2_resp };
struct test_stream test_1_6_2_2_list = { &preamble_1_6_2_2_list, &test_case_1_6_2_2_list, &postamble_1_6_2_2_list };

#define tgrp_case_1_6_2_3 test_group_1
#define sgrp_case_1_6_2_3 test_group_1_6_2
#define numb_case_1_6_2_3 "1.6.2.3"
#define name_case_1_6_2_3 "Wildcard N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_2_3 "NPI Rev 2.0.0"
#define desc_case_1_6_2_3 "\
Checks that the bind is successful when N_BIND_REQ is issued with a wildcard\n\
address but a specific port number in the NS_UNBND state."

int
test_case_1_6_2_3(int child)
{
	int port = htons(10000);
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL + child };

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
#if 0
	if (TOKEN_value != 0)
		goto failure;
	state++;
	if (PROTOID_length != 1)
		goto failure;
	state++;
	if (PROTOID_buffer[0] != TEST_PROTOCOL + child)
		goto failure;
	state++;
#endif
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
#if 0
	if (last_info.SERV_type != N_CLNS)
		goto failure;
#else
	if (last_info.SERV_type != N_CONS)
		goto failure;
#endif
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_2_3_conn	test_case_1_6_2_3
#define test_case_1_6_2_3_resp	test_case_1_6_2_3
#define test_case_1_6_2_3_list	test_case_1_6_2_3

#define preamble_1_6_2_3_conn	preamble_1_unbnd
#define preamble_1_6_2_3_resp	preamble_1_unbnd
#define preamble_1_6_2_3_list	preamble_1_unbnd

#define postamble_1_6_2_3_conn	postamble_1_idle
#define postamble_1_6_2_3_resp	postamble_1_idle
#define postamble_1_6_2_3_list	postamble_1_idle

struct test_stream test_1_6_2_3_conn = { &preamble_1_6_2_3_conn, &test_case_1_6_2_3_conn, &postamble_1_6_2_3_conn };
struct test_stream test_1_6_2_3_resp = { &preamble_1_6_2_3_resp, &test_case_1_6_2_3_resp, &postamble_1_6_2_3_resp };
struct test_stream test_1_6_2_3_list = { &preamble_1_6_2_3_list, &test_case_1_6_2_3_list, &postamble_1_6_2_3_list };

#define tgrp_case_1_6_2_4 test_group_1
#define sgrp_case_1_6_2_4 test_group_1_6_2
#define numb_case_1_6_2_4 "1.6.2.4"
#define name_case_1_6_2_4 "DEFAULT_DEST N_BIND_REQ in NS_UNBND state."
#define sref_case_1_6_2_4 "NPI Rev 2.0.0"
#define desc_case_1_6_2_4 "\
Checks that two Streams can be bound to the wildcard address when DEFAULT_DEST\n\
is set on one, DEFAULT_LISTENER on another, and none on the last.  N_BIND_REQ\n\
is properly issued in the NS_UNBND state."

int
test_case_1_6_2_4(int child)
{
	int port = 0; // htons(10000);
	int proto = TEST_PROTOCOL;
	int flag = (child == 0) ? DEFAULT_DEST : ((child == 2) ? DEFAULT_LISTENER : 0) ;
	int serv = (flag == DEFAULT_LISTENER) ? N_CONS : N_CLNS;
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { proto };

#if 1
	flag = 0;
	serv = N_CONS;
#endif
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = flag;
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
#if 0
	if (TOKEN_value != 0)
		goto failure;
	state++;
	if (PROTOID_length != 1)
		goto failure;
	state++;
	if (PROTOID_buffer[0] != proto)
		goto failure;
	state++;
#endif
	if (ADDR_length != sizeof(struct sockaddr_in))
		goto failure;
	state++;
	if (ADDR_buffer->sin_family != AF_INET)
		goto failure;
	state++;
#if 0
	if (ADDR_buffer->sin_port != port)
		goto failure;
	state++;
#endif
	if (ADDR_buffer->sin_addr.s_addr != INADDR_ANY)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.SERV_type != serv)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_2_4_conn	test_case_1_6_2_4
#define test_case_1_6_2_4_resp	test_case_1_6_2_4
#define test_case_1_6_2_4_list	test_case_1_6_2_4

#define preamble_1_6_2_4_conn	preamble_1_unbnd
#define preamble_1_6_2_4_resp	preamble_1_unbnd
#define preamble_1_6_2_4_list	preamble_1_unbnd

#define postamble_1_6_2_4_conn	postamble_1_idle
#define postamble_1_6_2_4_resp	postamble_1_idle
#define postamble_1_6_2_4_list	postamble_1_idle

struct test_stream test_1_6_2_4_conn = { &preamble_1_6_2_4_conn, &test_case_1_6_2_4_conn, &postamble_1_6_2_4_conn };
struct test_stream test_1_6_2_4_resp = { &preamble_1_6_2_4_resp, &test_case_1_6_2_4_resp, &postamble_1_6_2_4_resp };
struct test_stream test_1_6_2_4_list = { &preamble_1_6_2_4_list, &test_case_1_6_2_4_list, &postamble_1_6_2_4_list };

#define tgrp_case_1_6_2_5 test_group_1
#define sgrp_case_1_6_2_5 test_group_1_6_2
#define numb_case_1_6_2_5 "1.6.2.5"
#define name_case_1_6_2_5 "Wildcard N_BIND_REQ in NS_UNBND state with TOKEN_REQUEST."
#define sref_case_1_6_2_5 "NPI Rev 2.0.0"
#define desc_case_1_6_2_5 "\
Checks that the bind is successful when N_BIND_REQ is issued with a wildcard\n\
address and a specific port number in the NS_UNBND state.  Check that a N_CONS\n\
Stream results when the TOKEN_REQUEST flag is set and that a non-zero token is\n\
returned."

int
test_case_1_6_2_5(int child)
{
	int port = htons(10000);
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL + child };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = TOKEN_REQUEST;
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
	if (TOKEN_value == 0)
		goto failure;
	state++;
#if 0
	if (PROTOID_length != 1)
		goto failure;
	state++;
	if (PROTOID_buffer[0] != TEST_PROTOCOL + child)
		goto failure;
	state++;
#endif
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
	if (last_info.SERV_type != N_CONS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_2_5_conn	test_case_1_6_2_5
#define test_case_1_6_2_5_resp	test_case_1_6_2_5
#define test_case_1_6_2_5_list	test_case_1_6_2_5

#define preamble_1_6_2_5_conn	preamble_1_unbnd
#define preamble_1_6_2_5_resp	preamble_1_unbnd
#define preamble_1_6_2_5_list	preamble_1_unbnd

#define postamble_1_6_2_5_conn	postamble_1_idle
#define postamble_1_6_2_5_resp	postamble_1_idle
#define postamble_1_6_2_5_list	postamble_1_idle

struct test_stream test_1_6_2_5_conn = { &preamble_1_6_2_5_conn, &test_case_1_6_2_5_conn, &postamble_1_6_2_5_conn };
struct test_stream test_1_6_2_5_resp = { &preamble_1_6_2_5_resp, &test_case_1_6_2_5_resp, &postamble_1_6_2_5_resp };
struct test_stream test_1_6_2_5_list = { &preamble_1_6_2_5_list, &test_case_1_6_2_5_list, &postamble_1_6_2_5_list };

#define test_group_1_6_3 "1.6.3. Double bind from the NS_UNBND State."
#define tgrp_case_1_6_3_1 test_group_1
#define sgrp_case_1_6_3_1 test_group_1_6_3
#define numb_case_1_6_3_1 "1.6.3.1"
#define name_case_1_6_3_1 "NOUTSTATE in NS_IDLE state."
#define sref_case_1_6_3_1 "NPI Rev 2.0.0"
#define desc_case_1_6_3_1 "\
Checks that NOUTSTATE is returned under proper conditions when N_BIND_REQ is\n\
issued twice from the NS_UNBND state (double bind)."

int
test_case_1_6_3_1(int child)
{
	int port = htons(10000);
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { TEST_PROTOCOL + child };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = TOKEN_REQUEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = TOKEN_REQUEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_6_3_1_conn	test_case_1_6_3_1
#define test_case_1_6_3_1_resp	test_case_1_6_3_1
#define test_case_1_6_3_1_list	test_case_1_6_3_1

#define preamble_1_6_3_1_conn	preamble_1_unbnd
#define preamble_1_6_3_1_resp	preamble_1_unbnd
#define preamble_1_6_3_1_list	preamble_1_unbnd

#define postamble_1_6_3_1_conn	postamble_1_idle
#define postamble_1_6_3_1_resp	postamble_1_idle
#define postamble_1_6_3_1_list	postamble_1_idle

struct test_stream test_1_6_3_1_conn = { &preamble_1_6_3_1_conn, &test_case_1_6_3_1_conn, &postamble_1_6_3_1_conn };
struct test_stream test_1_6_3_1_resp = { &preamble_1_6_3_1_resp, &test_case_1_6_3_1_resp, &postamble_1_6_3_1_resp };
struct test_stream test_1_6_3_1_list = { &preamble_1_6_3_1_list, &test_case_1_6_3_1_list, &postamble_1_6_3_1_list };

#define test_group_2 "2. Connectionless service"
#define test_group_2_1 "2.1. Non-fatal errors in the NS_IDLE state"
#define tgrp_case_2_1_1 test_group_2
#define sgrp_case_2_1_1 test_group_2_1
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "NOUTSTATE for N_BIND_REQ in NS_IDLE state."
#define sref_case_2_1_1 "NPI Rev 2.0.0"
#define desc_case_2_1_1 "\
Checks that NOUTSTATE is returned when N_BIND_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_1(int child)
{
	int port = htons(10000 + child);
	int proto = TEST_PROTOCOL;
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { proto };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_1_conn	test_case_2_1_1
#define test_case_2_1_1_resp	test_case_2_1_1
#define test_case_2_1_1_list	test_case_2_1_1

#define preamble_2_1_1_conn	preamble_1_idle
#define preamble_2_1_1_resp	preamble_1_idle
#define preamble_2_1_1_list	preamble_1_idle

#define postamble_2_1_1_conn	postamble_1_idle
#define postamble_2_1_1_resp	postamble_1_idle
#define postamble_2_1_1_list	postamble_1_idle

struct test_stream test_2_1_1_conn = { &preamble_2_1_1_conn, &test_case_2_1_1_conn, &postamble_2_1_1_conn };
struct test_stream test_2_1_1_resp = { &preamble_2_1_1_resp, &test_case_2_1_1_resp, &postamble_2_1_1_resp };
struct test_stream test_2_1_1_list = { &preamble_2_1_1_list, &test_case_2_1_1_list, &postamble_2_1_1_list };

#define tgrp_case_2_1_2 test_group_2
#define sgrp_case_2_1_2 test_group_2_1
#define numb_case_2_1_2 "2.1.2"
#define name_case_2_1_2 "NNOTSUPPORT for N_CONN_REQ in NS_IDLE state."
#define sref_case_2_1_2 "NPI Rev 2.0.0"
#define desc_case_2_1_2 "\
Checks that NNOTSUPPORT is returned when N_CONN_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_2(int child)
{
	int port = htons(10000 + child + 3);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000000) } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOTSUPPORT)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_2_conn	test_case_2_1_2
#define test_case_2_1_2_resp	test_case_2_1_2
#define test_case_2_1_2_list	test_case_2_1_2

#define preamble_2_1_2_conn	preamble_1_idle_clns
#define preamble_2_1_2_resp	preamble_1_idle_clns
#define preamble_2_1_2_list	preamble_1_idle_clns

#define postamble_2_1_2_conn	postamble_1_idle
#define postamble_2_1_2_resp	postamble_1_idle
#define postamble_2_1_2_list	postamble_1_idle

struct test_stream test_2_1_2_conn = { &preamble_2_1_2_conn, &test_case_2_1_2_conn, &postamble_2_1_2_conn };
struct test_stream test_2_1_2_resp = { &preamble_2_1_2_resp, &test_case_2_1_2_resp, &postamble_2_1_2_resp };
struct test_stream test_2_1_2_list = { &preamble_2_1_2_list, &test_case_2_1_2_list, &postamble_2_1_2_list };

#define tgrp_case_2_1_3 test_group_2
#define sgrp_case_2_1_3 test_group_2_1
#define numb_case_2_1_3 "2.1.3"
#define name_case_2_1_3 "NNOTSUPPORT for N_CONN_RES in NS_IDLE state."
#define sref_case_2_1_3 "NPI Rev 2.0.0"
#define desc_case_2_1_3 "\
Checks that NNOTSUPPORT is returned when N_CONN_RES is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_3(int child)
{
	int port = htons(10000 + child + 3);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };

	TOKEN_value = 0;
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	SEQ_number = 0;
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOTSUPPORT)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_3_conn	test_case_2_1_3
#define test_case_2_1_3_resp	test_case_2_1_3
#define test_case_2_1_3_list	test_case_2_1_3

#define preamble_2_1_3_conn	preamble_1_idle_clns
#define preamble_2_1_3_resp	preamble_1_idle_clns
#define preamble_2_1_3_list	preamble_1_idle_clns

#define postamble_2_1_3_conn	postamble_1_idle
#define postamble_2_1_3_resp	postamble_1_idle
#define postamble_2_1_3_list	postamble_1_idle

struct test_stream test_2_1_3_conn = { &preamble_2_1_3_conn, &test_case_2_1_3_conn, &postamble_2_1_3_conn };
struct test_stream test_2_1_3_resp = { &preamble_2_1_3_resp, &test_case_2_1_3_resp, &postamble_2_1_3_resp };
struct test_stream test_2_1_3_list = { &preamble_2_1_3_list, &test_case_2_1_3_list, &postamble_2_1_3_list };

#define tgrp_case_2_1_4 test_group_2
#define sgrp_case_2_1_4 test_group_2_1
#define numb_case_2_1_4 "2.1.4"
#define name_case_2_1_4 "NNOTSUPPORT for N_RESET_REQ in NS_IDLE state."
#define sref_case_2_1_4 "NPI Rev 2.0.0"
#define desc_case_2_1_4 "\
Checks that NNOTSUPPORT is returned when N_RESET_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_4(int child)
{
	RESET_reason = N_RESET_UNSPECIFIED;
	if (do_signal(child, __TEST_RESET_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOTSUPPORT)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_4_conn	test_case_2_1_4
#define test_case_2_1_4_resp	test_case_2_1_4
#define test_case_2_1_4_list	test_case_2_1_4

#define preamble_2_1_4_conn	preamble_1_idle_clns
#define preamble_2_1_4_resp	preamble_1_idle_clns
#define preamble_2_1_4_list	preamble_1_idle_clns

#define postamble_2_1_4_conn	postamble_1_idle
#define postamble_2_1_4_resp	postamble_1_idle
#define postamble_2_1_4_list	postamble_1_idle

struct test_stream test_2_1_4_conn = { &preamble_2_1_4_conn, &test_case_2_1_4_conn, &postamble_2_1_4_conn };
struct test_stream test_2_1_4_resp = { &preamble_2_1_4_resp, &test_case_2_1_4_resp, &postamble_2_1_4_resp };
struct test_stream test_2_1_4_list = { &preamble_2_1_4_list, &test_case_2_1_4_list, &postamble_2_1_4_list };

#define tgrp_case_2_1_5 test_group_2
#define sgrp_case_2_1_5 test_group_2_1
#define numb_case_2_1_5 "2.1.5"
#define name_case_2_1_5 "NNOTSUPPORT for N_RESET_RES in NS_IDLE state."
#define sref_case_2_1_5 "NPI Rev 2.0.0"
#define desc_case_2_1_5 "\
Checks that NNOTSUPPORT is returned when N_RESET_RES is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_5(int child)
{
	if (do_signal(child, __TEST_RESET_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOTSUPPORT)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_5_conn	test_case_2_1_5
#define test_case_2_1_5_resp	test_case_2_1_5
#define test_case_2_1_5_list	test_case_2_1_5

#define preamble_2_1_5_conn	preamble_1_idle_clns
#define preamble_2_1_5_resp	preamble_1_idle_clns
#define preamble_2_1_5_list	preamble_1_idle_clns

#define postamble_2_1_5_conn	postamble_1_idle
#define postamble_2_1_5_resp	postamble_1_idle
#define postamble_2_1_5_list	postamble_1_idle

struct test_stream test_2_1_5_conn = { &preamble_2_1_5_conn, &test_case_2_1_5_conn, &postamble_2_1_5_conn };
struct test_stream test_2_1_5_resp = { &preamble_2_1_5_resp, &test_case_2_1_5_resp, &postamble_2_1_5_resp };
struct test_stream test_2_1_5_list = { &preamble_2_1_5_list, &test_case_2_1_5_list, &postamble_2_1_5_list };

#define tgrp_case_2_1_6 test_group_2
#define sgrp_case_2_1_6 test_group_2_1
#define numb_case_2_1_6 "2.1.6"
#define name_case_2_1_6 "NNOTSUPPORT for N_DISCON_REQ in NS_IDLE state."
#define sref_case_2_1_6 "NPI Rev 2.0.0"
#define desc_case_2_1_6 "\
Checks that NNOTSUPPORT is returned when N_DISCON_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_1_6(int child)
{
	ADDR_buffer = NULL;
	ADDR_length = 0;
	DISCON_reason = N_UNDEFINED;
	SEQ_number = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NNOTSUPPORT)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_6_conn	test_case_2_1_6
#define test_case_2_1_6_resp	test_case_2_1_6
#define test_case_2_1_6_list	test_case_2_1_6

#define preamble_2_1_6_conn	preamble_1_idle_clns
#define preamble_2_1_6_resp	preamble_1_idle_clns
#define preamble_2_1_6_list	preamble_1_idle_clns

#define postamble_2_1_6_conn	postamble_1_idle
#define postamble_2_1_6_resp	postamble_1_idle
#define postamble_2_1_6_list	postamble_1_idle

struct test_stream test_2_1_6_conn = { &preamble_2_1_6_conn, &test_case_2_1_6_conn, &postamble_2_1_6_conn };
struct test_stream test_2_1_6_resp = { &preamble_2_1_6_resp, &test_case_2_1_6_resp, &postamble_2_1_6_resp };
struct test_stream test_2_1_6_list = { &preamble_2_1_6_list, &test_case_2_1_6_list, &postamble_2_1_6_list };

#define test_group_2_2 "2.2. Fatal errors in the NS_IDLE state"
#define tgrp_case_2_2_1 test_group_2
#define sgrp_case_2_2_1 test_group_2_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "EPROTO for N_DATACK_REQ in NS_IDLE state."
#define sref_case_2_2_1 "NPI Rev 2.0.0"
#define desc_case_2_2_1 "\
Checks that EPROTO is returned when N_DATACK_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_2_1(int child)
{
	QOS_length = 0;
	if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_2_1_conn	test_case_2_2_1
#define test_case_2_2_1_resp	test_case_2_2_1
#define test_case_2_2_1_list	test_case_2_2_1

#define preamble_2_2_1_conn	preamble_1_idle_clns
#define preamble_2_2_1_resp	preamble_1_idle_clns
#define preamble_2_2_1_list	preamble_1_idle_clns

#define postamble_2_2_1_conn	postamble_1_idle
#define postamble_2_2_1_resp	postamble_1_idle
#define postamble_2_2_1_list	postamble_1_idle

struct test_stream test_2_2_1_conn = { &preamble_2_2_1_conn, &test_case_2_2_1_conn, &postamble_2_2_1_conn };
struct test_stream test_2_2_1_resp = { &preamble_2_2_1_resp, &test_case_2_2_1_resp, &postamble_2_2_1_resp };
struct test_stream test_2_2_1_list = { &preamble_2_2_1_list, &test_case_2_2_1_list, &postamble_2_2_1_list };

#define tgrp_case_2_2_2 test_group_2
#define sgrp_case_2_2_2 test_group_2_2
#define numb_case_2_2_2 "2.2.2"
#define name_case_2_2_2 "EPROTO for N_DATA_REQ in NS_IDLE state."
#define sref_case_2_2_2 "NPI Rev 2.0.0"
#define desc_case_2_2_2 "\
Checks that EPROTO is returned when N_DATA_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_2_2(int child)
{
	DATA_xfer_flags = 0;
	DATA_buffer = NULL;
	DATA_length = 0;
	QOS_length = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_2_2_conn	test_case_2_2_2
#define test_case_2_2_2_resp	test_case_2_2_2
#define test_case_2_2_2_list	test_case_2_2_2

#define preamble_2_2_2_conn	preamble_1_idle_clns
#define preamble_2_2_2_resp	preamble_1_idle_clns
#define preamble_2_2_2_list	preamble_1_idle_clns

#define postamble_2_2_2_conn	postamble_1_idle
#define postamble_2_2_2_resp	postamble_1_idle
#define postamble_2_2_2_list	postamble_1_idle

struct test_stream test_2_2_2_conn = { &preamble_2_2_2_conn, &test_case_2_2_2_conn, &postamble_2_2_2_conn };
struct test_stream test_2_2_2_resp = { &preamble_2_2_2_resp, &test_case_2_2_2_resp, &postamble_2_2_2_resp };
struct test_stream test_2_2_2_list = { &preamble_2_2_2_list, &test_case_2_2_2_list, &postamble_2_2_2_list };

#define tgrp_case_2_2_3 test_group_2
#define sgrp_case_2_2_3 test_group_2_2
#define numb_case_2_2_3 "2.2.3"
#define name_case_2_2_3 "EPROTO for N_EXDATA_REQ in NS_IDLE state."
#define sref_case_2_2_3 "NPI Rev 2.0.0"
#define desc_case_2_2_3 "\
Checks that EPROTO is returned when N_EXDATA_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_2_2_3(int child)
{
	DATA_buffer = NULL;
	DATA_length = 0;
	QOS_length = 0;
	if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_2_3_conn	test_case_2_2_3
#define test_case_2_2_3_resp	test_case_2_2_3
#define test_case_2_2_3_list	test_case_2_2_3

#define preamble_2_2_3_conn	preamble_1_idle_clns
#define preamble_2_2_3_resp	preamble_1_idle_clns
#define preamble_2_2_3_list	preamble_1_idle_clns

#define postamble_2_2_3_conn	postamble_1_idle
#define postamble_2_2_3_resp	postamble_1_idle
#define postamble_2_2_3_list	postamble_1_idle

struct test_stream test_2_2_3_conn = { &preamble_2_2_3_conn, &test_case_2_2_3_conn, &postamble_2_2_3_conn };
struct test_stream test_2_2_3_resp = { &preamble_2_2_3_resp, &test_case_2_2_3_resp, &postamble_2_2_3_resp };
struct test_stream test_2_2_3_list = { &preamble_2_2_3_list, &test_case_2_2_3_list, &postamble_2_2_3_list };

#define test_group_2_3 "2.3. Fatal errors in response to N_UNITDATA_REQ in the NS_IDLE state"
#define tgrp_case_2_3_1 test_group_2
#define sgrp_case_2_3_1 test_group_2_3
#define numb_case_2_3_1 "2.3.1"
#define name_case_2_3_1 "EPROTO for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_3_1 "NPI Rev 2.0.0"
#define desc_case_2_3_1 "\
Checks that EPROTO is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  Primitive too short."

int
test_case_2_3_1(int child)
{
	PRIM_type = N_UNITDATA_REQ;
	if (do_signal(child, __TEST_PRIM_TOO_SHORT) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_3_1_conn	test_case_2_3_1
#define test_case_2_3_1_resp	test_case_2_3_1
#define test_case_2_3_1_list	test_case_2_3_1

#define preamble_2_3_1_conn	preamble_1_idle_clns
#define preamble_2_3_1_resp	preamble_1_idle_clns
#define preamble_2_3_1_list	preamble_1_idle_clns

#define postamble_2_3_1_conn	postamble_1_idle
#define postamble_2_3_1_resp	postamble_1_idle
#define postamble_2_3_1_list	postamble_1_idle

struct test_stream test_2_3_1_conn = { &preamble_2_3_1_conn, &test_case_2_3_1_conn, &postamble_2_3_1_conn };
struct test_stream test_2_3_1_resp = { &preamble_2_3_1_resp, &test_case_2_3_1_resp, &postamble_2_3_1_resp };
struct test_stream test_2_3_1_list = { &preamble_2_3_1_list, &test_case_2_3_1_list, &postamble_2_3_1_list };

#define tgrp_case_2_3_2 test_group_2
#define sgrp_case_2_3_2 test_group_2_3
#define numb_case_2_3_2 "2.3.2"
#define name_case_2_3_2 "EPROTO for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_3_2 "NPI Rev 2.0.0"
#define desc_case_2_3_2 "\
Checks that EPROTO is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  No address."

int
test_case_2_3_2(int child)
{
	ADDR_buffer = NULL;
	ADDR_length = 0;
	DATA_buffer = "Test";
	DATA_length = strlen(DATA_buffer);
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_3_2_conn	test_case_2_3_2
#define test_case_2_3_2_resp	test_case_2_3_2
#define test_case_2_3_2_list	test_case_2_3_2

#define preamble_2_3_2_conn	preamble_1_idle_clns
#define preamble_2_3_2_resp	preamble_1_idle_clns
#define preamble_2_3_2_list	preamble_1_idle_clns

#define postamble_2_3_2_conn	postamble_1_idle
#define postamble_2_3_2_resp	postamble_1_idle
#define postamble_2_3_2_list	postamble_1_idle

struct test_stream test_2_3_2_conn = { &preamble_2_3_2_conn, &test_case_2_3_2_conn, &postamble_2_3_2_conn };
struct test_stream test_2_3_2_resp = { &preamble_2_3_2_resp, &test_case_2_3_2_resp, &postamble_2_3_2_resp };
struct test_stream test_2_3_2_list = { &preamble_2_3_2_list, &test_case_2_3_2_list, &postamble_2_3_2_list };

#define test_group_2_4 "2.4. Non-fatal errors in response to N_UNITDATA_REQ in the NS_IDLE state"
#define tgrp_case_2_4_1 test_group_2
#define sgrp_case_2_4_1 test_group_2_4
#define numb_case_2_4_1 "2.4.1"
#define name_case_2_4_1 "NBADDATA for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_4_1 "NPI Rev 2.0.0"
#define desc_case_2_4_1 "\
Checks that NBADDATA is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  No data."

int
test_case_2_4_1(int child)
{
	int port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000000) } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = NULL;
	DATA_length = 0;
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UDERROR_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ERROR_type != NBADDATA)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_4_1_conn	test_case_2_4_1
#define test_case_2_4_1_resp	test_case_2_4_1
#define test_case_2_4_1_list	test_case_2_4_1

#define preamble_2_4_1_conn	preamble_1_idle_clns
#define preamble_2_4_1_resp	preamble_1_idle_clns
#define preamble_2_4_1_list	preamble_1_idle_clns

#define postamble_2_4_1_conn	postamble_1_idle
#define postamble_2_4_1_resp	postamble_1_idle
#define postamble_2_4_1_list	postamble_1_idle

struct test_stream test_2_4_1_conn = { &preamble_2_4_1_conn, &test_case_2_4_1_conn, &postamble_2_4_1_conn };
struct test_stream test_2_4_1_resp = { &preamble_2_4_1_resp, &test_case_2_4_1_resp, &postamble_2_4_1_resp };
struct test_stream test_2_4_1_list = { &preamble_2_4_1_list, &test_case_2_4_1_list, &postamble_2_4_1_list };

#define tgrp_case_2_4_2 test_group_2
#define sgrp_case_2_4_2 test_group_2_4
#define numb_case_2_4_2 "2.4.2"
#define name_case_2_4_2 "NBADDATA for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_4_2 "NPI Rev 2.0.0"
#define desc_case_2_4_2 "\
Checks that NBADDATA is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  Zero length data."

int
test_case_2_4_2(int child)
{
	int port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000000) } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = "";
	DATA_length = strlen(DATA_buffer);
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UDERROR_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ERROR_type != NBADDATA)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_4_2_conn	test_case_2_4_2
#define test_case_2_4_2_resp	test_case_2_4_2
#define test_case_2_4_2_list	test_case_2_4_2

#define preamble_2_4_2_conn	preamble_1_idle_clns
#define preamble_2_4_2_resp	preamble_1_idle_clns
#define preamble_2_4_2_list	preamble_1_idle_clns

#define postamble_2_4_2_conn	postamble_1_idle
#define postamble_2_4_2_resp	postamble_1_idle
#define postamble_2_4_2_list	postamble_1_idle

struct test_stream test_2_4_2_conn = { &preamble_2_4_2_conn, &test_case_2_4_2_conn, &postamble_2_4_2_conn };
struct test_stream test_2_4_2_resp = { &preamble_2_4_2_resp, &test_case_2_4_2_resp, &postamble_2_4_2_resp };
struct test_stream test_2_4_2_list = { &preamble_2_4_2_list, &test_case_2_4_2_list, &postamble_2_4_2_list };

#define tgrp_case_2_4_3 test_group_2
#define sgrp_case_2_4_3 test_group_2_4
#define numb_case_2_4_3 "2.4.3"
#define name_case_2_4_3 "NBADDATA for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_4_3 "NPI Rev 2.0.0"
#define desc_case_2_4_3 "\
Checks that NBADDATA is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  Data exceeding maximum."

int
test_case_2_4_3(int child)
{
	int port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000000) } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = NULL;
	DATA_length = 0;
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UDERROR_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ERROR_type != NBADDATA)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_4_3_conn	test_case_2_4_3
#define test_case_2_4_3_resp	test_case_2_4_3
#define test_case_2_4_3_list	test_case_2_4_3

#define preamble_2_4_3_conn	preamble_1_idle_clns
#define preamble_2_4_3_resp	preamble_1_idle_clns
#define preamble_2_4_3_list	preamble_1_idle_clns

#define postamble_2_4_3_conn	postamble_1_idle
#define postamble_2_4_3_resp	postamble_1_idle
#define postamble_2_4_3_list	postamble_1_idle

struct test_stream test_2_4_3_conn = { &preamble_2_4_3_conn, &test_case_2_4_3_conn, &postamble_2_4_3_conn };
struct test_stream test_2_4_3_resp = { &preamble_2_4_3_resp, &test_case_2_4_3_resp, &postamble_2_4_3_resp };
struct test_stream test_2_4_3_list = { &preamble_2_4_3_list, &test_case_2_4_3_list, &postamble_2_4_3_list };

#define tgrp_case_2_4_4 test_group_2
#define sgrp_case_2_4_4 test_group_2_4
#define numb_case_2_4_4 "2.4.4"
#define name_case_2_4_4 "NBADADDR for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_4_4 "NPI Rev 2.0.0"
#define desc_case_2_4_4 "\
Checks that NBADADDR is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  Wildcard address."

int
test_case_2_4_4(int child)
{
	int port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = "Test";
	DATA_length = strlen(DATA_buffer);
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UDERROR_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ERROR_type != NBADADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_4_4_conn	test_case_2_4_4
#define test_case_2_4_4_resp	test_case_2_4_4
#define test_case_2_4_4_list	test_case_2_4_4

#define preamble_2_4_4_conn	preamble_1_idle_clns
#define preamble_2_4_4_resp	preamble_1_idle_clns
#define preamble_2_4_4_list	preamble_1_idle_clns

#define postamble_2_4_4_conn	postamble_1_idle
#define postamble_2_4_4_resp	postamble_1_idle
#define postamble_2_4_4_list	postamble_1_idle

struct test_stream test_2_4_4_conn = { &preamble_2_4_4_conn, &test_case_2_4_4_conn, &postamble_2_4_4_conn };
struct test_stream test_2_4_4_resp = { &preamble_2_4_4_resp, &test_case_2_4_4_resp, &postamble_2_4_4_resp };
struct test_stream test_2_4_4_list = { &preamble_2_4_4_list, &test_case_2_4_4_list, &postamble_2_4_4_list };

#define tgrp_case_2_4_5 test_group_2
#define sgrp_case_2_4_5 test_group_2_4
#define numb_case_2_4_5 "2.4.5"
#define name_case_2_4_5 "NBADADDR for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_4_5 "NPI Rev 2.0.0"
#define desc_case_2_4_5 "\
Checks that NBADADDR is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state, under error conditions.  Wrong family."

int
test_case_2_4_5(int child)
{
	int port = htons(10000 + child);
	struct sockaddr_in sin = { AF_X25, port, { htonl(0x7f000000) } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = "Test";
	DATA_length = strlen(DATA_buffer);
	
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UDERROR_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ERROR_type != NBADADDR)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_4_5_conn	test_case_2_4_5
#define test_case_2_4_5_resp	test_case_2_4_5
#define test_case_2_4_5_list	test_case_2_4_5

#define preamble_2_4_5_conn	preamble_1_idle_clns
#define preamble_2_4_5_resp	preamble_1_idle_clns
#define preamble_2_4_5_list	preamble_1_idle_clns

#define postamble_2_4_5_conn	postamble_1_idle
#define postamble_2_4_5_resp	postamble_1_idle
#define postamble_2_4_5_list	postamble_1_idle

struct test_stream test_2_4_5_conn = { &preamble_2_4_5_conn, &test_case_2_4_5_conn, &postamble_2_4_5_conn };
struct test_stream test_2_4_5_resp = { &preamble_2_4_5_resp, &test_case_2_4_5_resp, &postamble_2_4_5_resp };
struct test_stream test_2_4_5_list = { &preamble_2_4_5_list, &test_case_2_4_5_list, &postamble_2_4_5_list };

#define test_group_2_5 "2.5. Successful N_CLNS data transfer."
#define tgrp_case_2_5_1 test_group_2
#define sgrp_case_2_5_1 test_group_2_5
#define numb_case_2_5_1 "2.5.1"
#define name_case_2_5_1 "Successful N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_5_1 "NPI Rev 2.0.0"
#define desc_case_2_5_1 "\
Checks that data can be transferred using N_UNITDATA_REQ."

int
test_case_2_5_1(int child)
{
	unsigned short port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };
	char buf[] = "xxxxTest Data";

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = buf;
	DATA_length = 14;

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&port, &buf[0], sizeof(port));
	bcopy(&port, &buf[2], sizeof(port));

	test_msleep(child, LONG_WAIT);
	state++;
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UNITDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_5_1_conn	test_case_2_5_1
#define test_case_2_5_1_resp	test_case_2_5_1
#define test_case_2_5_1_list	test_case_2_5_1

#define preamble_2_5_1_conn	preamble_1_idle_clns
#define preamble_2_5_1_resp	preamble_1_idle_clns
#define preamble_2_5_1_list	preamble_1_idle_clns

#define postamble_2_5_1_conn	postamble_1_idle
#define postamble_2_5_1_resp	postamble_1_idle
#define postamble_2_5_1_list	postamble_1_idle

struct test_stream test_2_5_1_conn = { &preamble_2_5_1_conn, &test_case_2_5_1_conn, &postamble_2_5_1_conn };
struct test_stream test_2_5_1_resp = { &preamble_2_5_1_resp, &test_case_2_5_1_resp, &postamble_2_5_1_resp };
struct test_stream test_2_5_1_list = { &preamble_2_5_1_list, &test_case_2_5_1_list, &postamble_2_5_1_list };

#define tgrp_case_2_5_2 test_group_2
#define sgrp_case_2_5_2 test_group_2_5
#define numb_case_2_5_2 "2.5.2"
#define name_case_2_5_2 "Successful N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_2_5_2 "NPI Rev 2.0.0"
#define desc_case_2_5_2 "\
Checks that data can be transferred using N_UNITDATA_REQ."

int
test_case_2_5_2(int child)
{
	unsigned short port = htons(10000 + ((child + 1) % 3));
	struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };
	char buf[] = "xxxxTest Data";

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = buf;
	DATA_length = 14;

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&port, &buf[0], sizeof(port));
	bcopy(&port, &buf[2], sizeof(port));

	test_msleep(child, LONG_WAIT);
	state++;
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_UNITDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_5_2_conn	test_case_2_5_2
#define test_case_2_5_2_resp	test_case_2_5_2
#define test_case_2_5_2_list	test_case_2_5_2

#define preamble_2_5_2_conn	preamble_1_idle_clns
#define preamble_2_5_2_resp	preamble_1_idle_clns
#define preamble_2_5_2_list	preamble_1_idle_clns

#define postamble_2_5_2_conn	postamble_1_idle
#define postamble_2_5_2_resp	postamble_1_idle
#define postamble_2_5_2_list	postamble_1_idle

struct test_stream test_2_5_2_conn = { &preamble_2_5_2_conn, &test_case_2_5_2_conn, &postamble_2_5_2_conn };
struct test_stream test_2_5_2_resp = { &preamble_2_5_2_resp, &test_case_2_5_2_resp, &postamble_2_5_2_resp };
struct test_stream test_2_5_2_list = { &preamble_2_5_2_list, &test_case_2_5_2_list, &postamble_2_5_2_list };

#define test_group_3 "3. Connection-oriented service"
#define test_group_3_1 "3.1. Non-fatal errors in the NS_IDLE state"
#define tgrp_case_3_1_1 test_group_3
#define sgrp_case_3_1_1 test_group_3_1
#define numb_case_3_1_1 "3.1.1"
#define name_case_3_1_1 "NOUTSTATE for N_BIND_REQ in NS_IDLE state."
#define sref_case_3_1_1 "NPI Rev 2.0.0"
#define desc_case_3_1_1 "\
Checks that NOUTSTATE is returned when N_BIND_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_3_1_1(int child)
{
	unsigned short port = htons(10000 + child);
	unsigned char proto = TEST_PROTOCOL;
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { proto };
	int coninds = (child == 2) ? 1 : 0;

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = coninds;
	BIND_flags = TOKEN_REQUEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_1_1_conn	test_case_3_1_1
#define test_case_3_1_1_resp	test_case_3_1_1
#define test_case_3_1_1_list	test_case_3_1_1

#define preamble_3_1_1_conn	preamble_1_idle_cons
#define preamble_3_1_1_resp	preamble_1_idle_cons
#define preamble_3_1_1_list	preamble_1_idle_cons

#define postamble_3_1_1_conn	postamble_1_idle
#define postamble_3_1_1_resp	postamble_1_idle
#define postamble_3_1_1_list	postamble_1_idle

struct test_stream test_3_1_1_conn = { &preamble_3_1_1_conn, &test_case_3_1_1_conn, &postamble_3_1_1_conn };
struct test_stream test_3_1_1_resp = { &preamble_3_1_1_resp, &test_case_3_1_1_resp, &postamble_3_1_1_resp };
struct test_stream test_3_1_1_list = { &preamble_3_1_1_list, &test_case_3_1_1_list, &postamble_3_1_1_list };

#define tgrp_case_3_1_2 test_group_3
#define sgrp_case_3_1_2 test_group_3_1
#define numb_case_3_1_2 "3.1.2"
#define name_case_3_1_2 "NOUTSTATE for N_CONN_RES in NS_IDLE state."
#define sref_case_3_1_2 "NPI Rev 2.0.0"
#define desc_case_3_1_2 "\
Checks that NOUTSTATE is returned when N_CONN_RES is attempted in the\n\
NS_IDLE state."

int
test_case_3_1_2(int child)
{
	unsigned short port = htons(10000 + child + 3);
	struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000000) } };

	TOKEN_value = 0;
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	SEQ_number = 0;
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_1_2_conn	test_case_3_1_2
#define test_case_3_1_2_resp	test_case_3_1_2
#define test_case_3_1_2_list	test_case_3_1_2

#define preamble_3_1_2_conn	preamble_1_idle_cons
#define preamble_3_1_2_resp	preamble_1_idle_cons
#define preamble_3_1_2_list	preamble_1_idle_cons

#define postamble_3_1_2_conn	postamble_1_idle
#define postamble_3_1_2_resp	postamble_1_idle
#define postamble_3_1_2_list	postamble_1_idle

struct test_stream test_3_1_2_conn = { &preamble_3_1_2_conn, &test_case_3_1_2_conn, &postamble_3_1_2_conn };
struct test_stream test_3_1_2_resp = { &preamble_3_1_2_resp, &test_case_3_1_2_resp, &postamble_3_1_2_resp };
struct test_stream test_3_1_2_list = { &preamble_3_1_2_list, &test_case_3_1_2_list, &postamble_3_1_2_list };

#define tgrp_case_3_1_3 test_group_3
#define sgrp_case_3_1_3 test_group_3_1
#define numb_case_3_1_3 "3.1.3"
#define name_case_3_1_3 "NOUTSTATE for N_RESET_RES in NS_IDLE state."
#define sref_case_3_1_3 "NPI Rev 2.0.0"
#define desc_case_3_1_3 "\
Checks that NOUTSTATE is returned when N_RESET_RES is attempted in the\n\
NS_IDLE state."

int
test_case_3_1_3(int child)
{
	if (do_signal(child, __TEST_RESET_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS) {
		if (last_event != __EVENT_NO_MSG)
			goto failure;
	} else {
		state++;
		if (NPI_error != NOUTSTATE)
			goto failure;
	}
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_1_3_conn	test_case_3_1_3
#define test_case_3_1_3_resp	test_case_3_1_3
#define test_case_3_1_3_list	test_case_3_1_3

#define preamble_3_1_3_conn	preamble_1_idle_cons
#define preamble_3_1_3_resp	preamble_1_idle_cons
#define preamble_3_1_3_list	preamble_1_idle_cons

#define postamble_3_1_3_conn	postamble_1_idle
#define postamble_3_1_3_resp	postamble_1_idle
#define postamble_3_1_3_list	postamble_1_idle

struct test_stream test_3_1_3_conn = { &preamble_3_1_3_conn, &test_case_3_1_3_conn, &postamble_3_1_3_conn };
struct test_stream test_3_1_3_resp = { &preamble_3_1_3_resp, &test_case_3_1_3_resp, &postamble_3_1_3_resp };
struct test_stream test_3_1_3_list = { &preamble_3_1_3_list, &test_case_3_1_3_list, &postamble_3_1_3_list };

#define tgrp_case_3_1_4 test_group_3
#define sgrp_case_3_1_4 test_group_3_1
#define numb_case_3_1_4 "3.1.4"
#define name_case_3_1_4 "NOUTSTATE for N_DISCON_REQ in NS_IDLE state."
#define sref_case_3_1_4 "NPI Rev 2.0.0"
#define desc_case_3_1_4 "\
Checks that NOUTSTATE is returned when N_DISCON_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_3_1_4(int child)
{
	ADDR_buffer = NULL;
	ADDR_length = 0;
	DISCON_reason = N_UNDEFINED;
	SEQ_number = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NOUTSTATE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_1_4_conn	test_case_3_1_4
#define test_case_3_1_4_resp	test_case_3_1_4
#define test_case_3_1_4_list	test_case_3_1_4

#define preamble_3_1_4_conn	preamble_1_idle_cons
#define preamble_3_1_4_resp	preamble_1_idle_cons
#define preamble_3_1_4_list	preamble_1_idle_cons

#define postamble_3_1_4_conn	postamble_1_idle
#define postamble_3_1_4_resp	postamble_1_idle
#define postamble_3_1_4_list	postamble_1_idle

struct test_stream test_3_1_4_conn = { &preamble_3_1_4_conn, &test_case_3_1_4_conn, &postamble_3_1_4_conn };
struct test_stream test_3_1_4_resp = { &preamble_3_1_4_resp, &test_case_3_1_4_resp, &postamble_3_1_4_resp };
struct test_stream test_3_1_4_list = { &preamble_3_1_4_list, &test_case_3_1_4_list, &postamble_3_1_4_list };

#define test_group_3_2 "3.2. Fatal errors in the NS_IDLE state"
#define tgrp_case_3_2_1 test_group_3
#define sgrp_case_3_2_1 test_group_3_2
#define numb_case_3_2_1 "3.2.1"
#define name_case_3_2_1 "EPROTO for N_UNITDATA_REQ in NS_IDLE state."
#define sref_case_3_2_1 "NPI Rev 2.0.0"
#define desc_case_3_2_1 "\
Checks that EPROTO is returned when N_UNITDATA_REQ is attempted in the\n\
NS_IDLE state."

int
test_case_3_2_1(int child)
{
	unsigned short port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };
	char buf[] = "xxxxTest Data";

	return (__RESULT_SKIPPED);
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	DATA_buffer = buf;
	DATA_length = 14;

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&port, &buf[0], sizeof(port));
	bcopy(&port, &buf[2], sizeof(port));

	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (UNIX_error != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_2_1_conn	test_case_3_2_1
#define test_case_3_2_1_resp	test_case_3_2_1
#define test_case_3_2_1_list	test_case_3_2_1

#define preamble_3_2_1_conn	preamble_1_idle_cons
#define preamble_3_2_1_resp	preamble_1_idle_cons
#define preamble_3_2_1_list	preamble_1_idle_cons

#define postamble_3_2_1_conn	postamble_1_idle
#define postamble_3_2_1_resp	postamble_1_idle
#define postamble_3_2_1_list	postamble_1_idle

struct test_stream test_3_2_1_conn = { &preamble_3_2_1_conn, &test_case_3_2_1_conn, &postamble_3_2_1_conn };
struct test_stream test_3_2_1_resp = { &preamble_3_2_1_resp, &test_case_3_2_1_resp, &postamble_3_2_1_resp };
struct test_stream test_3_2_1_list = { &preamble_3_2_1_list, &test_case_3_2_1_list, &postamble_3_2_1_list };

#define test_group_3_3 "3.3. Ignored errors in the NS_IDLE state"
#define tgrp_case_3_3_1 test_group_3
#define sgrp_case_3_3_1 test_group_3_3
#define numb_case_3_3_1 "3.3.1"
#define name_case_3_3_1 "Ignored N_DATACK_REQ in NS_IDLE state."
#define sref_case_3_3_1 "NPI Rev 2.0.0"
#define desc_case_3_3_1 "\
Checks N_DATACK_REQ issued in the NS_IDLE state is ignored."

int
test_case_3_3_1(int child)
{
	QOS_length = 0;
	if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_3_1_conn	test_case_3_3_1
#define test_case_3_3_1_resp	test_case_3_3_1
#define test_case_3_3_1_list	test_case_3_3_1

#define preamble_3_3_1_conn	preamble_1_idle_cons
#define preamble_3_3_1_resp	preamble_1_idle_cons
#define preamble_3_3_1_list	preamble_1_idle_cons

#define postamble_3_3_1_conn	postamble_1_idle
#define postamble_3_3_1_resp	postamble_1_idle
#define postamble_3_3_1_list	postamble_1_idle

struct test_stream test_3_3_1_conn = { &preamble_3_3_1_conn, &test_case_3_3_1_conn, &postamble_3_3_1_conn };
struct test_stream test_3_3_1_resp = { &preamble_3_3_1_resp, &test_case_3_3_1_resp, &postamble_3_3_1_resp };
struct test_stream test_3_3_1_list = { &preamble_3_3_1_list, &test_case_3_3_1_list, &postamble_3_3_1_list };

#define tgrp_case_3_3_2 test_group_3
#define sgrp_case_3_3_2 test_group_3_3
#define numb_case_3_3_2 "3.3.2"
#define name_case_3_3_2 "Ignored N_DATA_REQ in NS_IDLE state."
#define sref_case_3_3_2 "NPI Rev 2.0.0"
#define desc_case_3_3_2 "\
Checks N_DATA_REQ issued in the NS_IDLE state is ignored."

int
test_case_3_3_2(int child)
{
	DATA_xfer_flags = 0;
	DATA_buffer = "Test data";
	DATA_length = strlen(DATA_buffer);
	QOS_length = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_3_2_conn	test_case_3_3_2
#define test_case_3_3_2_resp	test_case_3_3_2
#define test_case_3_3_2_list	test_case_3_3_2

#define preamble_3_3_2_conn	preamble_1_idle_cons
#define preamble_3_3_2_resp	preamble_1_idle_cons
#define preamble_3_3_2_list	preamble_1_idle_cons

#define postamble_3_3_2_conn	postamble_1_idle
#define postamble_3_3_2_resp	postamble_1_idle
#define postamble_3_3_2_list	postamble_1_idle

struct test_stream test_3_3_2_conn = { &preamble_3_3_2_conn, &test_case_3_3_2_conn, &postamble_3_3_2_conn };
struct test_stream test_3_3_2_resp = { &preamble_3_3_2_resp, &test_case_3_3_2_resp, &postamble_3_3_2_resp };
struct test_stream test_3_3_2_list = { &preamble_3_3_2_list, &test_case_3_3_2_list, &postamble_3_3_2_list };

#define tgrp_case_3_3_3 test_group_3
#define sgrp_case_3_3_3 test_group_3_3
#define numb_case_3_3_3 "3.3.3"
#define name_case_3_3_3 "Ignored N_EXDATA_REQ in NS_IDLE state."
#define sref_case_3_3_3 "NPI Rev 2.0.0"
#define desc_case_3_3_3 "\
Checks N_EXDATA_REQ issued in the NS_IDLE state is ignored."

int
test_case_3_3_3(int child)
{
	DATA_buffer = "Test data";
	DATA_length = strlen(DATA_buffer);
	QOS_length = 0;
	if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_3_3_conn	test_case_3_3_3
#define test_case_3_3_3_resp	test_case_3_3_3
#define test_case_3_3_3_list	test_case_3_3_3

#define preamble_3_3_3_conn	preamble_1_idle_cons
#define preamble_3_3_3_resp	preamble_1_idle_cons
#define preamble_3_3_3_list	preamble_1_idle_cons

#define postamble_3_3_3_conn	postamble_1_idle
#define postamble_3_3_3_resp	postamble_1_idle
#define postamble_3_3_3_list	postamble_1_idle

struct test_stream test_3_3_3_conn = { &preamble_3_3_3_conn, &test_case_3_3_3_conn, &postamble_3_3_3_conn };
struct test_stream test_3_3_3_resp = { &preamble_3_3_3_resp, &test_case_3_3_3_resp, &postamble_3_3_3_resp };
struct test_stream test_3_3_3_list = { &preamble_3_3_3_list, &test_case_3_3_3_list, &postamble_3_3_3_list };

#define tgrp_case_3_3_4 test_group_3
#define sgrp_case_3_3_4 test_group_3_3
#define numb_case_3_3_4 "3.3.4"
#define name_case_3_3_4 "Ignored N_RESET_REQ in NS_IDLE state."
#define sref_case_3_3_4 "NPI Rev 2.0.0"
#define desc_case_3_3_4 "\
Checks that an N_RESET_REQ is ignored when issued in the NS_IDLE state."

int
test_case_3_3_4(int child)
{
	RESET_reason = N_RESET_UNSPECIFIED;
	if (do_signal(child, __TEST_RESET_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_3_4_conn	test_case_3_3_4
#define test_case_3_3_4_resp	test_case_3_3_4
#define test_case_3_3_4_list	test_case_3_3_4

#define preamble_3_3_4_conn	preamble_1_idle_cons
#define preamble_3_3_4_resp	preamble_1_idle_cons
#define preamble_3_3_4_list	preamble_1_idle_cons

#define postamble_3_3_4_conn	postamble_1_idle
#define postamble_3_3_4_resp	postamble_1_idle
#define postamble_3_3_4_list	postamble_1_idle

struct test_stream test_3_3_4_conn = { &preamble_3_3_4_conn, &test_case_3_3_4_conn, &postamble_3_3_4_conn };
struct test_stream test_3_3_4_resp = { &preamble_3_3_4_resp, &test_case_3_3_4_resp, &postamble_3_3_4_resp };
struct test_stream test_3_3_4_list = { &preamble_3_3_4_list, &test_case_3_3_4_list, &postamble_3_3_4_list };

#define test_group_3_4 "3.4. Failed N_CONN_REQ in the NS_IDLE state"
#define tgrp_case_3_4_1 test_group_3
#define sgrp_case_3_4_1 test_group_3_4
#define numb_case_3_4_1 "3.4.1"
#define name_case_3_4_1 "EINVAL for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_1 "NPI Rev 2.0.0"
#define desc_case_3_4_1 "\
Checks that EINVAL is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Primitive too short."

int
test_case_3_4_1(int child)
{
	PRIM_type = N_CONN_REQ;
	if (do_signal(child, __TEST_PRIM_TOO_SHORT) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (NPI_error != NSYSERR)
		goto failure;
	state++;
	if (UNIX_error != EINVAL)
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_1_conn	test_case_3_4_1
#define test_case_3_4_1_resp	test_case_3_4_1
#define test_case_3_4_1_list	test_case_3_4_1

#define preamble_3_4_1_conn	preamble_1_idle_cons
#define preamble_3_4_1_resp	preamble_1_idle_cons
#define preamble_3_4_1_list	preamble_1_idle_cons

#define postamble_3_4_1_conn	postamble_1_idle
#define postamble_3_4_1_resp	postamble_1_idle
#define postamble_3_4_1_list	postamble_1_idle

struct test_stream test_3_4_1_conn = { &preamble_3_4_1_conn, &test_case_3_4_1_conn, &postamble_3_4_1_conn };
struct test_stream test_3_4_1_resp = { &preamble_3_4_1_resp, &test_case_3_4_1_resp, &postamble_3_4_1_resp };
struct test_stream test_3_4_1_list = { &preamble_3_4_1_list, &test_case_3_4_1_list, &postamble_3_4_1_list };

#define tgrp_case_3_4_2 test_group_3
#define sgrp_case_3_4_2 test_group_3_4
#define numb_case_3_4_2 "3.4.2"
#define name_case_3_4_2 "NACCESS for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_2 "NPI Rev 2.0.0"
#define desc_case_3_4_2 "\
Checks that NACCESS is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Listening stream."

int
test_case_3_4_2(int child)
{
	unsigned short port = htons(10000 + child);
	struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = NULL;
	DATA_length = 0;

	if (child == 2) {
		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NACCESS)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_2_conn	test_case_3_4_2
#define test_case_3_4_2_resp	test_case_3_4_2
#define test_case_3_4_2_list	test_case_3_4_2

#define preamble_3_4_2_conn	preamble_1_idle_cons
#define preamble_3_4_2_resp	preamble_1_idle_cons
#define preamble_3_4_2_list	preamble_1_idle_cons

#define postamble_3_4_2_conn	postamble_1_idle
#define postamble_3_4_2_resp	postamble_1_idle
#define postamble_3_4_2_list	postamble_1_idle

struct test_stream test_3_4_2_conn = { &preamble_3_4_2_conn, &test_case_3_4_2_conn, &postamble_3_4_2_conn };
struct test_stream test_3_4_2_resp = { &preamble_3_4_2_resp, &test_case_3_4_2_resp, &postamble_3_4_2_resp };
struct test_stream test_3_4_2_list = { &preamble_3_4_2_list, &test_case_3_4_2_list, &postamble_3_4_2_list };

#define tgrp_case_3_4_3 test_group_3
#define sgrp_case_3_4_3 test_group_3_4
#define numb_case_3_4_3 "3.4.3"
#define name_case_3_4_3 "NBADFLAG for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_3 "NPI Rev 2.0.0"
#define desc_case_3_4_3 "\
Checks that NBADFLAG is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Invalid flags."

int
test_case_3_4_3(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0x7ff;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADFLAG)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_3_conn	test_case_3_4_3
#define test_case_3_4_3_resp	test_case_3_4_3
#define test_case_3_4_3_list	test_case_3_4_3

#define preamble_3_4_3_conn	preamble_1_idle_cons
#define preamble_3_4_3_resp	preamble_1_idle_cons
#define preamble_3_4_3_list	preamble_1_idle_cons

#define postamble_3_4_3_conn	postamble_1_idle
#define postamble_3_4_3_resp	postamble_1_idle
#define postamble_3_4_3_list	postamble_1_idle

struct test_stream test_3_4_3_conn = { &preamble_3_4_3_conn, &test_case_3_4_3_conn, &postamble_3_4_3_conn };
struct test_stream test_3_4_3_resp = { &preamble_3_4_3_resp, &test_case_3_4_3_resp, &postamble_3_4_3_resp };
struct test_stream test_3_4_3_list = { &preamble_3_4_3_list, &test_case_3_4_3_list, &postamble_3_4_3_list };

#define tgrp_case_3_4_4 test_group_3
#define sgrp_case_3_4_4 test_group_3_4
#define numb_case_3_4_4 "3.4.4"
#define name_case_3_4_4 "NNOADDR for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_4 "NPI Rev 2.0.0"
#define desc_case_3_4_4 "\
Checks that NNOADDR is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  No destination address."

int
test_case_3_4_4(int child)
{
	if (child != 2) {
		ADDR_buffer = NULL;
		ADDR_length = 0;
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NNOADDR)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_4_conn	test_case_3_4_4
#define test_case_3_4_4_resp	test_case_3_4_4
#define test_case_3_4_4_list	test_case_3_4_4

#define preamble_3_4_4_conn	preamble_1_idle_cons
#define preamble_3_4_4_resp	preamble_1_idle_cons
#define preamble_3_4_4_list	preamble_1_idle_cons

#define postamble_3_4_4_conn	postamble_1_idle
#define postamble_3_4_4_resp	postamble_1_idle
#define postamble_3_4_4_list	postamble_1_idle

struct test_stream test_3_4_4_conn = { &preamble_3_4_4_conn, &test_case_3_4_4_conn, &postamble_3_4_4_conn };
struct test_stream test_3_4_4_resp = { &preamble_3_4_4_resp, &test_case_3_4_4_resp, &postamble_3_4_4_resp };
struct test_stream test_3_4_4_list = { &preamble_3_4_4_list, &test_case_3_4_4_list, &postamble_3_4_4_list };

#define tgrp_case_3_4_5 test_group_3
#define sgrp_case_3_4_5 test_group_3_4
#define numb_case_3_4_5 "3.4.5"
#define name_case_3_4_5 "NBADADDR for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_5 "NPI Rev 2.0.0"
#define desc_case_3_4_5 "\
Checks that NBADADDR is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Short address."

int
test_case_3_4_5(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin) - 1;
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADADDR)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_5_conn	test_case_3_4_5
#define test_case_3_4_5_resp	test_case_3_4_5
#define test_case_3_4_5_list	test_case_3_4_5

#define preamble_3_4_5_conn	preamble_1_idle_cons
#define preamble_3_4_5_resp	preamble_1_idle_cons
#define preamble_3_4_5_list	preamble_1_idle_cons

#define postamble_3_4_5_conn	postamble_1_idle
#define postamble_3_4_5_resp	postamble_1_idle
#define postamble_3_4_5_list	postamble_1_idle

struct test_stream test_3_4_5_conn = { &preamble_3_4_5_conn, &test_case_3_4_5_conn, &postamble_3_4_5_conn };
struct test_stream test_3_4_5_resp = { &preamble_3_4_5_resp, &test_case_3_4_5_resp, &postamble_3_4_5_resp };
struct test_stream test_3_4_5_list = { &preamble_3_4_5_list, &test_case_3_4_5_list, &postamble_3_4_5_list };

#define tgrp_case_3_4_6 test_group_3
#define sgrp_case_3_4_6 test_group_3_4
#define numb_case_3_4_6 "3.4.6"
#define name_case_3_4_6 "NBADADDR for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_6 "NPI Rev 2.0.0"
#define desc_case_3_4_6 "\
Checks that NBADADDR is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Bad address family."

int
test_case_3_4_6(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_X25, port, { htonl(0x7f000001) } };

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADADDR)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_6_conn	test_case_3_4_6
#define test_case_3_4_6_resp	test_case_3_4_6
#define test_case_3_4_6_list	test_case_3_4_6

#define preamble_3_4_6_conn	preamble_1_idle_cons
#define preamble_3_4_6_resp	preamble_1_idle_cons
#define preamble_3_4_6_list	preamble_1_idle_cons

#define postamble_3_4_6_conn	postamble_1_idle
#define postamble_3_4_6_resp	postamble_1_idle
#define postamble_3_4_6_list	postamble_1_idle

struct test_stream test_3_4_6_conn = { &preamble_3_4_6_conn, &test_case_3_4_6_conn, &postamble_3_4_6_conn };
struct test_stream test_3_4_6_resp = { &preamble_3_4_6_resp, &test_case_3_4_6_resp, &postamble_3_4_6_resp };
struct test_stream test_3_4_6_list = { &preamble_3_4_6_list, &test_case_3_4_6_list, &postamble_3_4_6_list };

#define tgrp_case_3_4_7 test_group_3
#define sgrp_case_3_4_7 test_group_3_4
#define numb_case_3_4_7 "3.4.7"
#define name_case_3_4_7 "NBADADDR for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_7 "NPI Rev 2.0.0"
#define desc_case_3_4_7 "\
Checks that NBADADDR is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Wildcard address."

int
test_case_3_4_7(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADADDR)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_7_conn	test_case_3_4_7
#define test_case_3_4_7_resp	test_case_3_4_7
#define test_case_3_4_7_list	test_case_3_4_7

#define preamble_3_4_7_conn	preamble_1_idle_cons
#define preamble_3_4_7_resp	preamble_1_idle_cons
#define preamble_3_4_7_list	preamble_1_idle_cons

#define postamble_3_4_7_conn	postamble_1_idle
#define postamble_3_4_7_resp	postamble_1_idle
#define postamble_3_4_7_list	postamble_1_idle

struct test_stream test_3_4_7_conn = { &preamble_3_4_7_conn, &test_case_3_4_7_conn, &postamble_3_4_7_conn };
struct test_stream test_3_4_7_resp = { &preamble_3_4_7_resp, &test_case_3_4_7_resp, &postamble_3_4_7_resp };
struct test_stream test_3_4_7_list = { &preamble_3_4_7_list, &test_case_3_4_7_list, &postamble_3_4_7_list };

#define tgrp_case_3_4_8 test_group_3
#define sgrp_case_3_4_8 test_group_3_4
#define numb_case_3_4_8 "3.4.8"
#define name_case_3_4_8 "NBADQOSTYPE for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_8 "NPI Rev 2.0.0"
#define desc_case_3_4_8 "\
Checks that NBADQOSTYPE is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Invalid QOS type."

int
test_case_3_4_8(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };
#if 0
		np_ulong type = N_QOS_SEL_UD_IP;
#else
		np_ulong type = N_QOS_SEL_DATA_SCTP;
#endif

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = &type;
		QOS_length = sizeof(type);
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADQOSTYPE)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_8_conn	test_case_3_4_8
#define test_case_3_4_8_resp	test_case_3_4_8
#define test_case_3_4_8_list	test_case_3_4_8

#define preamble_3_4_8_conn	preamble_1_idle_cons
#define preamble_3_4_8_resp	preamble_1_idle_cons
#define preamble_3_4_8_list	preamble_1_idle_cons

#define postamble_3_4_8_conn	postamble_1_idle
#define postamble_3_4_8_resp	postamble_1_idle
#define postamble_3_4_8_list	postamble_1_idle

struct test_stream test_3_4_8_conn = { &preamble_3_4_8_conn, &test_case_3_4_8_conn, &postamble_3_4_8_conn };
struct test_stream test_3_4_8_resp = { &preamble_3_4_8_resp, &test_case_3_4_8_resp, &postamble_3_4_8_resp };
struct test_stream test_3_4_8_list = { &preamble_3_4_8_list, &test_case_3_4_8_list, &postamble_3_4_8_list };

#define tgrp_case_3_4_9 test_group_3
#define sgrp_case_3_4_9 test_group_3_4
#define numb_case_3_4_9 "3.4.9"
#define name_case_3_4_9 "NBADQOSTYPE for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_9 "NPI Rev 2.0.0"
#define desc_case_3_4_9 "\
Checks that NBADQOSTYPE is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Short structure."

int
test_case_3_4_9(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };
#if 0
		np_ulong type = N_QOS_SEL_CONN_IP;
#else
		np_ulong type = N_QOS_SEL_CONN_SCTP;
#endif

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = &type;
		QOS_length = sizeof(type);
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADQOSTYPE)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_9_conn	test_case_3_4_9
#define test_case_3_4_9_resp	test_case_3_4_9
#define test_case_3_4_9_list	test_case_3_4_9

#define preamble_3_4_9_conn	preamble_1_idle_cons
#define preamble_3_4_9_resp	preamble_1_idle_cons
#define preamble_3_4_9_list	preamble_1_idle_cons

#define postamble_3_4_9_conn	postamble_1_idle
#define postamble_3_4_9_resp	postamble_1_idle
#define postamble_3_4_9_list	postamble_1_idle

struct test_stream test_3_4_9_conn = { &preamble_3_4_9_conn, &test_case_3_4_9_conn, &postamble_3_4_9_conn };
struct test_stream test_3_4_9_resp = { &preamble_3_4_9_resp, &test_case_3_4_9_resp, &postamble_3_4_9_resp };
struct test_stream test_3_4_9_list = { &preamble_3_4_9_list, &test_case_3_4_9_list, &postamble_3_4_9_list };

#define tgrp_case_3_4_10 test_group_3
#define sgrp_case_3_4_10 test_group_3_4
#define numb_case_3_4_10 "3.4.10"
#define name_case_3_4_10 "NBADQOSPARAM for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_10 "NPI Rev 2.0.0"
#define desc_case_3_4_10 "\
Checks that NBADQOSPARAM is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Short structure."

int
test_case_3_4_10(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };
#if 0
		N_qos_sel_conn_ip_t qos = {
			.n_qos_type = N_QOS_SEL_CONN_IP,
			.protocol = 2000,
			.priority = 1000,
			.ttl = 500,
			.tos = 128,
			.mtu = 128000,
			.saddr = 0,
			.daddr = 0,
		};
#else
		N_qos_sel_conn_sctp_t qos = {
			.n_qos_type = N_QOS_SEL_CONN_SCTP,
		};
#endif

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = &qos;
		QOS_length = sizeof(qos)-1;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADQOSPARAM)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_10_conn	test_case_3_4_10
#define test_case_3_4_10_resp	test_case_3_4_10
#define test_case_3_4_10_list	test_case_3_4_10

#define preamble_3_4_10_conn	preamble_1_idle_cons
#define preamble_3_4_10_resp	preamble_1_idle_cons
#define preamble_3_4_10_list	preamble_1_idle_cons

#define postamble_3_4_10_conn	postamble_1_idle
#define postamble_3_4_10_resp	postamble_1_idle
#define postamble_3_4_10_list	postamble_1_idle

struct test_stream test_3_4_10_conn = { &preamble_3_4_10_conn, &test_case_3_4_10_conn, &postamble_3_4_10_conn };
struct test_stream test_3_4_10_resp = { &preamble_3_4_10_resp, &test_case_3_4_10_resp, &postamble_3_4_10_resp };
struct test_stream test_3_4_10_list = { &preamble_3_4_10_list, &test_case_3_4_10_list, &postamble_3_4_10_list };

#define tgrp_case_3_4_11 test_group_3
#define sgrp_case_3_4_11 test_group_3_4
#define numb_case_3_4_11 "3.4.11"
#define name_case_3_4_11 "NBADDATA for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_11 "NPI Rev 2.0.0"
#define desc_case_3_4_11 "\
Checks that NBADDATA is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Zero length data."

int
test_case_3_4_11(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };
		char buf[] = "Test data";

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = buf;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADDATA)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_11_conn	test_case_3_4_11
#define test_case_3_4_11_resp	test_case_3_4_11
#define test_case_3_4_11_list	test_case_3_4_11

#define preamble_3_4_11_conn	preamble_1_idle_cons
#define preamble_3_4_11_resp	preamble_1_idle_cons
#define preamble_3_4_11_list	preamble_1_idle_cons

#define postamble_3_4_11_conn	postamble_1_idle
#define postamble_3_4_11_resp	postamble_1_idle
#define postamble_3_4_11_list	postamble_1_idle

struct test_stream test_3_4_11_conn = { &preamble_3_4_11_conn, &test_case_3_4_11_conn, &postamble_3_4_11_conn };
struct test_stream test_3_4_11_resp = { &preamble_3_4_11_resp, &test_case_3_4_11_resp, &postamble_3_4_11_resp };
struct test_stream test_3_4_11_list = { &preamble_3_4_11_list, &test_case_3_4_11_list, &postamble_3_4_11_list };

#define tgrp_case_3_4_12 test_group_3
#define sgrp_case_3_4_12 test_group_3_4
#define numb_case_3_4_12 "3.4.12"
#define name_case_3_4_12 "NBADDATA for N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_4_12 "NPI Rev 2.0.0"
#define desc_case_3_4_12 "\
Checks that NBADDATA is returned when N_CONN_REQ is issued in the NS_IDLE state,\n\
under error conditions.  Too much data."

int
test_case_3_4_12(int child)
{
	if (child != 2) {
		unsigned short port = htons(10000 + child);
		struct sockaddr_in sin = { AF_INET, port, { htonl(0x7f000001) } };
		char buf[70000] = "Test data";

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = buf;
		DATA_length = 70000;

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADDATA)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_4_12_conn	test_case_3_4_12
#define test_case_3_4_12_resp	test_case_3_4_12
#define test_case_3_4_12_list	test_case_3_4_12

#define preamble_3_4_12_conn	preamble_1_idle_cons
#define preamble_3_4_12_resp	preamble_1_idle_cons
#define preamble_3_4_12_list	preamble_1_idle_cons

#define postamble_3_4_12_conn	postamble_1_idle
#define postamble_3_4_12_resp	postamble_1_idle
#define postamble_3_4_12_list	postamble_1_idle

struct test_stream test_3_4_12_conn = { &preamble_3_4_12_conn, &test_case_3_4_12_conn, &postamble_3_4_12_conn };
struct test_stream test_3_4_12_resp = { &preamble_3_4_12_resp, &test_case_3_4_12_resp, &postamble_3_4_12_resp };
struct test_stream test_3_4_12_list = { &preamble_3_4_12_list, &test_case_3_4_12_list, &postamble_3_4_12_list };

#define test_group_3_5 "3.5. Successful N_CONN_REQ in the NS_IDLE state"
#define tgrp_case_3_5_1 test_group_3
#define sgrp_case_3_5_1 test_group_3_5
#define numb_case_3_5_1 "3.5.1"
#define name_case_3_5_1 "Successful N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_5_1 "NPI Rev 2.0.0"
#define desc_case_3_5_1 "\
Checks that a connection can be formed using N_CONN_REQ in the NS_IDLE state\n\
with no data and no QOS parameters, single address."

int
test_case_3_5_1(int child)
{
	unsigned short port = addrs[2][0].sin_port;
	struct sockaddr_in sin = { AF_INET, port, { addrs[2][0].sin_addr.s_addr } };

	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = NULL;
	DATA_length = 0;

	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_5_1_othr(int child)
{
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

#define test_case_3_5_1_conn	test_case_3_5_1
#define test_case_3_5_1_resp	test_case_3_5_1_othr
#define test_case_3_5_1_list	test_case_3_5_1_othr

#define preamble_3_5_1_conn	preamble_1_idle_cons
#define preamble_3_5_1_resp	preamble_1_idle_cons
#define preamble_3_5_1_list	preamble_1_data_xfer_list

#define postamble_3_5_1_conn	postamble_1_data_xfer
#define postamble_3_5_1_resp	postamble_1_idle
#define postamble_3_5_1_list	postamble_1_data_xfer

struct test_stream test_3_5_1_conn = { &preamble_3_5_1_conn, &test_case_3_5_1_conn, &postamble_3_5_1_conn };
struct test_stream test_3_5_1_resp = { &preamble_3_5_1_resp, &test_case_3_5_1_resp, &postamble_3_5_1_resp };
struct test_stream test_3_5_1_list = { &preamble_3_5_1_list, &test_case_3_5_1_list, &postamble_3_5_1_list };

#define tgrp_case_3_5_2 test_group_3
#define sgrp_case_3_5_2 test_group_3_5
#define numb_case_3_5_2 "3.5.2"
#define name_case_3_5_2 "Successful N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_5_2 "NPI Rev 2.0.0"
#define desc_case_3_5_2 "\
Checks that a connection can be formed using N_CONN_REQ in the NS_IDLE state\n\
with no data and no QOS parameters, multiple addresses."

int
test_case_3_5_2(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = sizeof(addrs[2]);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = NULL;
	DATA_length = 0;

	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_5_2_othr(int child)
{
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

#define test_case_3_5_2_conn	test_case_3_5_2
#define test_case_3_5_2_resp	test_case_3_5_2_othr
#define test_case_3_5_2_list	test_case_3_5_2_othr

#define preamble_3_5_2_conn	preamble_1_idle_cons
#define preamble_3_5_2_resp	preamble_1_idle_cons
#define preamble_3_5_2_list	preamble_1_data_xfer_list

#define postamble_3_5_2_conn	postamble_1_data_xfer
#define postamble_3_5_2_resp	postamble_1_idle
#define postamble_3_5_2_list	postamble_1_data_xfer

struct test_stream test_3_5_2_conn = { &preamble_3_5_2_conn, &test_case_3_5_2_conn, &postamble_3_5_2_conn };
struct test_stream test_3_5_2_resp = { &preamble_3_5_2_resp, &test_case_3_5_2_resp, &postamble_3_5_2_resp };
struct test_stream test_3_5_2_list = { &preamble_3_5_2_list, &test_case_3_5_2_list, &postamble_3_5_2_list };

#define tgrp_case_3_5_3 test_group_3
#define sgrp_case_3_5_3 test_group_3_5
#define numb_case_3_5_3 "3.5.3"
#define name_case_3_5_3 "Successful N_CONN_REQ in the NS_IDLE state"
#define sref_case_3_5_3 "NPI Rev 2.0.0"
#define desc_case_3_5_3 "\
Checks that a connection can be formed using N_CONN_REQ in the NS_IDLE state\n\
with user data and no QOS parameters, multiple addresses."

int
test_case_3_5_3(int child)
{
	char buf[] = "xxxxTest Data";

	ADDR_buffer = addrs[2];
	ADDR_length = sizeof(addrs[2]);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = buf;
	DATA_length = 14;

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&addrs[child][0].sin_port, &buf[0], sizeof(addrs[child][0].sin_port));
	bcopy(&addrs[child][0].sin_port, &buf[2], sizeof(addrs[child][0].sin_port));

	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_5_3_othr(int child)
{
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

#define test_case_3_5_3_conn	test_case_3_5_3
#define test_case_3_5_3_resp	test_case_3_5_3_othr
#define test_case_3_5_3_list	test_case_3_5_3_othr

#define preamble_3_5_3_conn	preamble_1_idle_cons
#define preamble_3_5_3_resp	preamble_1_idle_cons
#define preamble_3_5_3_list	preamble_1_data_xfer_list

#define postamble_3_5_3_conn	postamble_1_data_xfer
#define postamble_3_5_3_resp	postamble_1_idle
#define postamble_3_5_3_list	postamble_1_data_xfer

struct test_stream test_3_5_3_conn = { &preamble_3_5_3_conn, &test_case_3_5_3_conn, &postamble_3_5_3_conn };
struct test_stream test_3_5_3_resp = { &preamble_3_5_3_resp, &test_case_3_5_3_resp, &postamble_3_5_3_resp };
struct test_stream test_3_5_3_list = { &preamble_3_5_3_list, &test_case_3_5_3_list, &postamble_3_5_3_list };

#define test_group_3_6 "3.6. Non-fatal errors in the NS_WRES_CIND state"
#define tgrp_case_3_6_1 test_group_3
#define sgrp_case_3_6_1 test_group_3_6
#define numb_case_3_6_1 "3.6.1"
#define name_case_3_6_1 "NOUTSTATE for N_BIND_REQ in the NS_WRES_CIND state"
#define sref_case_3_6_1 "NPI Rev 2.0.0"
#define desc_case_3_6_1 "\
Checks that NOUTSTATE is returned when N_BIND_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_6_1(int child)
{
	if (child == 2) {
		unsigned char proto = TEST_PROTOCOL;

		ADDR_buffer = NULL;
		ADDR_length = 0;
		CONIND_number = 0;
		BIND_flags = 0;
		PROTOID_buffer = &proto;
		PROTOID_length = sizeof(proto);

		if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_1_conn	test_case_3_6_1
#define test_case_3_6_1_resp	test_case_3_6_1
#define test_case_3_6_1_list	test_case_3_6_1

#define preamble_3_6_1_conn	preamble_1_wres_cind_conn
#define preamble_3_6_1_resp	preamble_1_wres_cind_resp
#define preamble_3_6_1_list	preamble_1_wres_cind_list

#define postamble_3_6_1_conn	postamble_1_data_xfer
#define postamble_3_6_1_resp	postamble_1_idle
#define postamble_3_6_1_list	postamble_1_refuse

struct test_stream test_3_6_1_conn = { &preamble_3_6_1_conn, &test_case_3_6_1_conn, &postamble_3_6_1_conn };
struct test_stream test_3_6_1_resp = { &preamble_3_6_1_resp, &test_case_3_6_1_resp, &postamble_3_6_1_resp };
struct test_stream test_3_6_1_list = { &preamble_3_6_1_list, &test_case_3_6_1_list, &postamble_3_6_1_list };

#define tgrp_case_3_6_2 test_group_3
#define sgrp_case_3_6_2 test_group_3_6
#define numb_case_3_6_2 "3.6.2"
#define name_case_3_6_2 "NOUTSTATE for N_UNBIND_REQ in the NS_WRES_CIND state"
#define sref_case_3_6_2 "NPI Rev 2.0.0"
#define desc_case_3_6_2 "\
Checks that NOUTSTATE is returned when N_UNBIND_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_6_2(int child)
{
	if (child == 2) {
		if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_2_conn	test_case_3_6_2
#define test_case_3_6_2_resp	test_case_3_6_2
#define test_case_3_6_2_list	test_case_3_6_2

#define preamble_3_6_2_conn	preamble_1_wres_cind_conn
#define preamble_3_6_2_resp	preamble_1_wres_cind_resp
#define preamble_3_6_2_list	preamble_1_wres_cind_list

#define postamble_3_6_2_conn	postamble_1_data_xfer
#define postamble_3_6_2_resp	postamble_1_idle
#define postamble_3_6_2_list	postamble_1_refuse

struct test_stream test_3_6_2_conn = { &preamble_3_6_2_conn, &test_case_3_6_2_conn, &postamble_3_6_2_conn };
struct test_stream test_3_6_2_resp = { &preamble_3_6_2_resp, &test_case_3_6_2_resp, &postamble_3_6_2_resp };
struct test_stream test_3_6_2_list = { &preamble_3_6_2_list, &test_case_3_6_2_list, &postamble_3_6_2_list };

#define tgrp_case_3_6_3 test_group_3
#define sgrp_case_3_6_3 test_group_3_6
#define numb_case_3_6_3 "3.6.3"
#define name_case_3_6_3 "NACCESS for N_CONN_REQ in the NS_WRES_CIND state"
#define sref_case_3_6_3 "NPI Rev 2.0.0"
#define desc_case_3_6_3 "\
Checks that NACCESS is returned when N_CONN_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_6_3(int child)
{
	if (child == 2) {
		unsigned short port = htons(10000 + 1);
		struct sockaddr_in sin[3] = {
			{AF_INET, port, {htonl(0x7f000001)}},
			{AF_INET, port, {htonl(0x7f000002)}},
			{AF_INET, port, {htonl(0x7f000003)}}
		};
		char buf[] = "xxxxTest Data";

		ADDR_buffer = sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NACCESS)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_3_conn	test_case_3_6_3
#define test_case_3_6_3_resp	test_case_3_6_3
#define test_case_3_6_3_list	test_case_3_6_3

#define preamble_3_6_3_conn	preamble_1_wres_cind_conn
#define preamble_3_6_3_resp	preamble_1_wres_cind_resp
#define preamble_3_6_3_list	preamble_1_wres_cind_list

#define postamble_3_6_3_conn	postamble_1_data_xfer
#define postamble_3_6_3_resp	postamble_1_idle
#define postamble_3_6_3_list	postamble_1_refuse

struct test_stream test_3_6_3_conn = { &preamble_3_6_3_conn, &test_case_3_6_3_conn, &postamble_3_6_3_conn };
struct test_stream test_3_6_3_resp = { &preamble_3_6_3_resp, &test_case_3_6_3_resp, &postamble_3_6_3_resp };
struct test_stream test_3_6_3_list = { &preamble_3_6_3_list, &test_case_3_6_3_list, &postamble_3_6_3_list };

#define tgrp_case_3_6_4 test_group_3
#define sgrp_case_3_6_4 test_group_3_6
#define numb_case_3_6_4 "3.6.4"
#define name_case_3_6_4 "NOUTSTATE for N_RESET_REQ in the NS_WRES_CIND state"
#define sref_case_3_6_4 "NPI Rev 2.0.0"
#define desc_case_3_6_4 "\
Checks that NOUTSTATE is returned when N_RESET_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_6_4(int child)
{
	if (child == 2) {
		RESET_reason = N_RESET_UNSPECIFIED;
		if (do_signal(child, __TEST_RESET_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_4_conn	test_case_3_6_4
#define test_case_3_6_4_resp	test_case_3_6_4
#define test_case_3_6_4_list	test_case_3_6_4

#define preamble_3_6_4_conn	preamble_1_wres_cind_conn
#define preamble_3_6_4_resp	preamble_1_wres_cind_resp
#define preamble_3_6_4_list	preamble_1_wres_cind_list

#define postamble_3_6_4_conn	postamble_1_data_xfer
#define postamble_3_6_4_resp	postamble_1_idle
#define postamble_3_6_4_list	postamble_1_refuse

struct test_stream test_3_6_4_conn = { &preamble_3_6_4_conn, &test_case_3_6_4_conn, &postamble_3_6_4_conn };
struct test_stream test_3_6_4_resp = { &preamble_3_6_4_resp, &test_case_3_6_4_resp, &postamble_3_6_4_resp };
struct test_stream test_3_6_4_list = { &preamble_3_6_4_list, &test_case_3_6_4_list, &postamble_3_6_4_list };

#define tgrp_case_3_6_5 test_group_3
#define sgrp_case_3_6_5 test_group_3_6
#define numb_case_3_6_5 "3.6.5"
#define name_case_3_6_5 "NOUTSTATE for N_RESET_RES in the NS_WRES_CIND state"
#define sref_case_3_6_5 "NPI Rev 2.0.0"
#define desc_case_3_6_5 "\
Checks that NOUTSTATE is returned when N_RESET_RES is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_6_5(int child)
{
	if (child == 2) {
		if (do_signal(child, __TEST_RESET_RES) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_5_conn	test_case_3_6_5
#define test_case_3_6_5_resp	test_case_3_6_5
#define test_case_3_6_5_list	test_case_3_6_5

#define preamble_3_6_5_conn	preamble_1_wres_cind_conn
#define preamble_3_6_5_resp	preamble_1_wres_cind_resp
#define preamble_3_6_5_list	preamble_1_wres_cind_list

#define postamble_3_6_5_conn	postamble_1_data_xfer
#define postamble_3_6_5_resp	postamble_1_idle
#define postamble_3_6_5_list	postamble_1_refuse

struct test_stream test_3_6_5_conn = { &preamble_3_6_5_conn, &test_case_3_6_5_conn, &postamble_3_6_5_conn };
struct test_stream test_3_6_5_resp = { &preamble_3_6_5_resp, &test_case_3_6_5_resp, &postamble_3_6_5_resp };
struct test_stream test_3_6_5_list = { &preamble_3_6_5_list, &test_case_3_6_5_list, &postamble_3_6_5_list };

#define tgrp_case_3_6_6 test_group_3
#define sgrp_case_3_6_6 test_group_3_6
#define numb_case_3_6_6 "3.6.6"
#define name_case_3_6_6 "NBADSEQ for N_DISCON_REQ in the NS_WRES_CIND state"
#define sref_case_3_6_6 "NPI Rev 2.0.0"
#define desc_case_3_6_6 "\
Checks that NOUTSTATE is returned when N_DISCON_REQ is issued in the\n\
NS_WRES_CIND state (with a zero sequence number)."

int
test_case_3_6_6(int child)
{
	if (child == 2) {
		int seq = SEQ_number;

		SEQ_number = 0;
		ADDR_buffer = NULL;
		ADDR_length = 0;
		DISCON_reason = N_REASON_UNDEFINED;
		DATA_buffer = NULL;
		DATA_length = 0;
		if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS) {
			SEQ_number = seq;
			goto failure;
		}
		SEQ_number = seq;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADSEQ)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_WRES_CIND)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_6_6_conn	test_case_3_6_6
#define test_case_3_6_6_resp	test_case_3_6_6
#define test_case_3_6_6_list	test_case_3_6_6

#define preamble_3_6_6_conn	preamble_1_wres_cind_conn
#define preamble_3_6_6_resp	preamble_1_wres_cind_resp
#define preamble_3_6_6_list	preamble_1_wres_cind_list

#define postamble_3_6_6_conn	postamble_1_data_xfer
#define postamble_3_6_6_resp	postamble_1_idle
#define postamble_3_6_6_list	postamble_1_refuse

struct test_stream test_3_6_6_conn = { &preamble_3_6_6_conn, &test_case_3_6_6_conn, &postamble_3_6_6_conn };
struct test_stream test_3_6_6_resp = { &preamble_3_6_6_resp, &test_case_3_6_6_resp, &postamble_3_6_6_resp };
struct test_stream test_3_6_6_list = { &preamble_3_6_6_list, &test_case_3_6_6_list, &postamble_3_6_6_list };

#define test_group_3_7 "3.7. Fatal errors in the NS_WRES_CIND state"
#define tgrp_case_3_7_1 test_group_3
#define sgrp_case_3_7_1 test_group_3_7
#define numb_case_3_7_1 "3.7.1"
#define name_case_3_7_1 "EPROTO for N_DATA_REQ in the NS_WRES_CIND state"
#define sref_case_3_7_1 "NPI Rev 2.0.0"
#define desc_case_3_7_1 "\
Checks that EPROTO is returned when N_DATA_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_7_1(int child)
{
	if (child == 2) {
		unsigned short port = htons(10000 + 2);
		char buf[] = "xxxxTest Data";

		DATA_xfer_flags = 0;
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		QOS_length = 0;
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_7_1_conn	test_case_3_7_1
#define test_case_3_7_1_resp	test_case_3_7_1
#define test_case_3_7_1_list	test_case_3_7_1

#define preamble_3_7_1_conn	preamble_1_wres_cind_conn
#define preamble_3_7_1_resp	preamble_1_wres_cind_resp
#define preamble_3_7_1_list	preamble_1_wres_cind_list

#define postamble_3_7_1_conn	postamble_1_data_xfer
#define postamble_3_7_1_resp	postamble_1_idle
#define postamble_3_7_1_list	postamble_1_refuse

struct test_stream test_3_7_1_conn = { &preamble_3_7_1_conn, &test_case_3_7_1_conn, &postamble_3_7_1_conn };
struct test_stream test_3_7_1_resp = { &preamble_3_7_1_resp, &test_case_3_7_1_resp, &postamble_3_7_1_resp };
struct test_stream test_3_7_1_list = { &preamble_3_7_1_list, &test_case_3_7_1_list, &postamble_3_7_1_list };

#define tgrp_case_3_7_2 test_group_3
#define sgrp_case_3_7_2 test_group_3_7
#define numb_case_3_7_2 "3.7.2"
#define name_case_3_7_2 "EPROTO for N_EXDATA_REQ in the NS_WRES_CIND state"
#define sref_case_3_7_2 "NPI Rev 2.0.0"
#define desc_case_3_7_2 "\
Checks that EPROTO is returned when N_EXDATA_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_7_2(int child)
{
	if (child == 2) {
		unsigned short port = htons(10000 + 2);
		char buf[] = "xxxxTest Data";

		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		QOS_length = 0;
		if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_7_2_conn	test_case_3_7_2
#define test_case_3_7_2_resp	test_case_3_7_2
#define test_case_3_7_2_list	test_case_3_7_2

#define preamble_3_7_2_conn	preamble_1_wres_cind_conn
#define preamble_3_7_2_resp	preamble_1_wres_cind_resp
#define preamble_3_7_2_list	preamble_1_wres_cind_list

#define postamble_3_7_2_conn	postamble_1_data_xfer
#define postamble_3_7_2_resp	postamble_1_idle
#define postamble_3_7_2_list	postamble_1_refuse

struct test_stream test_3_7_2_conn = { &preamble_3_7_2_conn, &test_case_3_7_2_conn, &postamble_3_7_2_conn };
struct test_stream test_3_7_2_resp = { &preamble_3_7_2_resp, &test_case_3_7_2_resp, &postamble_3_7_2_resp };
struct test_stream test_3_7_2_list = { &preamble_3_7_2_list, &test_case_3_7_2_list, &postamble_3_7_2_list };

#define tgrp_case_3_7_3 test_group_3
#define sgrp_case_3_7_3 test_group_3_7
#define numb_case_3_7_3 "3.7.3"
#define name_case_3_7_3 "EPROTO for N_DATACK_REQ in the NS_WRES_CIND state"
#define sref_case_3_7_3 "NPI Rev 2.0.0"
#define desc_case_3_7_3 "\
Checks that EPROTO is returned when N_DATACK_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_7_3(int child)
{
	if (child == 2) {
		QOS_length = 0;
		if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_7_3_conn	test_case_3_7_3
#define test_case_3_7_3_resp	test_case_3_7_3
#define test_case_3_7_3_list	test_case_3_7_3

#define preamble_3_7_3_conn	preamble_1_wres_cind_conn
#define preamble_3_7_3_resp	preamble_1_wres_cind_resp
#define preamble_3_7_3_list	preamble_1_wres_cind_list

#define postamble_3_7_3_conn	postamble_1_data_xfer
#define postamble_3_7_3_resp	postamble_1_idle
#define postamble_3_7_3_list	postamble_1_refuse

struct test_stream test_3_7_3_conn = { &preamble_3_7_3_conn, &test_case_3_7_3_conn, &postamble_3_7_3_conn };
struct test_stream test_3_7_3_resp = { &preamble_3_7_3_resp, &test_case_3_7_3_resp, &postamble_3_7_3_resp };
struct test_stream test_3_7_3_list = { &preamble_3_7_3_list, &test_case_3_7_3_list, &postamble_3_7_3_list };

#define tgrp_case_3_7_4 test_group_3
#define sgrp_case_3_7_4 test_group_3_7
#define numb_case_3_7_4 "3.7.4"
#define name_case_3_7_4 "EPROTO for N_UNITDATA_REQ in the NS_WRES_CIND state"
#define sref_case_3_7_4 "NPI Rev 2.0.0"
#define desc_case_3_7_4 "\
Checks that EPROTO is returned when N_UNITDATA_REQ is issued in the\n\
NS_WRES_CIND state."

int
test_case_3_7_4(int child)
{
	return (__RESULT_SKIPPED);
	if (child == 2) {
		unsigned short port = htons(10000 + 1);
		struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };
		char buf[] = "xxxxTest Data";

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_7_4_conn	test_case_3_7_4
#define test_case_3_7_4_resp	test_case_3_7_4
#define test_case_3_7_4_list	test_case_3_7_4

#define preamble_3_7_4_conn	preamble_1_wres_cind_conn
#define preamble_3_7_4_resp	preamble_1_wres_cind_resp
#define preamble_3_7_4_list	preamble_1_wres_cind_list

#define postamble_3_7_4_conn	postamble_1_data_xfer
#define postamble_3_7_4_resp	postamble_1_idle
#define postamble_3_7_4_list	postamble_1_refuse

struct test_stream test_3_7_4_conn = { &preamble_3_7_4_conn, &test_case_3_7_4_conn, &postamble_3_7_4_conn };
struct test_stream test_3_7_4_resp = { &preamble_3_7_4_resp, &test_case_3_7_4_resp, &postamble_3_7_4_resp };
struct test_stream test_3_7_4_list = { &preamble_3_7_4_list, &test_case_3_7_4_list, &postamble_3_7_4_list };

#define test_group_3_8 "3.8. Connection indications in the NS_IDLE state"
#define tgrp_case_3_8_1 test_group_3
#define sgrp_case_3_8_1 test_group_3_8
#define numb_case_3_8_1 "3.8.1"
#define name_case_3_8_1 "Successful N_DISCON_REQ in the NS_WRES_CIND state"
#define sref_case_3_8_1 "NPI Rev 2.0.0"
#define desc_case_3_8_1 "\
Checks that a connection indication can be successfully refused with an\n\
N_DISCON_REQ in the NS_WRES_CIND state."

int
test_case_3_8_1(int child)
{
	if (child == 2) {
		ADDR_buffer = NULL;
		ADDR_length = 0;
		DISCON_reason = N_REASON_UNDEFINED;
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
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
		if (last_info.CURRENT_state != NS_IDLE)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_8_1_conn	test_case_3_8_1
#define test_case_3_8_1_resp	test_case_3_8_1
#define test_case_3_8_1_list	test_case_3_8_1

#define preamble_3_8_1_conn	preamble_1_wres_cind_conn
#define preamble_3_8_1_resp	preamble_1_wres_cind_resp
#define preamble_3_8_1_list	preamble_1_wres_cind_list

#define postamble_3_8_1_conn	postamble_1_data_xfer
#define postamble_3_8_1_resp	postamble_1_idle
#define postamble_3_8_1_list	postamble_1_idle

struct test_stream test_3_8_1_conn = { &preamble_3_8_1_conn, &test_case_3_8_1_conn, &postamble_3_8_1_conn };
struct test_stream test_3_8_1_resp = { &preamble_3_8_1_resp, &test_case_3_8_1_resp, &postamble_3_8_1_resp };
struct test_stream test_3_8_1_list = { &preamble_3_8_1_list, &test_case_3_8_1_list, &postamble_3_8_1_list };

#define tgrp_case_3_8_2 test_group_3
#define sgrp_case_3_8_2 test_group_3_8
#define numb_case_3_8_2 "3.8.2"
#define name_case_3_8_2 "Successful N_CONN_RES in the NS_WRES_CIND state"
#define sref_case_3_8_2 "NPI Rev 2.0.0"
#define desc_case_3_8_2 "\
Checks that a connection indication can be successfully accepted with an\n\
N_CONN_RES in the NS_WRES_CIND state (same stream)."

int
test_case_3_8_2(int child)
{
	if (child == 2) {
		char buf[] = "xxxxTest Data";
#if 0
		N_qos_sel_conn_ip_t qos = {
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
		N_qos_sel_conn_sctp_t qos = {
			.n_qos_type = N_QOS_SEL_CONN_SCTP,
		};
#endif

		test_resfd = test_fd[child];
		TOKEN_value = 0;
		ADDR_buffer = addrs[child];
		ADDR_length = sizeof(addrs[child]);
		CONN_flags = 0;
		QOS_buffer = &qos;
		QOS_length = sizeof(qos);
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is
		   meaningless. */
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
	}
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_8_2_conn	test_case_3_8_2
#define test_case_3_8_2_resp	test_case_3_8_2
#define test_case_3_8_2_list	test_case_3_8_2

#define preamble_3_8_2_conn	preamble_1_wres_cind_conn
#define preamble_3_8_2_resp	preamble_1_wres_cind_resp
#define preamble_3_8_2_list	preamble_1_wres_cind_list

#define postamble_3_8_2_conn	postamble_1_data_xfer
#define postamble_3_8_2_resp	postamble_1_idle
#define postamble_3_8_2_list	postamble_1_refuse

struct test_stream test_3_8_2_conn = { &preamble_3_8_2_conn, &test_case_3_8_2_conn, &postamble_3_8_2_conn };
struct test_stream test_3_8_2_resp = { &preamble_3_8_2_resp, &test_case_3_8_2_resp, &postamble_3_8_2_resp };
struct test_stream test_3_8_2_list = { &preamble_3_8_2_list, &test_case_3_8_2_list, &postamble_3_8_2_list };

#define tgrp_case_3_8_3 test_group_3
#define sgrp_case_3_8_3 test_group_3_8
#define numb_case_3_8_3 "3.8.3"
#define name_case_3_8_3 "Successful N_CONN_RES in the NS_WRES_CIND state"
#define sref_case_3_8_3 "NPI Rev 2.0.0"
#define desc_case_3_8_3 "\
Checks that a connection indication can be successfully accepted with an\n\
N_CONN_RES in the NS_WRES_CIND state (different stream)."

int
test_case_3_8_3(int child)
{
	return (__RESULT_SKIPPED); /* later */
	if (child == 2) {
		int port = htons(10000 + 0);
		struct sockaddr_in sin[3] = {
			{AF_INET, port, {htonl(0x7f000001)}},
			{AF_INET, port, {htonl(0x7f000002)}},
			{AF_INET, port, {htonl(0x7f000003)}}
		};
		char buf[] = "xxxxTest Data";
#if 0
		N_qos_sel_conn_ip_t qos = {
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
		N_qos_sel_conn_sctp_t qos = {
			.n_qos_type = N_QOS_SEL_CONN_SCTP,
		};
#endif

		TOKEN_value = 0;
		ADDR_buffer = sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = &qos;
		QOS_length = sizeof(qos);
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is
		   meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

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
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_8_3_conn	test_case_3_8_3
#define test_case_3_8_3_resp	test_case_3_8_3
#define test_case_3_8_3_list	test_case_3_8_3

#define preamble_3_8_3_conn	preamble_1_wres_cind_conn
#define preamble_3_8_3_resp	preamble_1_wres_cind_resp
#define preamble_3_8_3_list	preamble_1_wres_cind_list

#define postamble_3_8_3_conn	postamble_1_data_xfer
#define postamble_3_8_3_resp	postamble_1_idle
#define postamble_3_8_3_list	postamble_1_refuse

struct test_stream test_3_8_3_conn = { &preamble_3_8_3_conn, &test_case_3_8_3_conn, &postamble_3_8_3_conn };
struct test_stream test_3_8_3_resp = { &preamble_3_8_3_resp, &test_case_3_8_3_resp, &postamble_3_8_3_resp };
struct test_stream test_3_8_3_list = { &preamble_3_8_3_list, &test_case_3_8_3_list, &postamble_3_8_3_list };

#define test_group_3_9 "3.9. Non-fatal errors in the NS_DATA_XFER state"
#define tgrp_case_3_9_1 test_group_3
#define sgrp_case_3_9_1 test_group_3_9
#define numb_case_3_9_1 "3.9.1"
#define name_case_3_9_1 "NOUTSTATE for N_BIND_REQ in the NS_DATA_XFER state"
#define sref_case_3_9_1 "NPI Rev 2.0.0"
#define desc_case_3_9_1 "\
Checks that NOUTSTATE is returned when N_BIND_REQ is issued in the\n\
NS_DATA_XFER state."

int
test_case_3_9_1(int child)
{
	if (child != 1) {
		unsigned char proto = TEST_PROTOCOL;

		ADDR_buffer = NULL;
		ADDR_length = 0;
		CONIND_number = 0;
		BIND_flags = 0;
		PROTOID_buffer = &proto;
		PROTOID_length = sizeof(proto);

		if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
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
	} else {
		test_msleep(child, LONGER_WAIT);
		state++;
	}
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_9_1_conn	test_case_3_9_1
#define test_case_3_9_1_resp	test_case_3_9_1
#define test_case_3_9_1_list	test_case_3_9_1

#define preamble_3_9_1_conn	preamble_1_data_xfer_conn
#define preamble_3_9_1_resp	preamble_1_idle_cons
#define preamble_3_9_1_list	preamble_1_data_xfer_list

#define postamble_3_9_1_conn	postamble_1_data_xfer
#define postamble_3_9_1_resp	postamble_1_idle
#define postamble_3_9_1_list	postamble_1_data_xfer

struct test_stream test_3_9_1_conn = { &preamble_3_9_1_conn, &test_case_3_9_1_conn, &postamble_3_9_1_conn };
struct test_stream test_3_9_1_resp = { &preamble_3_9_1_resp, &test_case_3_9_1_resp, &postamble_3_9_1_resp };
struct test_stream test_3_9_1_list = { &preamble_3_9_1_list, &test_case_3_9_1_list, &postamble_3_9_1_list };

#define tgrp_case_3_9_2 test_group_3
#define sgrp_case_3_9_2 test_group_3_9
#define numb_case_3_9_2 "3.9.2"
#define name_case_3_9_2 "NOUTSTATE for N_UNBIND_REQ in the NS_DATA_XFER state"
#define sref_case_3_9_2 "NPI Rev 2.0.0"
#define desc_case_3_9_2 "\
Checks that NOUTSTATE is returned when N_UNBIND_REQ is issued in the\n\
NS_DATA_XFER state."

int
test_case_3_9_2(int child)
{
	if (child != 1) {
		if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE)
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
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_9_2_conn	test_case_3_9_2
#define test_case_3_9_2_resp	test_case_3_9_2
#define test_case_3_9_2_list	test_case_3_9_2

#define preamble_3_9_2_conn	preamble_1_data_xfer_conn
#define preamble_3_9_2_resp	preamble_1_data_xfer_resp
#define preamble_3_9_2_list	preamble_1_data_xfer_list

#define postamble_3_9_2_conn	postamble_1_data_xfer
#define postamble_3_9_2_resp	postamble_1_idle
#define postamble_3_9_2_list	postamble_1_data_xfer

struct test_stream test_3_9_2_conn = { &preamble_3_9_2_conn, &test_case_3_9_2_conn, &postamble_3_9_2_conn };
struct test_stream test_3_9_2_resp = { &preamble_3_9_2_resp, &test_case_3_9_2_resp, &postamble_3_9_2_resp };
struct test_stream test_3_9_2_list = { &preamble_3_9_2_list, &test_case_3_9_2_list, &postamble_3_9_2_list };

#define tgrp_case_3_9_3 test_group_3
#define sgrp_case_3_9_3 test_group_3_9
#define numb_case_3_9_3 "3.9.3"
#define name_case_3_9_3 "NOUTSTATE/NACCESS for N_CONN_REQ in the NS_DATA_XFER state"
#define sref_case_3_9_3 "NPI Rev 2.0.0"
#define desc_case_3_9_3 "\
Checks that NOUTSTATE/NACCESS is returned when N_CONN_REQ is issued in the\n\
NS_DATA_XFER state."

int
test_case_3_9_3(int child)
{
	if (child != 1) {
		unsigned short port = htons(10000 + 1);
		struct sockaddr_in sin[3] = {
			{AF_INET, port, {htonl(0x7f000001)}},
			{AF_INET, port, {htonl(0x7f000002)}},
			{AF_INET, port, {htonl(0x7f000003)}}
		};
		char buf[] = "xxxxTest Data";

		ADDR_buffer = sin;
		ADDR_length = sizeof(sin);
		CONN_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE && NPI_error != NACCESS)
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
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_9_3_conn	test_case_3_9_3
#define test_case_3_9_3_resp	test_case_3_9_3
#define test_case_3_9_3_list	test_case_3_9_3

#define preamble_3_9_3_conn	preamble_1_data_xfer_conn
#define preamble_3_9_3_resp	preamble_1_data_xfer_resp
#define preamble_3_9_3_list	preamble_1_data_xfer_list

#define postamble_3_9_3_conn	postamble_1_data_xfer
#define postamble_3_9_3_resp	postamble_1_idle
#define postamble_3_9_3_list	postamble_1_data_xfer

struct test_stream test_3_9_3_conn = { &preamble_3_9_3_conn, &test_case_3_9_3_conn, &postamble_3_9_3_conn };
struct test_stream test_3_9_3_resp = { &preamble_3_9_3_resp, &test_case_3_9_3_resp, &postamble_3_9_3_resp };
struct test_stream test_3_9_3_list = { &preamble_3_9_3_list, &test_case_3_9_3_list, &postamble_3_9_3_list };

#define tgrp_case_3_9_4 test_group_3
#define sgrp_case_3_9_4 test_group_3_9
#define numb_case_3_9_4 "3.9.4"
#define name_case_3_9_4 "NOUTSTATE for N_RESET_RES in the NS_DATA_XFER state"
#define sref_case_3_9_4 "NPI Rev 2.0.0"
#define desc_case_3_9_4 "\
Checks that NOUTSTATE is returned when N_RESET_RES is issued in the\n\
NS_DATA_XFER state."

int
test_case_3_9_4(int child)
{
	if (child != 1) {
		if (do_signal(child, __TEST_RESET_RES) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NOUTSTATE && NPI_error != NACCESS)
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
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_9_4_conn	test_case_3_9_4
#define test_case_3_9_4_resp	test_case_3_9_4
#define test_case_3_9_4_list	test_case_3_9_4

#define preamble_3_9_4_conn	preamble_1_data_xfer_conn
#define preamble_3_9_4_resp	preamble_1_data_xfer_resp
#define preamble_3_9_4_list	preamble_1_data_xfer_list

#define postamble_3_9_4_conn	postamble_1_data_xfer
#define postamble_3_9_4_resp	postamble_1_idle
#define postamble_3_9_4_list	postamble_1_data_xfer

struct test_stream test_3_9_4_conn = { &preamble_3_9_4_conn, &test_case_3_9_4_conn, &postamble_3_9_4_conn };
struct test_stream test_3_9_4_resp = { &preamble_3_9_4_resp, &test_case_3_9_4_resp, &postamble_3_9_4_resp };
struct test_stream test_3_9_4_list = { &preamble_3_9_4_list, &test_case_3_9_4_list, &postamble_3_9_4_list };

#define tgrp_case_3_9_5 test_group_3
#define sgrp_case_3_9_5 test_group_3_9
#define numb_case_3_9_5 "3.9.5"
#define name_case_3_9_5 "NBADSEQ for N_DISCON_REQ in the NS_DATA_XFER state"
#define sref_case_3_9_5 "NPI Rev 2.0.0"
#define desc_case_3_9_5 "\
Checks that NBADSEQ is returned when N_DISCON_REQ is issued in the\n\
NS_DATA_XFER state (non-zero sequence number)."

int
test_case_3_9_5(int child)
{
	if (child != 1) {
		int seq = SEQ_number;

		ADDR_buffer = NULL;
		ADDR_length = 0;
		DISCON_reason = N_REASON_UNDEFINED;
		DATA_buffer = NULL;
		DATA_length = 0;
		SEQ_number = 4;
		if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS) {
			SEQ_number = seq;
			goto failure;
		}
		SEQ_number = seq;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (NPI_error != NBADSEQ)
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
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_9_5_conn	test_case_3_9_5
#define test_case_3_9_5_resp	test_case_3_9_5
#define test_case_3_9_5_list	test_case_3_9_5

#define preamble_3_9_5_conn	preamble_1_data_xfer_conn
#define preamble_3_9_5_resp	preamble_1_data_xfer_resp
#define preamble_3_9_5_list	preamble_1_data_xfer_list

#define postamble_3_9_5_conn	postamble_1_data_xfer
#define postamble_3_9_5_resp	postamble_1_idle
#define postamble_3_9_5_list	postamble_1_data_xfer

struct test_stream test_3_9_5_conn = { &preamble_3_9_5_conn, &test_case_3_9_5_conn, &postamble_3_9_5_conn };
struct test_stream test_3_9_5_resp = { &preamble_3_9_5_resp, &test_case_3_9_5_resp, &postamble_3_9_5_resp };
struct test_stream test_3_9_5_list = { &preamble_3_9_5_list, &test_case_3_9_5_list, &postamble_3_9_5_list };

#define test_group_3_10 "3.10. Fatal errors in the NS_DATA_XFER state"
#define tgrp_case_3_10_1 test_group_3
#define sgrp_case_3_10_1 test_group_3_10
#define numb_case_3_10_1 "3.10.1"
#define name_case_3_10_1 "EPROTO for N_DATA_REQ in the NS_DATA_XFER state"
#define sref_case_3_10_1 "NPI Rev 2.0.0"
#define desc_case_3_10_1 "\
Checks that EPROTO is returned when N_DATA_REQ is issued in the\n\
NS_DATA_XFER state (zero length data)."

int
test_case_3_10_1(int child)
{
	if (child != 1) {
		char buf[] = "Test Data.";

		DATA_xfer_flags = 0;
		DATA_buffer = buf;
		DATA_length = 0;

		QOS_length = 0;
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	test_msleep(child, LONG_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_10_1_conn	test_case_3_10_1
#define test_case_3_10_1_resp	test_case_3_10_1
#define test_case_3_10_1_list	test_case_3_10_1

#define preamble_3_10_1_conn	preamble_1_data_xfer_conn
#define preamble_3_10_1_resp	preamble_1_data_xfer_resp
#define preamble_3_10_1_list	preamble_1_data_xfer_list

#define postamble_3_10_1_conn	postamble_1_data_xfer
#define postamble_3_10_1_resp	postamble_1_idle
#define postamble_3_10_1_list	postamble_1_data_xfer

struct test_stream test_3_10_1_conn = { &preamble_3_10_1_conn, &test_case_3_10_1_conn, &postamble_3_10_1_conn };
struct test_stream test_3_10_1_resp = { &preamble_3_10_1_resp, &test_case_3_10_1_resp, &postamble_3_10_1_resp };
struct test_stream test_3_10_1_list = { &preamble_3_10_1_list, &test_case_3_10_1_list, &postamble_3_10_1_list };

#define tgrp_case_3_10_2 test_group_3
#define sgrp_case_3_10_2 test_group_3_10
#define numb_case_3_10_2 "3.10.2"
#define name_case_3_10_2 "EPROTO for N_EXDATA_REQ in the NS_DATA_XFER state"
#define sref_case_3_10_2 "NPI Rev 2.0.0"
#define desc_case_3_10_2 "\
Checks that EPROTO is returned when N_EXDATA_REQ is issued in the\n\
NS_DATA_XFER state (zero length data)."

int
test_case_3_10_2(int child)
{
	if (child != 1) {
		char buf[] = "Test Data.";

		DATA_buffer = buf;
		DATA_length = 0;

		QOS_length = 0;
		if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_10_2_conn	test_case_3_10_2
#define test_case_3_10_2_resp	test_case_3_10_2
#define test_case_3_10_2_list	test_case_3_10_2

#define preamble_3_10_2_conn	preamble_1_data_xfer_conn
#define preamble_3_10_2_resp	preamble_1_data_xfer_resp
#define preamble_3_10_2_list	preamble_1_data_xfer_list

#define postamble_3_10_2_conn	postamble_1_data_xfer
#define postamble_3_10_2_resp	postamble_1_idle
#define postamble_3_10_2_list	postamble_1_data_xfer

struct test_stream test_3_10_2_conn = { &preamble_3_10_2_conn, &test_case_3_10_2_conn, &postamble_3_10_2_conn };
struct test_stream test_3_10_2_resp = { &preamble_3_10_2_resp, &test_case_3_10_2_resp, &postamble_3_10_2_resp };
struct test_stream test_3_10_2_list = { &preamble_3_10_2_list, &test_case_3_10_2_list, &postamble_3_10_2_list };

#define tgrp_case_3_10_3 test_group_3
#define sgrp_case_3_10_3 test_group_3_10
#define numb_case_3_10_3 "3.10.3"
#define name_case_3_10_3 "EPROTO for N_DATACK_REQ in the NS_DATA_XFER state"
#define sref_case_3_10_3 "NPI Rev 2.0.0"
#define desc_case_3_10_3 "\
Checks that EPROTO is returned when N_DATACK_REQ is issued in the\n\
NS_DATA_XFER state (zero length data)."

int
test_case_3_10_3(int child)
{
	if (child != 1) {
		QOS_length = 0;
		if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_10_3_conn	test_case_3_10_3
#define test_case_3_10_3_resp	test_case_3_10_3
#define test_case_3_10_3_list	test_case_3_10_3

#define preamble_3_10_3_conn	preamble_1_data_xfer_conn
#define preamble_3_10_3_resp	preamble_1_data_xfer_resp
#define preamble_3_10_3_list	preamble_1_data_xfer_list

#define postamble_3_10_3_conn	postamble_1_data_xfer
#define postamble_3_10_3_resp	postamble_1_idle
#define postamble_3_10_3_list	postamble_1_data_xfer

struct test_stream test_3_10_3_conn = { &preamble_3_10_3_conn, &test_case_3_10_3_conn, &postamble_3_10_3_conn };
struct test_stream test_3_10_3_resp = { &preamble_3_10_3_resp, &test_case_3_10_3_resp, &postamble_3_10_3_resp };
struct test_stream test_3_10_3_list = { &preamble_3_10_3_list, &test_case_3_10_3_list, &postamble_3_10_3_list };

#define tgrp_case_3_10_4 test_group_3
#define sgrp_case_3_10_4 test_group_3_10
#define numb_case_3_10_4 "3.10.4"
#define name_case_3_10_4 "EPROTO for N_UNITDATA_REQ in the NS_DATA_XFER state"
#define sref_case_3_10_4 "NPI Rev 2.0.0"
#define desc_case_3_10_4 "\
Checks that EPROTO is returned when N_UNITDATA_REQ is issued in the\n\
NS_DATA_XFER state (zero length data)."

int
test_case_3_10_4(int child)
{
	if (child != 1) {
		unsigned short port = htons(10000 + 1);
		struct sockaddr_in sin = { AF_INET, port, {htonl(0x7f000001)} };
		char buf[] = "xxxxTest Data";

		ADDR_buffer = &sin;
		ADDR_length = sizeof(sin);
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&port, &buf[0], sizeof(port));
		bcopy(&port, &buf[2], sizeof(port));

		if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (UNIX_error != EPROTO)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_10_4_conn	test_case_3_10_4
#define test_case_3_10_4_resp	test_case_3_10_4
#define test_case_3_10_4_list	test_case_3_10_4

#define preamble_3_10_4_conn	preamble_1_data_xfer_conn
#define preamble_3_10_4_resp	preamble_1_data_xfer_resp
#define preamble_3_10_4_list	preamble_1_data_xfer_list

#define postamble_3_10_4_conn	postamble_1_data_xfer
#define postamble_3_10_4_resp	postamble_1_idle
#define postamble_3_10_4_list	postamble_1_data_xfer

struct test_stream test_3_10_4_conn = { &preamble_3_10_4_conn, &test_case_3_10_4_conn, &postamble_3_10_4_conn };
struct test_stream test_3_10_4_resp = { &preamble_3_10_4_resp, &test_case_3_10_4_resp, &postamble_3_10_4_resp };
struct test_stream test_3_10_4_list = { &preamble_3_10_4_list, &test_case_3_10_4_list, &postamble_3_10_4_list };


#define test_group_3_11 "3.11. Successful data transfer in the NS_DATA_XFER state"
#define tgrp_case_3_11_1 test_group_3
#define sgrp_case_3_11_1 test_group_3_10
#define numb_case_3_11_1 "3.11.1"
#define name_case_3_11_1 "N_DATA_REQ in the NS_DATA_XFER state"
#define sref_case_3_11_1 "NPI Rev 2.0.0"
#define desc_case_3_11_1 "\
Checks that data can be successfully transferred in the NS_DATA_XFER state."

int
test_case_3_11_1(int child)
{
	if (child != 1) {
		unsigned short sport = htons((child == 0) ? ports[0] : ports[2]);
		unsigned short dport = htons((child == 0) ? ports[2] : ports[0]);
		char buf[] = "xxxxTest Data";

		DATA_xfer_flags = 0;
		DATA_buffer = buf;
		DATA_length = 14;

		/* source and destination port, port number in the destination address is meaningless. */
		bcopy(&sport, &buf[0], sizeof(sport));
		bcopy(&dport, &buf[2], sizeof(dport));

		QOS_buffer = NULL;
		QOS_length = 0;
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_3_11_1_conn	test_case_3_11_1
#define test_case_3_11_1_resp	test_case_3_11_1
#define test_case_3_11_1_list	test_case_3_11_1

#define preamble_3_11_1_conn	preamble_1_data_xfer_conn
#define preamble_3_11_1_resp	preamble_1_data_xfer_resp
#define preamble_3_11_1_list	preamble_1_data_xfer_list

#define postamble_3_11_1_conn	postamble_1_data_xfer
#define postamble_3_11_1_resp	postamble_1_idle
#define postamble_3_11_1_list	postamble_1_data_xfer

struct test_stream test_3_11_1_conn = { &preamble_3_11_1_conn, &test_case_3_11_1_conn, &postamble_3_11_1_conn };
struct test_stream test_3_11_1_resp = { &preamble_3_11_1_resp, &test_case_3_11_1_resp, &postamble_3_11_1_resp };
struct test_stream test_3_11_1_list = { &preamble_3_11_1_list, &test_case_3_11_1_list, &postamble_3_11_1_list };



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
	const char *sgrp;		/* test case subgroup */
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
		numb_case_0_1, tgrp_case_0_1, sgrp_case_0_1, name_case_0_1, desc_case_0_1, sref_case_0_1, {
	&test_0_1_conn, &test_0_1_resp, &test_0_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_1, tgrp_case_1_1, sgrp_case_1_1, name_case_1_1, desc_case_1_1, sref_case_1_1, {
	&test_1_1_conn, &test_1_1_resp, &test_1_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_2, tgrp_case_1_2, sgrp_case_1_2, name_case_1_2, desc_case_1_2, sref_case_1_2, {
	&test_1_2_conn, &test_1_2_resp, &test_1_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_1, tgrp_case_1_3_1, sgrp_case_1_3_1, name_case_1_3_1, desc_case_1_3_1, sref_case_1_3_1, {
	&test_1_3_1_conn, &test_1_3_1_resp, &test_1_3_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_2, tgrp_case_1_3_2, sgrp_case_1_3_2, name_case_1_3_2, desc_case_1_3_2, sref_case_1_3_2, {
	&test_1_3_2_conn, &test_1_3_2_resp, &test_1_3_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_3, tgrp_case_1_3_3, sgrp_case_1_3_3, name_case_1_3_3, desc_case_1_3_3, sref_case_1_3_3, {
	&test_1_3_3_conn, &test_1_3_3_resp, &test_1_3_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_4, tgrp_case_1_3_4, sgrp_case_1_3_4, name_case_1_3_4, desc_case_1_3_4, sref_case_1_3_4, {
	&test_1_3_4_conn, &test_1_3_4_resp, &test_1_3_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_5, tgrp_case_1_3_5, sgrp_case_1_3_5, name_case_1_3_5, desc_case_1_3_5, sref_case_1_3_5, {
	&test_1_3_5_conn, &test_1_3_5_resp, &test_1_3_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3_6, tgrp_case_1_3_6, sgrp_case_1_3_6, name_case_1_3_6, desc_case_1_3_6, sref_case_1_3_6, {
	&test_1_3_6_conn, &test_1_3_6_resp, &test_1_3_6_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_4_1, tgrp_case_1_4_1, sgrp_case_1_4_1, name_case_1_4_1, desc_case_1_4_1, sref_case_1_4_1, {
	&test_1_4_1_conn, &test_1_4_1_resp, &test_1_4_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_4_2, tgrp_case_1_4_2, sgrp_case_1_4_2, name_case_1_4_2, desc_case_1_4_2, sref_case_1_4_2, {
	&test_1_4_2_conn, &test_1_4_2_resp, &test_1_4_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_4_3, tgrp_case_1_4_3, sgrp_case_1_4_3, name_case_1_4_3, desc_case_1_4_3, sref_case_1_4_3, {
	&test_1_4_3_conn, &test_1_4_3_resp, &test_1_4_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_4_4, tgrp_case_1_4_4, sgrp_case_1_4_4, name_case_1_4_4, desc_case_1_4_4, sref_case_1_4_4, {
	&test_1_4_4_conn, &test_1_4_4_resp, &test_1_4_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_1_1, tgrp_case_1_5_1_1, sgrp_case_1_5_1_1, name_case_1_5_1_1, desc_case_1_5_1_1, sref_case_1_5_1_1, {
	&test_1_5_1_1_conn, &test_1_5_1_1_resp, &test_1_5_1_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_1_2, tgrp_case_1_5_1_2, sgrp_case_1_5_1_2, name_case_1_5_1_2, desc_case_1_5_1_2, sref_case_1_5_1_2, {
	&test_1_5_1_2_conn, &test_1_5_1_2_resp, &test_1_5_1_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_1_3, tgrp_case_1_5_1_3, sgrp_case_1_5_1_3, name_case_1_5_1_3, desc_case_1_5_1_3, sref_case_1_5_1_3, {
	&test_1_5_1_3_conn, &test_1_5_1_3_resp, &test_1_5_1_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_2_1, tgrp_case_1_5_2_1, sgrp_case_1_5_2_1, name_case_1_5_2_1, desc_case_1_5_2_1, sref_case_1_5_2_1, {
	&test_1_5_2_1_conn, &test_1_5_2_1_resp, &test_1_5_2_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_2_2, tgrp_case_1_5_2_2, sgrp_case_1_5_2_2, name_case_1_5_2_2, desc_case_1_5_2_2, sref_case_1_5_2_2, {
	&test_1_5_2_2_conn, &test_1_5_2_2_resp, &test_1_5_2_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5_2_3, tgrp_case_1_5_2_3, sgrp_case_1_5_2_3, name_case_1_5_2_3, desc_case_1_5_2_3, sref_case_1_5_2_3, {
	&test_1_5_2_3_conn, &test_1_5_2_3_resp, &test_1_5_2_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_1, tgrp_case_1_6_1_1, sgrp_case_1_6_1_1, name_case_1_6_1_1, desc_case_1_6_1_1, sref_case_1_6_1_1, {
	&test_1_6_1_1_conn, &test_1_6_1_1_resp, &test_1_6_1_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_2, tgrp_case_1_6_1_2, sgrp_case_1_6_1_2, name_case_1_6_1_2, desc_case_1_6_1_2, sref_case_1_6_1_2, {
	&test_1_6_1_2_conn, &test_1_6_1_2_resp, &test_1_6_1_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_3, tgrp_case_1_6_1_3, sgrp_case_1_6_1_3, name_case_1_6_1_3, desc_case_1_6_1_3, sref_case_1_6_1_3, {
	&test_1_6_1_3_conn, &test_1_6_1_3_resp, &test_1_6_1_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_4, tgrp_case_1_6_1_4, sgrp_case_1_6_1_4, name_case_1_6_1_4, desc_case_1_6_1_4, sref_case_1_6_1_4, {
	&test_1_6_1_4_conn, &test_1_6_1_4_resp, &test_1_6_1_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_5, tgrp_case_1_6_1_5, sgrp_case_1_6_1_5, name_case_1_6_1_5, desc_case_1_6_1_5, sref_case_1_6_1_5, {
	&test_1_6_1_5_conn, &test_1_6_1_5_resp, &test_1_6_1_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_6, tgrp_case_1_6_1_6, sgrp_case_1_6_1_6, name_case_1_6_1_6, desc_case_1_6_1_6, sref_case_1_6_1_6, {
	&test_1_6_1_6_conn, &test_1_6_1_6_resp, &test_1_6_1_6_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_7, tgrp_case_1_6_1_7, sgrp_case_1_6_1_7, name_case_1_6_1_7, desc_case_1_6_1_7, sref_case_1_6_1_7, {
	&test_1_6_1_7_conn, &test_1_6_1_7_resp, &test_1_6_1_7_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_8, tgrp_case_1_6_1_8, sgrp_case_1_6_1_8, name_case_1_6_1_8, desc_case_1_6_1_8, sref_case_1_6_1_8, {
	&test_1_6_1_8_conn, &test_1_6_1_8_resp, &test_1_6_1_8_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1_9, tgrp_case_1_6_1_9, sgrp_case_1_6_1_9, name_case_1_6_1_9, desc_case_1_6_1_9, sref_case_1_6_1_9, {
	&test_1_6_1_9_conn, &test_1_6_1_9_resp, &test_1_6_1_9_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2_1, tgrp_case_1_6_2_1, sgrp_case_1_6_2_1, name_case_1_6_2_1, desc_case_1_6_2_1, sref_case_1_6_2_1, {
	&test_1_6_2_1_conn, &test_1_6_2_1_resp, &test_1_6_2_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2_2, tgrp_case_1_6_2_2, sgrp_case_1_6_2_2, name_case_1_6_2_2, desc_case_1_6_2_2, sref_case_1_6_2_2, {
	&test_1_6_2_2_conn, &test_1_6_2_2_resp, &test_1_6_2_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2_3, tgrp_case_1_6_2_3, sgrp_case_1_6_2_3, name_case_1_6_2_3, desc_case_1_6_2_3, sref_case_1_6_2_3, {
	&test_1_6_2_3_conn, &test_1_6_2_3_resp, &test_1_6_2_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2_4, tgrp_case_1_6_2_4, sgrp_case_1_6_2_4, name_case_1_6_2_4, desc_case_1_6_2_4, sref_case_1_6_2_4, {
	&test_1_6_2_4_conn, &test_1_6_2_4_resp, &test_1_6_2_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2_5, tgrp_case_1_6_2_5, sgrp_case_1_6_2_5, name_case_1_6_2_5, desc_case_1_6_2_5, sref_case_1_6_2_5, {
	&test_1_6_2_5_conn, &test_1_6_2_5_resp, &test_1_6_2_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_3_1, tgrp_case_1_6_3_1, sgrp_case_1_6_3_1, name_case_1_6_3_1, desc_case_1_6_3_1, sref_case_1_6_3_1, {
	&test_1_6_3_1_conn, &test_1_6_3_1_resp, &test_1_6_3_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_1, tgrp_case_2_1_1, sgrp_case_2_1_1, name_case_2_1_1, desc_case_2_1_1, sref_case_2_1_1, {
	&test_2_1_1_conn, &test_2_1_1_resp, &test_2_1_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_2, tgrp_case_2_1_2, sgrp_case_2_1_2, name_case_2_1_2, desc_case_2_1_2, sref_case_2_1_2, {
	&test_2_1_2_conn, &test_2_1_2_resp, &test_2_1_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_3, tgrp_case_2_1_3, sgrp_case_2_1_3, name_case_2_1_3, desc_case_2_1_3, sref_case_2_1_3, {
	&test_2_1_3_conn, &test_2_1_3_resp, &test_2_1_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_4, tgrp_case_2_1_4, sgrp_case_2_1_4, name_case_2_1_4, desc_case_2_1_4, sref_case_2_1_4, {
	&test_2_1_4_conn, &test_2_1_4_resp, &test_2_1_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_5, tgrp_case_2_1_5, sgrp_case_2_1_5, name_case_2_1_5, desc_case_2_1_5, sref_case_2_1_5, {
	&test_2_1_5_conn, &test_2_1_5_resp, &test_2_1_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_6, tgrp_case_2_1_6, sgrp_case_2_1_6, name_case_2_1_6, desc_case_2_1_6, sref_case_2_1_6, {
	&test_2_1_6_conn, &test_2_1_6_resp, &test_2_1_6_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_1, tgrp_case_2_2_1, sgrp_case_2_2_1, name_case_2_2_1, desc_case_2_2_1, sref_case_2_2_1, {
	&test_2_2_1_conn, &test_2_2_1_resp, &test_2_2_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_2, tgrp_case_2_2_2, sgrp_case_2_2_2, name_case_2_2_2, desc_case_2_2_2, sref_case_2_2_2, {
	&test_2_2_2_conn, &test_2_2_2_resp, &test_2_2_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_3, tgrp_case_2_2_3, sgrp_case_2_2_3, name_case_2_2_3, desc_case_2_2_3, sref_case_2_2_3, {
	&test_2_2_3_conn, &test_2_2_3_resp, &test_2_2_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_1, tgrp_case_2_3_1, sgrp_case_2_3_1, name_case_2_3_1, desc_case_2_3_1, sref_case_2_3_1, {
	&test_2_3_1_conn, &test_2_3_1_resp, &test_2_3_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_2, tgrp_case_2_3_2, sgrp_case_2_3_2, name_case_2_3_2, desc_case_2_3_2, sref_case_2_3_2, {
	&test_2_3_2_conn, &test_2_3_2_resp, &test_2_3_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_1, tgrp_case_2_4_1, sgrp_case_2_4_1, name_case_2_4_1, desc_case_2_4_1, sref_case_2_4_1, {
	&test_2_4_1_conn, &test_2_4_1_resp, &test_2_4_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_2, tgrp_case_2_4_2, sgrp_case_2_4_2, name_case_2_4_2, desc_case_2_4_2, sref_case_2_4_2, {
	&test_2_4_2_conn, &test_2_4_2_resp, &test_2_4_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_3, tgrp_case_2_4_3, sgrp_case_2_4_3, name_case_2_4_3, desc_case_2_4_3, sref_case_2_4_3, {
	&test_2_4_3_conn, &test_2_4_3_resp, &test_2_4_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_4, tgrp_case_2_4_4, sgrp_case_2_4_4, name_case_2_4_4, desc_case_2_4_4, sref_case_2_4_4, {
	&test_2_4_4_conn, &test_2_4_4_resp, &test_2_4_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_5, tgrp_case_2_4_5, sgrp_case_2_4_5, name_case_2_4_5, desc_case_2_4_5, sref_case_2_4_5, {
	&test_2_4_5_conn, &test_2_4_5_resp, &test_2_4_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_5_1, tgrp_case_2_5_1, sgrp_case_2_5_1, name_case_2_5_1, desc_case_2_5_1, sref_case_2_5_1, {
	&test_2_5_1_conn, &test_2_5_1_resp, &test_2_5_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_5_2, tgrp_case_2_5_2, sgrp_case_2_5_2, name_case_2_5_2, desc_case_2_5_2, sref_case_2_5_2, {
	&test_2_5_2_conn, &test_2_5_2_resp, &test_2_5_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_1, tgrp_case_3_1_1, sgrp_case_3_1_1, name_case_3_1_1, desc_case_3_1_1, sref_case_3_1_1, {
	&test_3_1_1_conn, &test_3_1_1_resp, &test_3_1_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_2, tgrp_case_3_1_2, sgrp_case_3_1_2, name_case_3_1_2, desc_case_3_1_2, sref_case_3_1_2, {
	&test_3_1_2_conn, &test_3_1_2_resp, &test_3_1_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_3, tgrp_case_3_1_3, sgrp_case_3_1_3, name_case_3_1_3, desc_case_3_1_3, sref_case_3_1_3, {
	&test_3_1_3_conn, &test_3_1_3_resp, &test_3_1_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_4, tgrp_case_3_1_4, sgrp_case_3_1_4, name_case_3_1_4, desc_case_3_1_4, sref_case_3_1_4, {
	&test_3_1_4_conn, &test_3_1_4_resp, &test_3_1_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_1, tgrp_case_3_2_1, sgrp_case_3_2_1, name_case_3_2_1, desc_case_3_2_1, sref_case_3_2_1, {
	&test_3_2_1_conn, &test_3_2_1_resp, &test_3_2_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_1, tgrp_case_3_3_1, sgrp_case_3_3_1, name_case_3_3_1, desc_case_3_3_1, sref_case_3_3_1, {
	&test_3_3_1_conn, &test_3_3_1_resp, &test_3_3_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_2, tgrp_case_3_3_2, sgrp_case_3_3_2, name_case_3_3_2, desc_case_3_3_2, sref_case_3_3_2, {
	&test_3_3_2_conn, &test_3_3_2_resp, &test_3_3_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_3, tgrp_case_3_3_3, sgrp_case_3_3_3, name_case_3_3_3, desc_case_3_3_3, sref_case_3_3_3, {
	&test_3_3_3_conn, &test_3_3_3_resp, &test_3_3_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_4, tgrp_case_3_3_4, sgrp_case_3_3_4, name_case_3_3_4, desc_case_3_3_4, sref_case_3_3_4, {
	&test_3_3_4_conn, &test_3_3_4_resp, &test_3_3_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_1, tgrp_case_3_4_1, sgrp_case_3_4_1, name_case_3_4_1, desc_case_3_4_1, sref_case_3_4_1, {
	&test_3_4_1_conn, &test_3_4_1_resp, &test_3_4_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_2, tgrp_case_3_4_2, sgrp_case_3_4_2, name_case_3_4_2, desc_case_3_4_2, sref_case_3_4_2, {
	&test_3_4_2_conn, &test_3_4_2_resp, &test_3_4_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_3, tgrp_case_3_4_3, sgrp_case_3_4_3, name_case_3_4_3, desc_case_3_4_3, sref_case_3_4_3, {
	&test_3_4_3_conn, &test_3_4_3_resp, &test_3_4_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_4, tgrp_case_3_4_4, sgrp_case_3_4_4, name_case_3_4_4, desc_case_3_4_4, sref_case_3_4_4, {
	&test_3_4_4_conn, &test_3_4_4_resp, &test_3_4_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_5, tgrp_case_3_4_5, sgrp_case_3_4_5, name_case_3_4_5, desc_case_3_4_5, sref_case_3_4_5, {
	&test_3_4_5_conn, &test_3_4_5_resp, &test_3_4_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_6, tgrp_case_3_4_6, sgrp_case_3_4_6, name_case_3_4_6, desc_case_3_4_6, sref_case_3_4_6, {
	&test_3_4_6_conn, &test_3_4_6_resp, &test_3_4_6_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_7, tgrp_case_3_4_7, sgrp_case_3_4_7, name_case_3_4_7, desc_case_3_4_7, sref_case_3_4_7, {
	&test_3_4_7_conn, &test_3_4_7_resp, &test_3_4_7_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_8, tgrp_case_3_4_8, sgrp_case_3_4_8, name_case_3_4_8, desc_case_3_4_8, sref_case_3_4_8, {
	&test_3_4_8_conn, &test_3_4_8_resp, &test_3_4_8_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_9, tgrp_case_3_4_9, sgrp_case_3_4_9, name_case_3_4_9, desc_case_3_4_9, sref_case_3_4_9, {
	&test_3_4_9_conn, &test_3_4_9_resp, &test_3_4_9_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_10, tgrp_case_3_4_10, sgrp_case_3_4_10, name_case_3_4_10, desc_case_3_4_10, sref_case_3_4_10, {
	&test_3_4_10_conn, &test_3_4_10_resp, &test_3_4_10_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_11, tgrp_case_3_4_11, sgrp_case_3_4_11, name_case_3_4_11, desc_case_3_4_11, sref_case_3_4_11, {
	&test_3_4_11_conn, &test_3_4_11_resp, &test_3_4_11_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_4_12, tgrp_case_3_4_12, sgrp_case_3_4_12, name_case_3_4_12, desc_case_3_4_12, sref_case_3_4_12, {
	&test_3_4_12_conn, &test_3_4_12_resp, &test_3_4_12_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_5_1, tgrp_case_3_5_1, sgrp_case_3_5_1, name_case_3_5_1, desc_case_3_5_1, sref_case_3_5_1, {
	&test_3_5_1_conn, &test_3_5_1_resp, &test_3_5_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_5_2, tgrp_case_3_5_2, sgrp_case_3_5_2, name_case_3_5_2, desc_case_3_5_2, sref_case_3_5_2, {
	&test_3_5_2_conn, &test_3_5_2_resp, &test_3_5_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_5_3, tgrp_case_3_5_3, sgrp_case_3_5_3, name_case_3_5_3, desc_case_3_5_3, sref_case_3_5_3, {
	&test_3_5_3_conn, &test_3_5_3_resp, &test_3_5_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_1, tgrp_case_3_6_1, sgrp_case_3_6_1, name_case_3_6_1, desc_case_3_6_1, sref_case_3_6_1, {
	&test_3_6_1_conn, &test_3_6_1_resp, &test_3_6_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_2, tgrp_case_3_6_2, sgrp_case_3_6_2, name_case_3_6_2, desc_case_3_6_2, sref_case_3_6_2, {
	&test_3_6_2_conn, &test_3_6_2_resp, &test_3_6_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_3, tgrp_case_3_6_3, sgrp_case_3_6_3, name_case_3_6_3, desc_case_3_6_3, sref_case_3_6_3, {
	&test_3_6_3_conn, &test_3_6_3_resp, &test_3_6_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_4, tgrp_case_3_6_4, sgrp_case_3_6_4, name_case_3_6_4, desc_case_3_6_4, sref_case_3_6_4, {
	&test_3_6_4_conn, &test_3_6_4_resp, &test_3_6_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_5, tgrp_case_3_6_5, sgrp_case_3_6_5, name_case_3_6_5, desc_case_3_6_5, sref_case_3_6_5, {
	&test_3_6_5_conn, &test_3_6_5_resp, &test_3_6_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_6_6, tgrp_case_3_6_6, sgrp_case_3_6_6, name_case_3_6_6, desc_case_3_6_6, sref_case_3_6_6, {
	&test_3_6_6_conn, &test_3_6_6_resp, &test_3_6_6_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_7_1, tgrp_case_3_7_1, sgrp_case_3_7_1, name_case_3_7_1, desc_case_3_7_1, sref_case_3_7_1, {
	&test_3_7_1_conn, &test_3_7_1_resp, &test_3_7_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_7_2, tgrp_case_3_7_2, sgrp_case_3_7_2, name_case_3_7_2, desc_case_3_7_2, sref_case_3_7_2, {
	&test_3_7_2_conn, &test_3_7_2_resp, &test_3_7_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_7_3, tgrp_case_3_7_3, sgrp_case_3_7_3, name_case_3_7_3, desc_case_3_7_3, sref_case_3_7_3, {
	&test_3_7_3_conn, &test_3_7_3_resp, &test_3_7_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_7_4, tgrp_case_3_7_4, sgrp_case_3_7_4, name_case_3_7_4, desc_case_3_7_4, sref_case_3_7_4, {
	&test_3_7_4_conn, &test_3_7_4_resp, &test_3_7_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_8_1, tgrp_case_3_8_1, sgrp_case_3_8_1, name_case_3_8_1, desc_case_3_8_1, sref_case_3_8_1, {
	&test_3_8_1_conn, &test_3_8_1_resp, &test_3_8_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_8_2, tgrp_case_3_8_2, sgrp_case_3_8_2, name_case_3_8_2, desc_case_3_8_2, sref_case_3_8_2, {
	&test_3_8_2_conn, &test_3_8_2_resp, &test_3_8_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_8_3, tgrp_case_3_8_3, sgrp_case_3_8_3, name_case_3_8_3, desc_case_3_8_3, sref_case_3_8_3, {
	&test_3_8_3_conn, &test_3_8_3_resp, &test_3_8_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_9_1, tgrp_case_3_9_1, sgrp_case_3_9_1, name_case_3_9_1, desc_case_3_9_1, sref_case_3_9_1, {
	&test_3_9_1_conn, &test_3_9_1_resp, &test_3_9_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_9_2, tgrp_case_3_9_2, sgrp_case_3_9_2, name_case_3_9_2, desc_case_3_9_2, sref_case_3_9_2, {
	&test_3_9_2_conn, &test_3_9_2_resp, &test_3_9_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_9_3, tgrp_case_3_9_3, sgrp_case_3_9_3, name_case_3_9_3, desc_case_3_9_3, sref_case_3_9_3, {
	&test_3_9_3_conn, &test_3_9_3_resp, &test_3_9_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_9_4, tgrp_case_3_9_4, sgrp_case_3_9_4, name_case_3_9_4, desc_case_3_9_4, sref_case_3_9_4, {
	&test_3_9_4_conn, &test_3_9_4_resp, &test_3_9_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_9_5, tgrp_case_3_9_5, sgrp_case_3_9_5, name_case_3_9_5, desc_case_3_9_5, sref_case_3_9_5, {
	&test_3_9_5_conn, &test_3_9_5_resp, &test_3_9_5_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_10_1, tgrp_case_3_10_1, sgrp_case_3_10_1, name_case_3_10_1, desc_case_3_10_1, sref_case_3_10_1, {
	&test_3_10_1_conn, &test_3_10_1_resp, &test_3_10_1_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_10_2, tgrp_case_3_10_2, sgrp_case_3_10_2, name_case_3_10_2, desc_case_3_10_2, sref_case_3_10_2, {
	&test_3_10_2_conn, &test_3_10_2_resp, &test_3_10_2_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_10_3, tgrp_case_3_10_3, sgrp_case_3_10_3, name_case_3_10_3, desc_case_3_10_3, sref_case_3_10_3, {
	&test_3_10_3_conn, &test_3_10_3_resp, &test_3_10_3_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_10_4, tgrp_case_3_10_4, sgrp_case_3_10_4, name_case_3_10_4, desc_case_3_10_4, sref_case_3_10_4, {
	&test_3_10_4_conn, &test_3_10_4_resp, &test_3_10_4_list}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_11_1, tgrp_case_3_11_1, sgrp_case_3_11_1, name_case_3_11_1, desc_case_3_11_1, sref_case_3_11_1, {
	&test_3_11_1_conn, &test_3_11_1_resp, &test_3_11_1_list}, &begin_tests, &end_tests, 0, 0}, {
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
				if (verbose > 1)
					fprintf(stdout, "\nTest Subgroup: %s", tests[i].sgrp);
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
						if (verbose > 2)
							fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
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
					if (verbose > 2)
						fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
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
