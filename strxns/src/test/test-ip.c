/*****************************************************************************

 @(#) $RCSfile: test-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/05/06 10:22:35 $

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

 Last Modified $Date: 2006/05/06 10:22:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-ip.c,v $
 Revision 0.9.2.1  2006/05/06 10:22:35  brian
 - added test suite for NPI-IP driver

 *****************************************************************************/

#ident "@(#) $RCSfile: test-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/05/06 10:22:35 $"

static char const ident[] = "$RCSfile: test-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/05/06 10:22:35 $";

/*
 *  Simple test program for NPI-IP streams.
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/npi.h>
#include <sys/npi_ip.h>

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

#define SHORT_WAIT 10
#define NORMAL_WAIT 100
#define LONG_WAIT 500
#define LONGER_WAIT 5000

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };

union {
	np_ulong prim;
	union N_primitives npi;
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
const char *device = "/dev/ip";
int show = 1;
struct sockaddr_in addr1[3], addr2[3], addr3[3], addr4[3];
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

long test_start = 0;

static int state;

#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

static int
time_event(int event)
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
		fprintf(stdout, "                    | %11.6g                    |  |                   <%d>\n", t, state);
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
 *  Options
 *  
 *  -------------------------------------------------------------------------
 */

N_qos_sel_ud_ip_t qos_data = {
	.n_qos_type = N_QOS_SEL_UD_IP,
	.protocol = 132,
	.priority = 0,
	.ttl = 64,
	.tos = N_ROUTINE|N_NOTOS,
	.saddr = 0,
};

N_qos_sel_conn_ip_t qos_conn = {
	.n_qos_type = N_QOS_SEL_CONN_IP,
	.protocol = 132,
	.priority = 0,
	.ttl = 64,
	.tos = N_ROUTINE|N_NOTOS,
	.mtu = 536,
	.saddr = 0,
	.daddr = 0,
};

N_qos_sel_info_ip_t qos_info = {
	.n_qos_type = N_QOS_SEL_INFO_IP,
	.protocol = 132,
	.priority = 0,
	.ttl = 64,
	.tos = N_ROUTINE|N_NOTOS,
	.mtu = 536,
	.saddr = 0,
	.daddr = 0,
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

		snprintf(buf, sizeof(buf), "[%ld]", err);
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

		snprintf(buf, sizeof(buf), "[%lu]", nerr);
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
	case __EVENT_CONN_REQ:
		return ("N_CONN_REQ");
	case __EVENT_CONN_RES:
		return ("N_CONN_RES");
	case __EVENT_DISCON_REQ:
		return ("N_DISCON_REQ");
	case __EVENT_DATA_REQ:
		return ("N_DATA_REQ");
	case __EVENT_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case __EVENT_INFO_REQ:
		return ("N_INFO_REQ");
	case __EVENT_BIND_REQ:
		return ("N_BIND_REQ");
	case __EVENT_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case __EVENT_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case __EVENT_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case __EVENT_CONN_IND:
		return ("N_CONN_IND");
	case __EVENT_CONN_CON:
		return ("N_CONN_CON");
	case __EVENT_DISCON_IND:
		return ("N_DISCON_IND");
	case __EVENT_DATA_IND:
		return ("N_DATA_IND");
	case __EVENT_EXDATA_IND:
		return ("N_EXDATA_IND");
	case __EVENT_INFO_ACK:
		return ("N_INFO_ACK");
	case __EVENT_BIND_ACK:
		return ("N_BIND_ACK");
	case __EVENT_ERROR_ACK:
		return ("N_ERROR_ACK");
	case __EVENT_OK_ACK:
		return ("N_OK_ACK");
	case __EVENT_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case __EVENT_UDERROR_IND:
		return ("N_UDERROR_IND");
	case __EVENT_DATACK_REQ:
		return ("N_DATACK_REQ");
	case __EVENT_DATACK_IND:
		return ("N_DATACK_IND");
	case __EVENT_RESET_REQ:
		return ("N_RESET_REQ");
	case __EVENT_RESET_IND:
		return ("N_RESET_IND");
	case __EVENT_RESET_RES:
		return ("N_RESET_RES");
	case __EVENT_RESET_CON:
		return ("N_RESET_CON");
	default:
		return ("(unexpected");
	}
}

const char *
state_string(ulong state)
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

			fprintf(stdout, "%s%d.%d.%d.%d.", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
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

			snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d.", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
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
print_qos(char *qos_ptr, size_t qos_len)
{
	union N_qos_ip_types *qos = (union N_qos_ip_types *) qos_ptr;

	if (qos_len) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_CONN_IP:
			printf("CONN:");
			printf(" protocol=%ld,", (long) qos->n_qos_sel_conn.protocol);
			printf(" priority=%ld,", (long) qos->n_qos_sel_conn.priority);
			printf(" ttl=%ld,", (long) qos->n_qos_sel_conn.ttl);
			printf(" tos=%ld,", (long) qos->n_qos_sel_conn.tos);
			printf(" mtu=%ld,", (long) qos->n_qos_sel_conn.mtu);
			printf(" saddr=%ld,", (long) qos->n_qos_sel_conn.saddr);
			printf(" daddr=%ld,", (long) qos->n_qos_sel_conn.daddr);
			break;

		case N_QOS_SEL_UD_IP:
			printf("DATA: ");
			printf(" protocol=%ld,", (long) qos->n_qos_sel_conn.protocol);
			printf(" priority=%ld,", (long) qos->n_qos_sel_conn.priority);
			printf(" ttl=%ld,", (long) qos->n_qos_sel_conn.ttl);
			printf(" tos=%ld,", (long) qos->n_qos_sel_conn.tos);
			printf(" saddr=%ld,", (long) qos->n_qos_sel_conn.saddr);
			break;

		case N_QOS_SEL_INFO_IP:
			printf("INFO: ");
			printf(" protocol=%ld,", (long) qos->n_qos_sel_conn.protocol);
			printf(" priority=%ld,", (long) qos->n_qos_sel_conn.priority);
			printf(" ttl=%ld,", (long) qos->n_qos_sel_conn.ttl);
			printf(" tos=%ld,", (long) qos->n_qos_sel_conn.tos);
			printf(" mtu=%ld,", (long) qos->n_qos_sel_conn.mtu);
			printf(" saddr=%ld,", (long) qos->n_qos_sel_conn.saddr);
			printf(" daddr=%ld,", (long) qos->n_qos_sel_conn.daddr);
			break;

		case N_QOS_RANGE_INFO_IP:
			printf("RANGE: ");
			break;

		default:
			printf("(unknown qos structure %lu)\n", (ulong) qos->n_qos_type);
			break;
		}
	} else
		printf("(no qos)");
	printf("\n");
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

void
print_event_conn(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "N_INFO_REQ    ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "N_INFO_ACK    <----/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "N_OPTMGMT_REQ ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "N_BIND_REQ    ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "N_BIND_ACK    <----/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "N_UNBIND_REQ  ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "N_ERROR_ACK   <----/|                               |  |                    [%d]\n", state);
		fprintf(stdout, "%-15s     |                               |  |                    \n", nerrno_string(cmd.npi.error_ack.NPI_error, cmd.npi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "N_OK_ACK      <----/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "N_UNITDATA_REQ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "N_UNITDATA_IND<-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "N_UDERROR_IND <----/|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "N_CONN_REQ    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "N_CONN_IND    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		seq[fd] = cmd.npi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "N_CONN_RES    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "N_CONN_CON    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.npi.data_req.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "N_DATA_REQ+   ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "N_DATA_REQ    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.npi.data_ind.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "N_DATA_IND+   <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "N_DATA_IND    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		fprintf(stdout, "N_EXDATA_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		fprintf(stdout, "N_EXDATA_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "N_DISCON_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "N_DISCON_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_DATACK_REQ:
		fprintf(stdout, "N_DATACK_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_DATACK_IND:
		fprintf(stdout, "N_DATACK_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_RESET_REQ:
		fprintf(stdout, "N_RESET_REQ   ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_RESET_IND:
		fprintf(stdout, "N_RESET_IND   <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_RESET_RES:
		fprintf(stdout, "N_RESET_RES   ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_RESET_CON:
		fprintf(stdout, "N_RESET_CON   <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "????%4ld????  ?----?|?- - - - - - -?                |  |                    [%d]\n", (long) cmd.npi.type, state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void
print_event_list(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "                    |                               |<-+---- N_INFO_REQ     [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "                    |                               |\\-+---> N_INFO_ACK     [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "                    |                               |<-+---- N_OPTMGMT_REQ  [%d]\n", state);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "                    |                               |<-+---- N_BIND_REQ     [%d]\n", state);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "                    |                               |\\-+---> N_BIND_ACK     [%d]\n", state);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "                    |                               |<-+---- N_UNBIND_REQ   [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "                    |                               |\\-+---> N_ERROR_ACK    [%d]\n", state);
		fprintf(stdout, "                    |                               |   .    %-15s\n", nerrno_string(cmd.npi.error_ack.NPI_error, cmd.npi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "                    |                               |\\-+---> N_OK_ACK       [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_UNITDATA_REQ [%d]\n", state);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_UNITDATA_IND [%d]\n", state);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|\\-+---> N_UDERROR_IND  [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_CONN_REQ     [%d]\n", state);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_CONN_IND     [%d]\n", state);
		seq[fd] = cmd.npi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_CONN_RES     [%d]\n", state);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_CONN_CON     [%d]\n", state);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.npi.data_req.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_DATA_REQ+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_DATA_REQ     [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.npi.data_ind.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_DATA_IND+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_DATA_IND     [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_EXDATA_REQ   [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_EXDATA_IND   [%d]\n", state);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_DISCON_REQ   [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_DISCON_IND   [%d]\n", state);
		break;
	case __EVENT_DATACK_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_DATACK_REQ   [%d]\n", state);
		break;
	case __EVENT_DATACK_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_DATACK_IND   [%d]\n", state);
		break;
	case __EVENT_RESET_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_RESET_REQ    [%d]\n", state);
		break;
	case __EVENT_RESET_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_RESET_IND    [%d]\n", state);
		break;
	case __EVENT_RESET_RES:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- N_RESET_RES    [%d]\n", state);
		break;
	case __EVENT_RESET_CON:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> N_RESET_CON    [%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "                    |                               |?-+---? ????%4ld????   [%d]\n", (long) cmd.npi.type, state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void
print_event_resp(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "                    |                               |  |<--- N_INFO_REQ     [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "                    |                               |  |\\--> N_INFO_ACK---> [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "                    |                               |  |<--- N_OPTMGMT_REQ  [%d]\n", state);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "                    |                               |  |<--- N_BIND_REQ     [%d]\n", state);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "                    |                               |  |\\--> N_BIND_ACK     [%d]\n", state);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "                    |                               |  |<--- N_UNBIND_REQ   [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "                    |                               |  |\\--> N_ERROR_ACK    [%d]\n", state);
		fprintf(stdout, "                    |                               |  |     %-15s\n", nerrno_string(cmd.npi.error_ack.NPI_error, cmd.npi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "                    |                               |  |\\--> N_OK_ACK       [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_UNITDATA_REQ [%d]\n", state);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_UNITDATA_IND [%d]\n", state);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|\\--> N_UDERROR_IND  [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_CONN_REQ     [%d]\n", state);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_CONN_IND     [%d]\n", state);
		seq[fd] = cmd.npi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_CONN_RES     [%d]\n", state);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_CONN_CON     [%d]\n", state);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.npi.data_req.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_DATA_REQ+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_DATA_REQ     [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.npi.data_ind.DATA_xfer_flags & N_MORE_DATA_FLAG)
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_DATA_IND+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_DATA_IND     [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_EXDATA_REQ   [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_EXDATA_IND   [%d]\n", state);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_DISCON_REQ   [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_DISCON_IND   [%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "                    |                               |  |?--? ????%4ld????   [%d]\n", (long) cmd.npi.type, state);
		break;
	case __EVENT_DATACK_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_DATACK_REQ   [%d]\n", state);
		break;
	case __EVENT_DATACK_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_DATACK_IND   [%d]\n", state);
		break;
	case __EVENT_RESET_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_RESET_REQ    [%d]\n", state);
		break;
	case __EVENT_RESET_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_RESET_IND    [%d]\n", state);
		break;
	case __EVENT_RESET_RES:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- N_RESET_RES    [%d]\n", state);
		break;
	case __EVENT_RESET_CON:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_RESET_CON    [%d]\n", state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void
print_event(int fd, int event)
{
	if (verbose < 1 || !show)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	if (fd == conn_fd)
		print_event_conn(fd, event);
	else if (fd == list_fd)
		print_event_list(fd, event);
	else if (fd == resp_fd)
		print_event_resp(fd, event);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return;
}

void
print_less(int fd)
{
	if (verbose < 1 || !show)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
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
print_open(int *fdp)
{
	if (verbose > 3) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fdp == &conn_fd)
			fprintf(stdout, "open()        ----->v                                                       \n");
		else if (fdp == &resp_fd)
			fprintf(stdout, "                    |                                  v<--- open()         \n");
		else if (fdp == &list_fd)
			fprintf(stdout, "                    |                               v<-|---- open()         \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_close(int *fdp)
{
	if (verbose > 3) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fdp == &conn_fd)
			fprintf(stdout, "close()       ----->X                               |  |                    \n");
		else if (fdp == &resp_fd)
			fprintf(stdout, "                                                    |  X<--- close()        \n");
		else if (fdp == &list_fd)
			fprintf(stdout, "                                                    X<------ close()        \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_command(int fd, const char *command)
{
	if (verbose > 3) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "%-14s----->|                               |  |                    [%d]\n", command, state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |<--- %-14s [%d]\n", command, state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |<-+---- %-14s [%d]\n", command, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_errno(int fd, long error)
{
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "%-14s<----/|                               |  |                    [%d]\n", errno_string(error), state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |\\--> %-14s [%d]\n", errno_string(error), state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |\\-+---> %-14s [%d]\n", errno_string(error), state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_success(int fd)
{
	if (verbose > 4) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "ok            <----/|                               |  |                    [%d]\n", state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |\\--> ok             [%d]\n", state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |\\-+---> ok             [%d]\n", state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

static int
decode_data(int fd)
{
	int event = __RESULT_DECODE_ERROR;

	if (data.len >= 0)
		event = __EVENT_DATA;
	if (verbose > 0 && show)
		print_event(fd, event);
	return ((last_event = event));
}

static int
decode_ctrl(int fd)
{
	int event = __RESULT_DECODE_ERROR;

	if (ctrl.len >= sizeof(cmd.npi.type))
		switch ((last_prim = cmd.npi.type)) {
		case N_CONN_REQ:
			event = __EVENT_CONN_REQ;
			break;
		case N_CONN_RES:
			event = __EVENT_CONN_RES;
			break;
		case N_DISCON_REQ:
			event = __EVENT_DISCON_REQ;
			break;
		case N_DATA_REQ:
			event = __EVENT_DATA_REQ;
			break;
		case N_EXDATA_REQ:
			event = __EVENT_EXDATA_REQ;
			break;
		case N_INFO_REQ:
			event = __EVENT_INFO_REQ;
			break;
		case N_BIND_REQ:
			event = __EVENT_BIND_REQ;
			break;
		case N_UNBIND_REQ:
			event = __EVENT_UNBIND_REQ;
			break;
		case N_UNITDATA_REQ:
			event = __EVENT_UNITDATA_REQ;
			break;
		case N_OPTMGMT_REQ:
			event = __EVENT_OPTMGMT_REQ;
			break;
		case N_DATACK_REQ:
			event = __EVENT_DATACK_REQ;
			break;
		case N_RESET_REQ:
			event = __EVENT_RESET_REQ;
			break;
		case N_RESET_RES:
			event = __EVENT_RESET_RES;
			break;
		case N_CONN_IND:
			event = __EVENT_CONN_IND;
			break;
		case N_CONN_CON:
			event = __EVENT_CONN_CON;
			break;
		case N_DISCON_IND:
			event = __EVENT_DISCON_IND;
			break;
		case N_DATA_IND:
			event = __EVENT_DATA_IND;
			break;
		case N_EXDATA_IND:
			event = __EVENT_EXDATA_IND;
			break;
		case N_INFO_ACK:
			event = __EVENT_INFO_ACK;
			break;
		case N_BIND_ACK:
			event = __EVENT_BIND_ACK;
			break;
		case N_ERROR_ACK:
			event = __EVENT_ERROR_ACK;
			break;
		case N_OK_ACK:
			event = __EVENT_OK_ACK;
			break;
		case N_UNITDATA_IND:
			event = __EVENT_UNITDATA_IND;
			break;
		case N_UDERROR_IND:
			event = __EVENT_UDERROR_IND;
			break;
		case N_DATACK_IND:
			event = __EVENT_DATACK_IND;
			break;
		case N_RESET_IND:
			event = __EVENT_RESET_IND;
			break;
		case N_RESET_CON:
			event = __EVENT_RESET_CON;
			break;
		default:
			event = __EVENT_UNKNOWN;
			break;
		}
	if (verbose > 0 && show)
		print_event(fd, event);
	return ((last_event = event));
}

static int
decode_msg(int fd)
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

int
wait_event(int fd, int wait)
{
	for (;;) {
		struct pollfd pfd[] = { {fd, POLLIN | POLLPRI, 0} };

		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				if (fd == conn_fd)
					fprintf(stdout, "++++++++++++++++++++|+++++++++++ TIMEOUT +++++++++++|  |                    [%d]\n", state);
				else if (fd == resp_fd)
					fprintf(stdout, "                    |+++++++++++ TIMEOUT +++++++++++|  |++++++++++++++++++++[%d]\n", state);
				else if (fd == list_fd)
					fprintf(stdout, "                    |+++++++++++ TIMEOUT +++++++++++|++|++++++++++++++++++++[%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
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

int
get_event(int fd)
{
	return wait_event(fd, -1);
}

void
test_sleep(int fd, unsigned long t)
{
	if (verbose > 1 && show) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "/ / / / / / / / / / | / / / Waiting %03lu seconds / / |  |                    [%d]\n", t, state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    | / / / Waiting %03lu seconds / / |  | / / / / / / / / / /[%d]\n", t, state);
		else if (fd == list_fd)
			fprintf(stdout, "                    | / / / Waiting %03lu seconds / / |/ | / / / / / / / / / /[%d]\n", t, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	sleep(t);
}

int
expect(int fd, int wait, int want)
{
	if ((last_event = wait_event(fd, wait)) == want)
		return (__RESULT_SUCCESS);
	if (verbose > 1 && show) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "- (%-15s) | - - - - - [Expected]- - - - - |  |                    [%d]\n", event_string(want), state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    | - - - - - [Expected]- - - - - |  | (%-15s) -[%d]\n", event_string(want), state);
		else if (fd == list_fd)
			fprintf(stdout, "                    | - - - - - [Expected]- - - - - |- | (%-15s) -[%d]\n", event_string(want), state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (__RESULT_FAILURE);
}

int
put_msg(int fd, int band, int flags, int wait)
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

int
put_fdi(int fd, int resfd, int offset, int flags)
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
		print_errno(fd, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
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
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return __RESULT_FAILURE;
	}
}

int
ip_info_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.info_req);
	cmd.prim = N_INFO_REQ;
	return put_msg(fd, 0, MSG_HIPRI, 0);
}

int
ip_optmgmt_req(int fd, ulong flags)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.optmgmt_req) + sizeof(qos_info);
	cmd.prim = N_OPTMGMT_REQ;
	cmd.npi.optmgmt_req.OPTMGMT_flags = flags;
	cmd.npi.optmgmt_req.QOS_length = sizeof(qos_info);
	cmd.npi.optmgmt_req.QOS_offset = sizeof(cmd.npi.optmgmt_req);
	bcopy(&qos_info, (cmd.cbuf + sizeof(cmd.npi.optmgmt_req)), sizeof(qos_info));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
ip_bind_req(int fd, void *add_ptr, size_t add_len, int coninds)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.bind_req) + add_len;
	cmd.prim = N_BIND_REQ;
	cmd.npi.bind_req.ADDR_length = add_len;
	cmd.npi.bind_req.ADDR_offset = sizeof(cmd.npi.bind_req);
	cmd.npi.bind_req.CONIND_number = coninds;
	cmd.npi.bind_req.BIND_flags = TOKEN_REQUEST;
	cmd.npi.bind_req.PROTOID_length = 0;
	cmd.npi.bind_req.PROTOID_offset = 0;
	bcopy(add_ptr, (&cmd.npi.bind_req) + 1, add_len);
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
ip_unbind_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.npi.unbind_req);
	cmd.prim = N_UNBIND_REQ;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
ip_conn_req(int fd, void *add_ptr, size_t add_len, const char *dat)
{
	int ret;

	if (!dat)
		data.len = 0;
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.conn_req) + add_len + sizeof(qos_conn);
	cmd.prim = N_CONN_REQ;
	cmd.npi.conn_req.DEST_length = add_len;
	cmd.npi.conn_req.DEST_offset = sizeof(cmd.npi.conn_req);
	cmd.npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_req.QOS_length = sizeof(qos_conn);
	cmd.npi.conn_req.QOS_offset = sizeof(cmd.npi.conn_req) + add_len;
	bcopy(add_ptr, (cmd.cbuf + sizeof(cmd.npi.conn_req)), add_len);
	bcopy(&qos_conn, (cmd.cbuf + sizeof(cmd.npi.conn_req) + add_len), sizeof(qos_conn));
	ret = put_msg(fd, 0, MSG_BAND, 0);
	data.maxlen = BUFSIZE;
	data.len = -1;
	data.buf = dbuf;
	return (ret);
}

int
ip_conn_res(int fd, int fd2, const char *dat)
{
	int ret;

	if (!dat)
		data.len = 0;
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.conn_res);
	cmd.prim = N_CONN_RES;
	cmd.npi.conn_res.TOKEN_value = tok[fd2];
	cmd.npi.conn_res.RES_offset = 0;
	cmd.npi.conn_res.RES_length = 0;
	cmd.npi.conn_res.SEQ_number = seq[fd];
	cmd.npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_res.QOS_offset = 0;
	cmd.npi.conn_res.QOS_length = 0;
	ret = put_msg(fd, 0, MSG_BAND, 0);
	data.maxlen = BUFSIZE;
	data.len = -1;
	data.buf = dbuf;
	return (ret);
}

int
ip_discon_req(int fd, ulong seq)
{
	ctrl.len = sizeof(cmd.npi.discon_req);
	cmd.prim = N_DISCON_REQ;
	cmd.npi.discon_req.DISCON_reason = 0;
	cmd.npi.discon_req.RES_length = 0;
	cmd.npi.discon_req.RES_offset = 0;
	cmd.npi.discon_req.SEQ_number = seq;
	data.len = -1;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int
ip_data_req(int fd, ulong flags, const char *dat, int wait)
{
	int ret;

	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(qos_data);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = flags;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = -1;
	data.buf = dbuf;
	return (ret);
}

int
ip_ndata_req(int fd, ulong flags, const char *dat, size_t len, int wait)
{
	int ret;

	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.maxlen = len;
		data.len = len;
		data.buf = (char *) dat;
	}
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(qos_data);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = flags;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int
ip_exdata_req(int fd, const char *dat)
{
	int ret;

	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.npi.exdata_req) + sizeof(qos_data);
	cmd.prim = N_EXDATA_REQ;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.exdata_req), sizeof(qos_data));
	ret = put_msg(fd, 1, MSG_BAND, 0);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int
ip_datack_req(int fd)
{
	int ret;

	data.len = -1;
	ctrl.len = sizeof(cmd.npi.datack_req) + sizeof(qos_data);
	cmd.prim = N_DATACK_REQ;
	bcopy(&qos_data, cmd.cbuf + sizeof(cmd.npi.datack_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, 0);
	return (ret);
}

/* 
 *  -------------------------------------------------------------------------
 *  -------------------------------------------------------------------------
 */

int
begin_tests(void)
{
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
	if (ip_n_open(device, &conn_fd) != __RESULT_SUCCESS)
		goto failure;
	if (ip_n_open(device, &resp_fd) != __RESULT_SUCCESS)
		goto failure;
	if (ip_n_open(device, &list_fd) != __RESULT_SUCCESS)
		goto failure;
	show = 0;
	if (ip_bind_req(resp_fd, &addr3, sizeof(addr3), 0) != __RESULT_SUCCESS)
		goto failure;
	if (expect(resp_fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	tok[resp_fd] = cmd.npi.bind_ack.TOKEN_value;
	if (ip_unbind_req(resp_fd) != __RESULT_SUCCESS)
		goto failure;
	if (expect(resp_fd, NORMAL_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	show = 1;
	return (__RESULT_SUCCESS);
      failure:
	show = 1;
	return __RESULT_FAILURE;
}

int
end_tests(void)
{
	show_acks = 0;
	if (ip_close(&conn_fd) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (ip_close(&resp_fd) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (ip_close(&list_fd) != __RESULT_SUCCESS)
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

int
preamble_0(int fd)
{
	return (__RESULT_SUCCESS);
}

int
postamble_0(int fd)
{
	return (__RESULT_SUCCESS);
}

int
preamble_1(int fd, void *add_ptr, size_t add_len, int coninds)
{
	if (ip_optmgmt_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ip_bind_req(fd, add_ptr, add_len, coninds) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_1_conn(int fd)
{
	return preamble_1(fd, &addr1, sizeof(addr1), 0);
}

int
preamble_1_list(int fd)
{
	return preamble_1(fd, &addr2, sizeof(addr2), 5);
}

int
preamble_1_resp(int fd)
{
	return preamble_1(fd, &addr2, sizeof(addr3), 0);
}

int
postamble_1(int fd)
{
	if (ip_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_1s_conn(int fd)
{
	if (preamble_1_conn(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(fd, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_1s_list(int fd)
{
	if (preamble_1_list(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(fd, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
preamble_1s_resp(int fd)
{
	if (preamble_1_resp(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(fd, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
postamble_1e(int fd)
{
	if (ip_unbind_req(fd) == __RESULT_SUCCESS || last_errno != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	expect(fd, NORMAL_WAIT, __EVENT_OK_ACK);
	return (__RESULT_FAILURE);
}

/* 
 *  Do options management.
 */
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Do options management."
#define desc_case_1_1 "\
Checks that options management can be performed on several streams\n\
and that one stream can be bound and unbound."
int
test_case_1_1(int fd, void *add_ptr, size_t add_len, int coninds)
{
	if (ip_optmgmt_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ip_info_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (add_len) {
		if (ip_bind_req(fd, add_ptr, add_len, coninds) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (ip_optmgmt_req(fd, 0) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(fd, NORMAL_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (ip_info_req(fd) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(fd, NORMAL_WAIT, __EVENT_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (ip_unbind_req(fd) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(fd, NORMAL_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_1_1_conn(int fd)
{
	return test_case_1_1(fd, &addr1, sizeof(addr1), 0);
}

int
test_case_1_1_list(int fd)
{
	return test_case_1_1(fd, NULL, 0, 0);
}

int
test_case_1_1_resp(int fd)
{
	return test_case_1_1(fd, NULL, 0, 0);
}

#define preamble_1_1_conn preamble_0
#define preamble_1_1_resp preamble_0
#define preamble_1_1_list preamble_0

#define postamble_1_1_conn postamble_0
#define postamble_1_1_resp postamble_0
#define postamble_1_1_list postamble_0

/* 
 *  Bind and unbind three streams.
 */
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Bind and unbind three streams."
#define desc_case_1_2 "\
Checks that three streams can be bound and unbound with\n\
one stream as listener."
int
test_case_1_2(int fd, void *add_ptr, size_t add_len, int coninds)
{
	if (ip_bind_req(fd, add_ptr, add_len, coninds) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (ip_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_1_2_conn(int fd)
{
	return test_case_1_2(fd, &addr1, sizeof(addr1), 0);
}

int
test_case_1_2_list(int fd)
{
	return test_case_1_2(fd, &addr2, sizeof(addr2), 5);
}

int
test_case_1_2_resp(int fd)
{
	return test_case_1_2(fd, &addr2, sizeof(addr3), 0);
}

#define preamble_1_2_conn preamble_0
#define preamble_1_2_resp preamble_0
#define preamble_1_2_list preamble_0

#define postamble_1_2_conn postamble_0
#define postamble_1_2_resp postamble_0
#define postamble_1_2_list postamble_0


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
conn_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;

	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "--------------------+------------Preamble-----------+--+                    \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(conn_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------+--------------Test-------------+--+                    \n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(conn_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "####################|########## SCRIPT ERROR #######|##|                    [%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "********************|************ PASSED ***********|**|                    [%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------+------------Postamble----------+--+                    \n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(conn_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "--------------------+-------------------------------+--+                    \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

int
resp_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;

	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +------------Preamble-----------+  +--------------------\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(resp_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +--------------Test-------------+  +--------------------\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(resp_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|  |XXXXXXXXXXXXXXXXXXX(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |######### SCRIPT ERROR ########|  |###################(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |************ PASSED ***********|  |********************(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +------------Postamble----------+  +--------------------\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(resp_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +-------------------------------+  +--------------------\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

int
list_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;

	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +------------Preamble-----------+--+--------------------\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(list_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +--------------Test-------------+--+--------------------\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(list_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |######### SCRIPT ERROR ########|##|###################(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |************ PASSED ***********|**|********************(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +------------Postamble----------+--+--------------------\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(list_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +-------------------------------+--+--------------------\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

/* 
 *  Fork multiple children to do the actual testing.  The conn child is the
 *  connecting process, the resp child is the responding process.
 */

int
test_run(struct test_side *conn_side, struct test_side *resp_side, struct test_side *list_side)
{
	int children = 0;
	pid_t got_chld, conn_chld = 0, resp_chld = 0, list_chld = 0;
	int got_stat, conn_stat = __RESULT_SUCCESS, resp_stat = __RESULT_SUCCESS, list_stat = __RESULT_SUCCESS;

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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |@@@@@@@@@@ TERMINATED @@@@@@@@@|  |@@@@@@@@@@@@@@@@@@@@{%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@@{%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |&&&&&&&&&&&&&&&&&&&&{%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&&&{%d}\n", signal);
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++{%d}\n", state);
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
	NULL,}
};

int
do_tests(void)
{
	int i;
	int result = __RESULT_INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;

	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\n\nRFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Program.\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "\nTest Case SCTP-NPI/%s: %s\n", tests[i].numb, tests[i].name);
				if (verbose > 1)
					fprintf(stdout, "%s\n", tests[i].desc);
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
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
					dummy = lockf(fileno(stdout), F_LOCK, 0);
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
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
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
					fprintf(stdout, "?????????\n");
					fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
					fprintf(stdout, "?????????\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			}
			tests[i].result = result;
		}
		if (summary && verbose) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case SCTP-NPI/%-10s ", tests[i].numb);
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
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "Done.\n\n");
			fprintf(stdout, "========= %2d successes   \n", successes);
			fprintf(stdout, "========= %2d failures    \n", failures);
			fprintf(stdout, "========= %2d inconclusive\n", inconclusive);
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (0);
	} else {
		end_tests();
		show = 1;
		if (verbose > 0) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "Test setup failed!\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (2);
	}
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
NPI-IP - OpenSS7 STREAMS NPI IP - Conformance Test Suite\n\
\n\
Copyright (c) 2001-2006 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Regulations (\"FAR\") (or any successor regulations) or, in the  cases of NASA, in\n\
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
    Copyright (c) 2001-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    %1$s {-C, --copying}\n\
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
    %1$s {-C, --copying}\n\
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
        Prints this usage message and exits\n\
    -V, --version\n\
        Prints the version and exits\n\
    -C, --copying\n\
        Prints copyright and permission and exits\n\
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
			{NULL, }
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
						fprintf(stdout, "Test Case SCTP-NPI/%s: %s\n", t->numb, t->name);
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
					fprintf(stdout, "Test Case SCTP-NPI/%s: %s\n", t->numb, t->name);
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
			splash(argc, argv);
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
	splash(argc, argv);
	do_tests();
	exit(0);
}
