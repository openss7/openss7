/*****************************************************************************

 @(#) $RCSfile: test-sigtran.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 09:33:28 $

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

 Last Modified $Date: 2007/03/12 09:33:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sigtran.c,v $
 Revision 0.9.2.6  2007/03/12 09:33:28  brian
 - boosted default test port numbers from 10000 to 18000

 Revision 0.9.2.5  2007/03/08 08:26:27  brian
 - print primitives at default verbosity

 Revision 0.9.2.4  2007/01/23 10:00:50  brian
 - added test program and m2ua-as updates

 Revision 0.9.2.3  2006/12/18 08:51:41  brian
 - corections from testing, resolve device numbering

 Revision 0.9.2.2  2006/10/27 22:50:43  brian
 - working up modules and testsuite

 Revision 0.9.2.1  2006/10/16 10:42:51  brian
 - added new package files

 Revision 0.9.2.1  2006/10/13 07:57:47  brian
 - added initial STREAMS Channel package

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sigtran.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 09:33:28 $"

static char const ident[] = "$RCSfile: test-sigtran.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/12 09:33:28 $";

#define TEST_M2PA   0
#define TEST_X400   0
#define TEST_M2UA   0

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

#if 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#if 0
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#endif
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#if TEST_M2PA || TEST_M2UA
#include <sys/npi.h>
#include <sys/npi_sctp.h>
#endif				/* TEST_M2PA || TEST_M2UA */
#if TEST_M2UA
#include <xti.h>
#include <tihdr.h>
#include <timod.h>
#include <xti_inet.h>
#include <sys/xti_sctp.h>
#endif				/* TEST_M2UA */

#if __BYTE_ORDER == __BIG_ENDIAN
#   define __constant_ntohl(x)	(x)
#   define __constant_ntohs(x)	(x)
#   define __constant_htonl(x)	(x)
#   define __constant_htons(x)	(x)
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
#   define __constant_ntohl(x)	__bswap_constant_32(x)
#   define __constant_ntohs(x)	__bswap_constant_16(x)
#   define __constant_htonl(x)	__bswap_constant_32(x)
#   define __constant_htons(x)	__bswap_constant_16(x)
#endif
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "SIGnalling TRANsport";

/* static const char *spkgname = "SIGTRAN"; */
static const char *lstdname = "RFC 2960/ETSI TS 102 144 V.1.1.1 (2003-05)";
static const char *sstdname = "RFC 2960";
static const char *shortname = "SCTP";
#ifdef LFS
static char devname[256] = "/dev/streams/clone/ua-as";
static char xptname[256] = "/dev/streams/clone/sctp_t";
#else
static char devname[256] = "/dev/ua-as";
static char xptname[256] = "/dev/sctp_t";
#endif

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

#if 1
static int client_port_specified = 0;
static int server_port_specified = 0;
static int client_host_specified = 0;
static int server_host_specified = 0;
#endif

static int verbose = 1;

static int client_exec = 0;		/* execute client side */
static int server_exec = 0;		/* execute server side */

#if 1
static uint32_t aspid = 0;

struct {
	uint32_t num;
	char text[32];
} iids[2] = { {
0,},};
#endif				/* TEST_M2UA */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
int show_fisus = 1;

#if TEST_X400
static int show_msus = 1;
#endif

//static int show_data = 1;

int last_prim = 0;
int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
int last_prio = 0;

#if TEST_M2PA
static int PRIM_type = 0;
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
#endif				/* TEST_M2PA */

#if TEST_M2UA
static int PRIM_type = 0;
static int CONIND_number = 2;
static int TOKEN_value = 0;
static int SEQ_number = 1;
static int SERV_type = T_COTS_ORD;
static int CURRENT_state = TS_UNBND;
struct T_info_ack_t last_info = { 0, };

static int DATA_xfer_flags = 0;
static int BIND_flags = 0;
static int DISCONN_reason = 0;
static int CONN_flags = 0;
static int ERROR_type = 0;
static int RESERVED_field[2] = { 0, 0 };
#endif				/* TEST_M2UA */

#define TEST_PROTOCOL 132

#define CHILD_PTU   0
#define CHILD_IUT   1

int test_fd[3] = { 0, 0, 0 };
uint32_t bsn[3] = { 0, 0, 0 };
uint32_t fsn[3] = { 0, 0, 0 };
uint8_t fib[3] = { 0, 0, 0, };
uint8_t bib[3] = { 0, 0, 0, };
uint8_t li[3] = { 0, 0, 0, };
uint8_t sio[3] = { 0, 0, 0, };

static int iut_connects = 1;

#define MSU_LEN 35
int msu_len = MSU_LEN;

/*
   some globals for compressing events 
 */
int oldact = 0;			/* previous action */
int cntact = 0;			/* repeats of previous action */
int oldevt = 0;
int cntevt = 0;

int count = 0;
int tries = 0;

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

#if 0
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
#endif

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
#define SCTP_VERSION_2
#endif

#if 1
#ifndef SCTP_VERSION_2
typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[3] __attribute__ ((packed));
} addr_t;
#endif				/* SCTP_VERSION_2 */
#endif
#else
typedef unsigned short ppa_t;
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

enum {
	__STATUS_OUT_OF_SERVICE = 200, __STATUS_ALIGNMENT, __STATUS_PROVING_NORMAL, __STATUS_PROVING_EMERG,
	__STATUS_IN_SERVICE, __STATUS_PROCESSOR_OUTAGE, __STATUS_PROCESSOR_ENDED, __STATUS_BUSY,
	__STATUS_BUSY_ENDED, __STATUS_INVALID_STATUS, __STATUS_SEQUENCE_SYNC, __MSG_PROVING,
	__TEST_ACK, __TEST_TX_BREAK, __TEST_TX_MAKE, __TEST_BAD_ACK, __TEST_MSU_TOO_SHORT,
	__TEST_FISU, __TEST_FISU_S, __TEST_FISU_CORRUPT, __TEST_FISU_CORRUPT_S,
	__TEST_MSU_SEVEN_ONES, __TEST_MSU_TOO_LONG, __TEST_FISU_FISU_1FLAG, __TEST_FISU_FISU_2FLAG,
	__TEST_MSU_MSU_1FLAG, __TEST_MSU_MSU_2FLAG, __TEST_COUNT, __TEST_TRIES,
	__TEST_ETC, __TEST_SIB_S,
	__TEST_FISU_BAD_FIB, __TEST_LSSU_CORRUPT, __TEST_LSSU_CORRUPT_S,
	__TEST_MSU, __TEST_MSU_S,
	__TEST_SIO, __TEST_SIN, __TEST_SIE, __TEST_SIOS, __TEST_SIPO, __TEST_SIB, __TEST_SIX,
	__TEST_SIO2, __TEST_SIN2, __TEST_SIE2, __TEST_SIOS2, __TEST_SIPO2, __TEST_SIB2, __TEST_SIX2,
};

#define __EVENT_IUT_IN_SERVICE	    __STATUS_IN_SERVICE
#define __EVENT_IUT_OUT_OF_SERVICE  __STATUS_OUT_OF_SERVICE
#define __EVENT_IUT_RPO		    __STATUS_PROCESSOR_OUTAGE
#define __EVENT_IUT_RPR		    __STATUS_PROCESSOR_ENDED
#define __EVENT_IUT_DATA	    __TEST_DATA

enum {
	__TEST_POWER_ON = 300, __TEST_START, __TEST_STOP, __TEST_LPO, __TEST_LPR, __TEST_EMERG,
	__TEST_CEASE, __TEST_SEND_MSU, __TEST_SEND_MSU_S, __TEST_CONG_A, __TEST_CONG_D,
	__TEST_NO_CONG, __TEST_CLEARB, __TEST_SYNC, __TEST_CONTINUE,
};

enum {
	__TEST_ATTACH_REQ = 400, __TEST_DETACH_REQ, __TEST_ENABLE_REQ, __TEST_ENABLE_CON,
	__TEST_DISABLE_REQ, __TEST_DISABLE_CON, __TEST_ERROR_IND, __TEST_SDL_OPTIONS,
	__TEST_SDL_CONFIG, __TEST_SDT_OPTIONS, __TEST_SDT_CONFIG, __TEST_SL_OPTIONS,
	__TEST_SL_CONFIG, __TEST_SDL_STATS, __TEST_SDT_STATS, __TEST_SL_STATS,
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static int ss7_pvar = SS7_PVAR_ITUT_00;

struct test_stats {
	sdl_stats_t sdl;
	sdt_stats_t sdt;
	sl_stats_t sl;
} iutstat;

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
	char *name;
} timer_range_t;

enum { t1 = 0, t2, t3, t4n, t4e, t5, t6, t7, tmax };

timer_range_t timer[tmax] = {
	{40000, 50000, "T1"},	/* Timer T1 30000 */
	{5000, 150000, "T2"},	/* Timer T2 5000 */
	{1000, 1500, "T3"},	/* Timer T3 100 */
	{7500, 9500, "T4(Pn)"},	/* Timer T4n 3000 */
	{200, 800, "T4(Pe)"},	/* Timer T4e 50 */
	{125, 125, "T5"},	/* Timer T5 10 */
	{3000, 6000, "T6"},	/* Timer T6 300 */
	{500, 2000, "T7"}	/* Timer T7 50 */
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

int
time_event(int child, int event)
{
	static const char *msgs[] = {
		"                    ! %11.6g                |                    <%d:%03d>\n",
		"                    |                %11.6g !                    <%d:%03d>\n",
		"                    |             %11.6g !  |                    <%d:%03d>\n",
		"                    !        %11.6g         !                    <%d:%03d>\n",
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

long beg_time = 0;

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

		snprintf(buf, sizeof(buf), "[%ld]", (long) err);
		return buf;
	}
	}
}

#if TEST_M2UA
char *
terrno_string(t_uscalar_t terr, t_scalar_t uerr)
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

		snprintf(buf, sizeof(buf), "[%lu]", (ulong) terr);
		return buf;
	}
	}
}
#endif				/* TEST_M2UA */

#if TEST_M2PA
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
#endif				/* TEST_M2PA */

#if 0
const char *
lmi_strreason(unsigned int reason)
{
	switch (reason) {
	default:
	case LMI_UNSPEC:
		return ("Unknown or unspecified");
	case LMI_BADADDRESS:
		return ("Address was invalid");
	case LMI_BADADDRTYPE:
		return ("Invalid address type");
	case LMI_BADDIAL:
		return ("(not used)");
	case LMI_BADDIALTYPE:
		return ("(not used)");
	case LMI_BADDISPOSAL:
		return ("Invalid disposal parameter");
	case LMI_BADFRAME:
		return ("Defective SDU received");
	case LMI_BADPPA:
		return ("Invalid PPA identifier");
	case LMI_BADPRIM:
		return ("Unregognized primitive");
	case LMI_DISC:
		return ("Disconnected");
	case LMI_EVENT:
		return ("Protocol-specific event ocurred");
	case LMI_FATALERR:
		return ("Device has become unusable");
	case LMI_INITFAILED:
		return ("Link initialization failed");
	case LMI_NOTSUPP:
		return ("Primitive not supported by this device");
	case LMI_OUTSTATE:
		return ("Primitive was issued from invalid state");
	case LMI_PROTOSHORT:
		return ("M_PROTO block too short");
	case LMI_SYSERR:
		return ("UNIX system error");
	case LMI_WRITEFAIL:
		return ("Unitdata request failed");
	case LMI_CRCERR:
		return ("CRC or FCS error");
	case LMI_DLE_EOT:
		return ("DLE EOT detected");
	case LMI_FORMAT:
		return ("Format error detected");
	case LMI_HDLC_ABORT:
		return ("Aborted frame detected");
	case LMI_OVERRUN:
		return ("Input overrun");
	case LMI_TOOSHORT:
		return ("Frame too short");
	case LMI_INCOMPLETE:
		return ("Partial frame received");
	case LMI_BUSY:
		return ("Telephone was busy");
	case LMI_NOANSWER:
		return ("Connection went unanswered");
	case LMI_CALLREJECT:
		return ("Connection rejected");
	case LMI_HDLC_IDLE:
		return ("HDLC line went idle");
	case LMI_HDLC_NOTIDLE:
		return ("HDLC link no longer idle");
	case LMI_QUIESCENT:
		return ("Line being reassigned");
	case LMI_RESUMED:
		return ("Line has been reassigned");
	case LMI_DSRTIMEOUT:
		return ("Did not see DSR in time");
	case LMI_LAN_COLLISIONS:
		return ("LAN excessive collisions");
	case LMI_LAN_REFUSED:
		return ("LAN message refused");
	case LMI_LAN_NOSTATION:
		return ("LAN no such station");
	case LMI_LOSTCTS:
		return ("Lost Clear to Send signal");
	case LMI_DEVERR:
		return ("Start of device-specific error codes");
	}
}

char *
lmerrno_string(long uerr, ulong lmerr)
{
	switch (lmerr) {
	case LMI_UNSPEC:	/* Unknown or unspecified */
		return ("[LMI_UNSPEC]");
	case LMI_BADADDRESS:	/* Address was invalid */
		return ("[LMI_BADADDRESS]");
	case LMI_BADADDRTYPE:	/* Invalid address type */
		return ("[LMI_BADADDRTYPE]");
	case LMI_BADDIAL:	/* (not used) */
		return ("[LMI_BADDIAL]");
	case LMI_BADDIALTYPE:	/* (not used) */
		return ("[LMI_BADDIALTYPE]");
	case LMI_BADDISPOSAL:	/* Invalid disposal parameter */
		return ("[LMI_BADDISPOSAL]");
	case LMI_BADFRAME:	/* Defective SDU received */
		return ("[LMI_BADFRAME]");
	case LMI_BADPPA:	/* Invalid PPA identifier */
		return ("[LMI_BADPPA]");
	case LMI_BADPRIM:	/* Unregognized primitive */
		return ("[LMI_BADPRIM]");
	case LMI_DISC:		/* Disconnected */
		return ("[LMI_DISC]");
	case LMI_EVENT:	/* Protocol-specific event ocurred */
		return ("[LMI_EVENT]");
	case LMI_FATALERR:	/* Device has become unusable */
		return ("[LMI_FATALERR]");
	case LMI_INITFAILED:	/* Link initialization failed */
		return ("[LMI_INITFAILED]");
	case LMI_NOTSUPP:	/* Primitive not supported by this device */
		return ("[LMI_NOTSUPP]");
	case LMI_OUTSTATE:	/* Primitive was issued from invalid state */
		return ("[LMI_OUTSTATE]");
	case LMI_PROTOSHORT:	/* M_PROTO block too short */
		return ("[LMI_PROTOSHORT]");
	case LMI_SYSERR:	/* UNIX system error */
		return errno_string(uerr);
	case LMI_WRITEFAIL:	/* Unitdata request failed */
		return ("[LMI_WRITEFAIL]");
	case LMI_CRCERR:	/* CRC or FCS error */
		return ("[LMI_CRCERR]");
	case LMI_DLE_EOT:	/* DLE EOT detected */
		return ("[LMI_DLE_EOT]");
	case LMI_FORMAT:	/* Format error detected */
		return ("[LMI_FORMAT]");
	case LMI_HDLC_ABORT:	/* Aborted frame detected */
		return ("[LMI_HDLC_ABORT]");
	case LMI_OVERRUN:	/* Input overrun */
		return ("[LMI_OVERRUN]");
	case LMI_TOOSHORT:	/* Frame too short */
		return ("[LMI_TOOSHORT]");
	case LMI_INCOMPLETE:	/* Partial frame received */
		return ("[LMI_INCOMPLETE]");
	case LMI_BUSY:		/* Telephone was busy */
		return ("[LMI_BUSY]");
	case LMI_NOANSWER:	/* Connection went unanswered */
		return ("[LMI_NOANSWER]");
	case LMI_CALLREJECT:	/* Connection rejected */
		return ("[LMI_CALLREJECT]");
	case LMI_HDLC_IDLE:	/* HDLC line went idle */
		return ("[LMI_HDLC_IDLE]");
	case LMI_HDLC_NOTIDLE:	/* HDLC link no longer idle */
		return ("[LMI_HDLC_NOTIDLE]");
	case LMI_QUIESCENT:	/* Line being reassigned */
		return ("[LMI_QUIESCENT]");
	case LMI_RESUMED:	/* Line has been reassigned */
		return ("[LMI_RESUMED]");
	case LMI_DSRTIMEOUT:	/* Did not see DSR in time */
		return ("[LMI_DSRTIMEOUT]");
	case LMI_LAN_COLLISIONS:	/* LAN excessive collisions */
		return ("[LMI_LAN_COLLISIONS]");
	case LMI_LAN_REFUSED:	/* LAN message refused */
		return ("[LMI_LAN_REFUSED]");
	case LMI_LAN_NOSTATION:	/* LAN no such station */
		return ("[LMI_LAN_NOSTATION]");
	case LMI_LOSTCTS:	/* Lost Clear to Send signal */
		return ("[LMI_LOSTCTS]");
	case LMI_DEVERR:	/* Start of device-specific error codes */
		return ("[LMI_DEVERR]");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "[%lu]", (ulong) lmerr);
		return buf;
	}
	}
}
#endif

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

#if TEST_M2UA
const char *
service_type(t_uscalar_t type)
{
	switch (type) {
	case T_CLTS:
		return ("T_CLTS");
	case T_COTS:
		return ("T_COTS");
	case T_COTS_ORD:
		return ("T_COTS_ORD");
	default:
		return ("(unknown)");
	}
}

const char *
state_string(t_uscalar_t state)
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
#endif				/* TEST_M2UA */

#if 0
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

#if TEST_M2UA
char *
mgmtflag_string(t_uscalar_t flag)
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
#endif				/* TEST_M2UA */
#if TEST_M2PA
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
#endif				/* TEST_M2PA */

#if TEST_M2UA
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

#if TEST_M2UA
const char *
prim_string(t_uscalar_t prim)
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
		return ("T_????_??? -----");
	}
}

char *
t_errno_string(t_scalar_t err, t_scalar_t syserr)
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

		snprintf(buf, sizeof(buf), "[%ld]", (long) err);
		return buf;
	}
	}
}

char *
t_look_string(int look)
{
	switch (look) {
	case 0:
		return ("(NO EVENT)");
	case T_LISTEN:
		return ("(T_LISTEN)");
	case T_CONNECT:
		return ("(T_CONNECT)");
	case T_DATA:
		return ("(T_DATA)");
	case T_EXDATA:
		return ("(T_EXDATA)");
	case T_DISCONNECT:
		return ("(T_DISCONNECT)");
	case T_UDERR:
		return ("(T_UDERR)");
	case T_ORDREL:
		return ("(T_ORDREL)");
	case T_GODATA:
		return ("(T_GODATA)");
	case T_GOEXDATA:
		return ("(T_GOEXDATA)");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(%d)", look);
		return buf;
	}
	}
}
#endif				/* TEST_M2UA */

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

#if 0
const char *
oos_string(sl_ulong reason)
{
	switch (reason) {
	case SL_FAIL_UNSPECIFIED:
		return ("!out of service (unspec)");
	case SL_FAIL_CONG_TIMEOUT:
		return ("!out of service (T6)");
	case SL_FAIL_ACK_TIMEOUT:
		return ("!out of service (T7)");
	case SL_FAIL_ABNORMAL_BSNR:
		return ("!out of service (BSNR)");
	case SL_FAIL_ABNORMAL_FIBR:
		return ("!out of service (FIBR)");
	case SL_FAIL_SUERM_EIM:
		return ("!out of service (SUERM)");
	case SL_FAIL_ALIGNMENT_NOT_POSSIBLE:
		return ("!out of service (AERM)");
	case SL_FAIL_RECEIVED_SIO:
		return ("!out of service (SIO)");
	case SL_FAIL_RECEIVED_SIN:
		return ("!out of service (SIN)");
	case SL_FAIL_RECEIVED_SIE:
		return ("!out of service (SIE)");
	case SL_FAIL_RECEIVED_SIOS:
		return ("!out of service (SIOS)");
	case SL_FAIL_T1_TIMEOUT:
		return ("!out of service (T1)");
	default:
		return ("!out of service (\?\?\?)");
	}
}
#endif

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
#if 0
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		return ("!rpo");
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		return ("!rpr");
	case SL_IN_SERVICE_IND:
		return ("!in service");
	case SL_OUT_OF_SERVICE_IND:
		return ("!out of service");
	case SL_PDU_IND:
		return ("!msu");
	case SL_LINK_CONGESTED_IND:
		return ("!congested");
	case SL_LINK_CONGESTION_CEASED_IND:
		return ("!congestion ceased");
	case SL_RETRIEVED_MESSAGE_IND:
		return ("!retrieved message");
	case SL_RETRIEVAL_COMPLETE_IND:
		return ("!retrieval complete");
	case SL_RB_CLEARED_IND:
		return ("!rb cleared");
	case SL_BSNT_IND:
		return ("!bsnt");
	case SL_RTB_CLEARED_IND:
		return ("!rtb cleared");
#if TEST_X400
	case SDT_RC_SIGNAL_UNIT_IND:
		return ("(!su)");
	case SDT_IAC_CORRECT_SU_IND:
		return ("(!correct su)");
	case SDT_IAC_ABORT_PROVING_IND:
		return ("(!abort proving)");
	case SDT_LSC_LINK_FAILURE_IND:
		return ("(!link failure)");
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		return ("(!tx request)");
	case SDT_RC_CONGESTION_ACCEPT_IND:
		return ("(!cong accept)");
	case SDT_RC_CONGESTION_DISCARD_IND:
		return ("(!cong discard)");
	case SDT_RC_NO_CONGESTION_IND:
		return ("(!no congestion)");
#endif				/* TEST_X400 */
	case LMI_INFO_ACK:
		return ("!info ack");
	case LMI_OK_ACK:
		return ("!ok ack");
	case LMI_ERROR_ACK:
		return ("!error ack");
	case LMI_ENABLE_CON:
		return ("!enable con");
	case LMI_DISABLE_CON:
		return ("!disable con");
	case LMI_ERROR_IND:
		return ("!error ind");
	case LMI_STATS_IND:
		return ("!stats ind");
	case LMI_EVENT_IND:
		return ("!event ind");
#endif
	default:
		return ("?_????_??? -----");
	}
}
#endif

#if TEST_M2PA
static const char *
status_string(uint32_t status)
{
	switch (status) {
	case M2PA_STATUS_OUT_OF_SERVICE:
		return ("OUT-OF-SERVICE");
	case M2PA_STATUS_IN_SERVICE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT3:
		case M2PA_VERSION_DRAFT3_1:
			return ("IN-SERVICE");
		}
		return ("READY");
	case M2PA_STATUS_PROVING_NORMAL:
		return ("PROVING-NORMAL");
	case M2PA_STATUS_PROVING_EMERGENCY:
		return ("PROVING-EMERGENCY");
	case M2PA_STATUS_ALIGNMENT:
		return ("ALIGNMENT");
	case M2PA_STATUS_PROCESSOR_OUTAGE:
		return ("PROCESSOR-OUTAGE");
	case M2PA_STATUS_BUSY:
		return ("BUSY");
	case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			return ("PROCESSOR-RECOVERED");
		case M2PA_VERSION_DRAFT3:
		case M2PA_VERSION_DRAFT3_1:
		case M2PA_VERSION_DRAFT4:
		case M2PA_VERSION_DRAFT4_1:
		case M2PA_VERSION_DRAFT5:
		case M2PA_VERSION_DRAFT5_1:
		case M2PA_VERSION_DRAFT6:
		case M2PA_VERSION_DRAFT6_1:
		case M2PA_VERSION_DRAFT6_9:
		case M2PA_VERSION_DRAFT7:
		default:
			break;
		}
		return ("PROCESSOR-OUTAGE-ENDED");
	case M2PA_STATUS_BUSY_ENDED:
		return ("BUSY-ENDED");
	case M2PA_STATUS_NONE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT4:
		case M2PA_VERSION_DRAFT4_1:
		case M2PA_VERSION_DRAFT4_9:
			return ("ACK");
		}
		break;
	case M2PA_STATUS_INVALID:
		return ("[INVALID-STATUS]");
	}
	return ("[UNKNOWN-STATUS]");
}

static const char *
msg_string(uint32_t msg)
{
	switch (msg) {
	case M2PA_STATUS_MESSAGE:
		return ("STATUS");
	case M2PA_ACK_MESSAGE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT3_1:
			return ("ACK");
		}
		break;
	case M2PA_PROVING_MESSAGE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT3:
		case M2PA_VERSION_DRAFT3_1:
			return ("PROVING");
		}
		break;
	case M2PA_DATA_MESSAGE:
		return ("DATA");
	}
	return ("[UNKNOWN-MESSAGE]");
}
#endif				/* TEST_M2PA */

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
		"--------------------+-------------Preamble--------------+                   \n",
		"                    +-------------Preamble--------------+-------------------\n",
		"                    +-------------Preamble-----------+--+-------------------\n",
		"--------------------+-------------Preamble--------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failure(int child, const char *string)
{
	static const char *msgs[] = {
		"....................|%-32.32s   |                    [%d:%03d]\n",
		"                    |%-32.32s   |................... [%d:%03d]\n",
		"                    |%-32.32s|...................... [%d:%03d]\n",
		"....................|%-32.32s...|................... [%d:%03d]\n",
	};

	if (string && strnlen(string, 32) > 0 && verbose > 0)
		print_string_state(child, msgs, string);
}

void
print_notapplicable(int child)
{
	static const char *msgs[] = {
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|                    [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-|-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_skipped(int child)
{
	static const char *msgs[] = {
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|                    [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED ::::::::::::::|::::::::::::::::::: [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED :::::::::::|::|::::::::::::::::::: [%d:%03d]\n",
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|::::::::::::::::::: [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ???????????|                    [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ???????????|??????????????????? [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ????????|??|??????????????????? [%d:%03d]\n",
		"????????????????????|?????????? INCONCLUSIVE ???????????|??????????????????? [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+---------------Test----------------+                   \n",
		"                    +---------------Test----------------+-------------------\n",
		"                    +---------------Test-------------+--+-------------------\n",
		"--------------------+---------------Test----------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|                    [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR ##########|                    [%d:%03d]\n",
		"                    |########### SCRIPT ERROR ##########|################### [%d:%03d]\n",
		"                    |########### SCRIPT ERROR #######|##|################### [%d:%03d]\n",
		"####################|########### SCRIPT ERROR ##########|################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED **************|                    [%d:%03d]\n",
		"                    |************* PASSED **************|******************* [%d:%03d]\n",
		"                    |************* PASSED ***********|**|******************* [%d:%03d]\n",
		"********************|************* PASSED **************|******************* [%d:%03d]\n",
	};

	if (verbose > 2)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Postamble-------------+                   \n",
		"                    +-------------Postamble-------------+-------------------\n",
		"                    +-------------Postamble----------+--+-------------------\n",
		"--------------------+-------------Postamble-------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+-----------------------------------+                   \n",
		"                    +-----------------------------------+-------------------\n",
		"                    +--------------------------------+--+-------------------\n",
		"--------------------+-----------------------------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|                    {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|                    {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|                    [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
	};

	if (show_timeout || verbose > 1) {
		print_double_int(child, msgs, child, state);
		show_timeout--;
	}
}

void
print_nothing(int child)
{
	static const char *msgs[] = {
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|                    [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - - -|- - - - - - - - - - [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - | -|- - - - - - - - - - [%d:%03d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|- - - - - - - - - - [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_double_int(child, msgs, child, state);
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                   |                    [%d:%03d]\n",
		"                    |                                   |<---%-14s  [%d:%03d]\n",
		"                    |                                |<-+----%-14s  [%d:%03d]\n",
		"                    |          %-14s           |                    [%d:%03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, command);
}

void
print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->| - - - - - - - - - - - - - - - - ->|                    [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - - - |<-%16s- [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s- [%d:%03d]\n",
		"                    |         %-16s          |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_rx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|<- - - - - - - - - - - - - - - - - |                    [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - - - >|--%16s> [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - ->|--+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, command);
}

#if 0
void
print_string_state_sn(int child, const char *msgs[], const char *string, uint32_t bsn, uint32_t fsn)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, bsn, fsn, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_tx_msg_sn(int child, const char *string, uint32_t bsn, uint32_t fsn)
{
	static const char *msgs[] = {
		"%1$20.20s| ---------[%3$06X,%2$06X]--------> |                    [%4$d:%5$03d]\n",
		"                    | <--------[%2$06X,%3$06X]--------- |%1$-20.20s[%4$d:%5$03d]\n",
		"                    | <--------[%2$06X,%3$06X]------ |  |%1$-20.20s[%4$d:%5$03d]\n",
		"                    |      <%1$-20.20s>       |                    [%4$d:%5$03d]\n",
	};

	if (show && verbose > 0)
		print_string_state_sn(child, msgs, string, bsn, fsn);
}

void
print_rx_msg_sn(int child, const char *string, uint32_t bsn, uint32_t fsn)
{
	static const char *msgs[] = {
		"%1$20.20s| <--------[%2$06X,%3$06X]--------- |                    [%4$d:%5$03d]\n",
		"                    | ---------[%3$06X,%2$06X]--------> |%1$-20.20s[%4$d:%5$03d]\n",
		"                    | ---------[%3$06X,%2$06X]-----> |   %1$-20.20s[%4$d:%5$03d]\n",
		"                    |       <%1$20.20s>      |                    [%4$d:%5$03d]\n",
	};

	if (show && verbose > 0)
		print_string_state_sn(child, msgs, string, bsn, fsn);
}

void
print_tx_msg(int child, const char *string)
{
	static const char *msgs[] = {
		"%20.20s| --------------------------------> |                    [%d:%03d]\n",
		"                    | <-------------------------------- |%-20.20s[%d:%03d]\n",
		"                    | <----------------------------- |  |%-20.20s[%d:%03d]\n",
		"                    |      <%-20.20s>       |                    [%d:%03d]\n",
	};

	if (show && verbose > 0) {
#if TEST_X400
		if (fsn[child] != 0x7f || bsn[child] != 0x7f || fib[child] != 0x80 || bib[child] != 0x80)
			print_tx_msg_sn(child, string, bib[child] | bsn[child], fib[child] | fsn[child]);
		else
			print_string_state(child, msgs, string);
#endif				/* TEST_X400 */
#if TEST_M2PA
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT4:
		case M2PA_VERSION_DRAFT4_1:
		case M2PA_VERSION_DRAFT4_9:
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			if (fsn[child] != 0xffffff || bsn[child] != 0xffffff) {
				print_tx_msg_sn(child, string, bsn[child], fsn[child]);
				return;
			}
		default:
			print_string_state(child, msgs, string);
			return;
		}
#endif				/* TEST_M2PA */
	}
}

void
print_rx_msg(int child, const char *string)
{
	static const char *msgs[] = {
		"%20.20s| <-------------------------------- |                    [%d:%03d]\n",
		"                    | --------------------------------> |%-20.20s[%d:%03d]\n",
		"                    | -----------------------------> |   %-20.20s[%d:%03d]\n",
		"                    |       <%20.20s>      |                    [%d:%03d]\n",
	};

	if (show && verbose > 0) {
		int other = (child + 1) % 2;

#if TEST_X400
		if (fsn[other] != 0x7f || bsn[other] != 0x7f || fib[other] != 0x80 || bib[other] != 0x80)
			print_rx_msg_sn(child, string, bib[other] | bsn[other], fib[other] | fsn[other]);
		else
			print_string_state(child, msgs, string);
#endif				/* TEST_X400 */
#if TEST_M2PA
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT4:
		case M2PA_VERSION_DRAFT4_1:
		case M2PA_VERSION_DRAFT4_9:
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			if (fsn[other] != 0xffffff || bsn[other] != 0xffffff) {
				print_rx_msg_sn(child, string, bsn[other], fsn[other]);
				return;
			}
		default:
			print_string_state(child, msgs, string);
			return;
		}
#endif				/* TEST_M2PA */
	}
}
#endif

void
print_ack_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s-/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-%16s> [%d:%03d]\n",
		"                    |                                |\\-+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
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
		"????%4ld????  ?----?| ?- - - - - - - - - - - - - - - -? |                     [%d:%03d]\n",
		"                    | ?- - - - - - - - - - - - - - - -? |?--? ????%4ld????    [%d:%03d]\n",
		"                    | ?- - - - - - - -- - - - - - -? |?-+---? ????%4ld????    [%d:%03d]\n",
		"                    | ?- - - - - - -%4ld- - - - - -? |  |                     [%d:%03d]\n",
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
		"%1$-14s----->|         %2$-16.16s          |                    [%3$d:%4$03d]\n",
		"                    |         %2$-16.16s          |<---%1$-14s  [%3$d:%4$03d]\n",
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
		"--%1$16s->| - %2$5d bytes - - - - - - - - - ->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - - - |<-%1$16s- [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+ -%1$16s- [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show) || verbose > 4)
		print_string_int_state(child, msgs, command, bytes);
}

void
print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - - -|                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - - >|--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|--+--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_errno(int child, long error)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, errno_string(error));
}

void
print_success(int child)
{
	static const char *msgs[] = {
		"  ok          <----/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\---->         ok   [%d:%03d]\n",
		"                    |                                |\\-+----->         ok   [%d:%03d]\n",
		"                    |                 ok                |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_double_int(child, msgs, child, state);
}

void
print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"  %10d  <----/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\---->  %10d  [%d:%03d]\n",
		"                    |                                |\\-+----->  %10d  [%d:%03d]\n",
		"                    |            [%10d]           |                    [%d:%03d]\n",
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
		"  %1$10d  <----/| %2$-30.30s    |                    [%3$d:%4$03d]\n",
		"                    | %2$-30.30s    |\\---->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-30.30s |\\-+----->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-20.20s [%1$10d] |                    [%3$d:%4$03d]\n",
	};

	if (show && verbose > 3)
		print_int_string_state(child, msgs, value, poll_events_string(revents));
}

void
print_ti_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"--ioctl(2)--------->|       %16s            |                    [%d:%03d]\n",
		"                    |       %16s            |<---ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s         |<-+----ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s ioctl(2)   |                    [%d:%03d]\n",
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
		"  %1$16s->| - %2$5d bytes - - - - - - - - - ->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - - - |<-%1$16s  [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+--%1$16s  [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_libcall(int child, const char *command)
{
	static const char *msgs[] = {
		"  %-16s->|                                   |                    [%d:%03d]\n",
		"                    |                                   |<-%16s  [%d:%03d]\n",
		"                    |                                |<-+--%16s  [%d:%03d]\n",
		"                    |        [%16s]         |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

#if 0
void
print_terror(int child, long error, long terror)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
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
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-|--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, t_look_string(tlook));
}
#endif

void
print_expect(int child, int want)
{
	static const char *msgs[] = {
		" (%-16s) |- - - - - -[Expected]- - - - - - - |                    [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - - - | (%-16s) [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - -|- | (%-16s) [%d:%03d]\n",
		"                    |- [Expected %-16s ] - - |                    [%d:%03d]\n",
	};

	if (verbose > 1 && show)
		print_string_state(child, msgs, event_string(child, want));
}

void
print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-20s|                                   |                    \n",
		"                    |                                   |%-20s\n",
		"                    |                                |   %-20s\n",
		"                    |       %-20s        |                    \n",
	};

	if (show && verbose > 0)
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
		"%20s|                                   |                    [%d:%03d]\n",
		"                    |                                   |%-20s[%d:%03d]\n",
		"                    |                                |   %-20s[%d:%03d]\n",
		"                    |       %-20s        |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
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
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / |                    [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / / / | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 1 && show)
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
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ /|/ / / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 1 && show) {
		float delay;

		delay = time->tv_nsec;
		delay = delay / 1000000000;
		delay = delay + time->tv_sec;
		print_float_state(child, msgs, delay);
	}
}

#if 0
#if TEST_M2PA || TEST_M2UA
void
print_mgmtflag(int child, uint flag)
{
	print_string(child, mgmtflag_string(flag));
}
#endif				/* TEST_M2PA || TEST_M2UA */

#if TEST_M2UA
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
#endif				/* TEST_M2UA */
#endif

#if 0
void
print_options(int child, const char *cmd_buf, size_t qos_ofs, size_t qos_len)
{
	unsigned char *qos_ptr = (unsigned char *) (cmd_buf + qos_ofs);
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

void
print_info(int child, N_info_ack_t * info)
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

#if TEST_M2PA
int
test_m2pa_status(int child, uint32_t status)
{
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	struct strbuf ctrl_buf = {.buf = cbuf,.len = 0,.maxlen = 0, };
	struct strbuf data_buf = {.buf = dbuf,.len = 0,.maxlen = 0, };
	struct strbuf *ctrl = &ctrl_buf;
	struct strbuf *data = &data_buf;
	union N_primitives *p = (typeof(p)) cbuf;

	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		data->len = 3 * sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = status;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		data->len = 4 * sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint16_t *) dbuf)[4] = htons(bsn[child]);
		((uint16_t *) dbuf)[5] = htons(fsn[child]);
		((uint32_t *) dbuf)[3] = status;
		break;
	case M2PA_VERSION_DRAFT6_9:
	case M2PA_VERSION_DRAFT7:
		data->len = 3 * sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = status;
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT9:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		data->len = 5 * sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = htonl(bsn[child] & 0x00ffffff);
		((uint32_t *) dbuf)[3] = htonl(fsn[child] & 0x00ffffff);
		((uint32_t *) dbuf)[4] = status;
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	ctrl->len = sizeof(p->data_req) + sizeof(qos[0].data);
	p->type = N_DATA_REQ;
	p->data_req.DATA_xfer_flags = 0;
	bcopy(&qos[0].data, (&p->data_req) + 1, sizeof(qos[0].data));
	print_tx_msg(child, status_string(status));
	return test_putpmsg(child, ctrl, data, 0, MSG_BAND);
}

int
test_m2pa_data(int child)
{
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	struct strbuf ctrl_buf = {.buf = cbuf,.len = 0,.maxlen = 0, };
	struct strbuf data_buf = {.buf = dbuf,.len = 0,.maxlen = 0, };
	struct strbuf *ctrl = &ctrl_buf;
	struct strbuf *data = &data_buf;
	union N_primitives *p = (typeof(p)) cbuf;

	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		qos[0].data.sid = 1;
		data->len = 2 * sizeof(uint32_t) + msu_len + 1;
		ctrl->len = sizeof(p->data_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		memset(&(((uint32_t *) data->buf)[2]), 'B', msu_len + 1);
		memset(&(((uint32_t *) data->buf)[2]), 0, 1);
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		fsn[child] = (fsn[child] + 1) & 0xffff;
		qos[0].data.sid = 1;
		data->len = 3 * sizeof(uint32_t) + msu_len + 1;
		ctrl->len = sizeof(p->data_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint16_t *) data->buf)[4] = htons(bsn[child]);
		((uint16_t *) data->buf)[5] = htons(fsn[child]);
		memset(&(((uint32_t *) data->buf)[3]), 'B', msu_len + 1);
		memset(&(((uint32_t *) data->buf)[3]), 0, 1);
		break;
	default:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT5_1:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	case M2PA_VERSION_DRAFT7:
	case M2PA_VERSION_DRAFT9:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		fsn[child] = (fsn[child] + 1) & 0xffffff;
		qos[0].data.sid = 1;
		data->len = 4 * sizeof(uint32_t) + msu_len + 1;
		ctrl->len = sizeof(p->data_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint32_t *) data->buf)[2] = htonl(bsn[child]);
		((uint32_t *) data->buf)[3] = htonl(fsn[child]);
		memset(&(((uint32_t *) data->buf)[4]), 'B', msu_len + 1);
		memset(&(((uint32_t *) data->buf)[4]), 0, 1);
		break;
	}
	p->type = N_DATA_REQ;
	p->data_req.DATA_xfer_flags = (m2pa_version == M2PA_VERSION_DRAFT3) ? N_RC_FLAG : 0;
	bcopy(&qos[0].data, (&p->data_req) + 1, sizeof(qos[0].data));
	print_tx_msg(child, "DATA");
	return test_putpmsg(child, ctrl, data, 0, MSG_BAND);
}

static size_t nacks = 1;

int
test_m2pa_ack(int child)
{
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	struct strbuf ctrl_buf = {.buf = cbuf,.len = 0,.maxlen = 0, };
	struct strbuf data_buf = {.buf = dbuf,.len = 0,.maxlen = 0, };
	struct strbuf *ctrl = &ctrl_buf;
	struct strbuf *data = &data_buf;
	union N_primitives *p = (typeof(p)) cbuf;

	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		qos[0].data.sid = 1;
		data->len = 3 * sizeof(uint32_t);
		ctrl->len = sizeof(p->exdata_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_ACK_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint32_t *) data->buf)[2] = htonl(nacks);
		p->type = N_EXDATA_REQ;
		bcopy(&qos[0].data, (&p->exdata_req) + 1, sizeof(qos[0].data));
		break;

	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		qos[0].data.sid = 0;
		data->len = 4 * sizeof(uint32_t);
		ctrl->len = sizeof(p->exdata_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint16_t *) data->buf)[4] = htons(bsn[child]);
		((uint16_t *) data->buf)[5] = htons(fsn[child]);
		((uint32_t *) data->buf)[3] = M2PA_STATUS_NONE;
		p->type = N_EXDATA_REQ;
		bcopy(&qos[0].data, (&p->exdata_req) + 1, sizeof(qos[0].data));
		break;

	case M2PA_VERSION_DRAFT4_9:
		qos[0].data.sid = 0;
		data->len = 5 * sizeof(uint32_t);
		ctrl->len = sizeof(p->exdata_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint32_t *) data->buf)[2] = htonl(bsn[child] & 0xffffff);
		((uint32_t *) data->buf)[3] = htonl(fsn[child] & 0xffffff);
		((uint32_t *) data->buf)[4] = M2PA_STATUS_NONE;
		p->type = N_EXDATA_REQ;
		bcopy(&qos[0].data, (&p->exdata_req) + 1, sizeof(qos[0].data));
		break;

	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT5_1:
		qos[0].data.sid = 0;
		data->len = 5 * sizeof(uint32_t);
		ctrl->len = sizeof(p->exdata_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint32_t *) data->buf)[2] = htonl(bsn[child] & 0xffffff);
		((uint32_t *) data->buf)[3] = htonl(fsn[child] & 0xffffff);
		((uint32_t *) data->buf)[4] = M2PA_STATUS_IN_SERVICE;
		p->type = N_EXDATA_REQ;
		bcopy(&qos[0].data, (&p->exdata_req) + 1, sizeof(qos[0].data));
		break;

	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	case M2PA_VERSION_DRAFT7:
	case M2PA_VERSION_DRAFT9:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		qos[0].data.sid = 1;
		data->len = 4 * sizeof(uint32_t);
		ctrl->len = sizeof(p->data_req) + sizeof(qos[0].data);
		((uint32_t *) data->buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) data->buf)[1] = htonl(data->len);
		((uint32_t *) data->buf)[2] = htonl(bsn[child] & 0xffffff);
		((uint32_t *) data->buf)[3] = htonl(fsn[child] & 0xffffff);
		p->type = N_DATA_REQ;
		p->data_req.DATA_xfer_flags = 0;
		bcopy(&qos[0].data, (&p->data_req) + 1, sizeof(qos[0].data));
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	print_tx_msg(child, "ACK");
	return test_putpmsg(child, ctrl, data, 0, MSG_BAND);
}
#endif				/* TEST_M2PA */

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
#if 0
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
#endif
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
#if 0
	qos_data.sid = 0;
	qos_info.hmac = SCTP_HMAC_NONE;
	qos_info.options = 0;
	qos_info.i_streams = 1;
	qos_info.o_streams = 1;
	qos_conn.i_streams = 1;
	qos_conn.o_streams = 1;
#endif
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
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */

int
preamble_0(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_0_1(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_RDONLY) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_0_2(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_WRONLY) != __RESULT_SUCCESS)
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
 *  Preambles and Postambles...
 *
 *  =========================================================================
 */

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

static const char sref_none[] = "(none)";

/*
 *  Open and Close 1 Stream.
 */
static const char test_group_1[] = "Open and close Streams";

#define tgrp_case_1_1 test_group_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 1 user BCM Stream."
#define sref_case_1_1 sref_none
#define desc_case_1_1 "\
Checks that one user BCM Stream can be opened and closed."

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
 *  Open and Close 3 Streams.
 */
#define tgrp_case_1_2 test_group_1
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Open and close 3 user BCM Streams."
#define sref_case_1_2 sref_none
#define desc_case_1_2 "\
Checks that three user BCM Streams can be opened and closed."

int
test_case_1_2(int child)
{
	return __RESULT_NOTAPPL;
#if 0
	/* just happens to work on UP (because it opens and closes so fast) */
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
#endif
}
struct test_stream test_1_2 = { NULL, &test_case_1_2, NULL };

#define test_case_1_2_stream_0 (&test_1_2)
#define test_case_1_2_stream_1 (&test_1_2)
#define test_case_1_2_stream_2 (&test_1_2)

#define tgrp_case_1_3 test_group_1
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Open and close 1 admin BCM Stream."
#define sref_case_1_3 sref_none
#define desc_case_1_3 "\
Checks that one admin BCM Stream can be opened and closed."

int
test_case_1_3(int child)
{
	if (getuid() != 0 && geteuid() != 0)
		return __RESULT_SKIPPED;
	if (test_open(child, devname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_1_3 = { NULL, &test_case_1_3, NULL };

#define test_case_1_3_stream_0 (&test_1_3)
#define test_case_1_3_stream_1 (NULL)
#define test_case_1_3_stream_2 (NULL)

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

#if 0
	oldact = 0;		/* previous action for this child */
	cntact = 0;		/* count of this action for this child */
	oldevt = 0;		/* previous return for this child */
	cntevt = 0;		/* count of this return for this child */
#endif

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
		numb_case_1_1, tgrp_case_1_1, NULL, name_case_1_1, NULL, desc_case_1_1, sref_case_1_1, {
	test_case_1_1_stream_0, test_case_1_1_stream_1, test_case_1_1_stream_2}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_2, tgrp_case_1_2, NULL, name_case_1_2, NULL, desc_case_1_2, sref_case_1_2, {
	test_case_1_2_stream_0, test_case_1_2_stream_1, test_case_1_2_stream_2}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_3, tgrp_case_1_3, NULL, name_case_1_3, NULL, desc_case_1_3, sref_case_1_3, {
	test_case_1_3_stream_0, test_case_1_3_stream_1, test_case_1_3_stream_2}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
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
		fprintf(stdout, "\nUsing device %s\n", devname);
		fprintf(stdout, "\nUsing transport %s\n\n", xptname);
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
    -T, --iids, --text IID[,IID]\n\
        specify the number or text IIDs for tests.\n\
    -A, --aspid ASPID\n\
        specify the ASP Identifier for tests.\n\
    -u, --iut\n\
        IUT connects instead of PT.\n\
    -F, --decl-std [STANDARD]\n\
        specify the SS7 standard version to test.\n\
    -D, --draft [DRAFT]\n\
        specify the M2UA draft or RFC version to test.\n\
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
        port number from which to connect [default: %4$d+index*3]\n\
    -P, --server-port [PORT]\n\
        port number to which to connect or upon which to listen\n\
        [default: %4$d+index*3+2]\n\
    -i, --client-host [HOSTNAME[,HOSTNAME]*]\n\
        client host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -I, --server-host [HOSTNAME[,HOSTNAME]*]\n\
        server host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -d, --device DEVICE\n\
        device name to open [default: %2$s].\n\
    -x, --transport DEVICE\n\
        device name to open for transport [default: %3$s].\n\
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
Symbols:\n\
    [STANDARD]\n\
        SS7_PVAR_ITUT_83  SS7_PVAR_ETSI_88  SS7_PVAR_ANSI_92  SS7_PVAR_SING_88\n\
        SS7_PVAR_ITUT_93  SS7_PVAR_ETSI_93  SS7_PVAR_JTTC_94  SS7_PVAR_SPAN_88\n\
        SS7_PVAR_ITUT_96  SS7_PVAR_ETSI_96  SS7_PVAR_ANSI_96\n\
        SS7_PVAR_ITUT_00  SS7_PVAR_ETSI_00  SS7_PVAR_ANSI_00  SS7_PVAR_CHIN_00\n\
\n\
", argv[0], devname, xptname, TEST_PORT_NUMBER);
}

#define HOST_BUF_LEN 128

int
main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;

#if 1
	char *hostc = "127.0.0.1,127.0.0.2,127.0.0.3";
	char *hosts = "127.0.0.1,127.0.0.2,127.0.0.3";
	char hostbufc[HOST_BUF_LEN];
	char hostbufs[HOST_BUF_LEN];
	struct hostent *haddr;
#endif

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
			{"iids",	required_argument,	NULL, 'T'},
			{"text",	required_argument,	NULL, 'T'},
			{"aspid",	required_argument,	NULL, 'A'},
			{"iut",		no_argument,		NULL, 'u'},
			{"draft",	required_argument,	NULL, 'D'},
			{"decl-std",	required_argument,	NULL, 'F'},
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
			{"transport",	required_argument,	NULL, 'x'},
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

		c = getopt_long(argc, argv, "T:A:uD:F:cSawp:P:i:I:rRd:x:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "T:A:uD:F:cSawp:P:i:I:rRd:x:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
#if 1
		case 'T':	/* -T, --iids, --text IID[,IID] */
		{
			char *token = NULL;

			if ((token = index(optarg, ','))) {
				token[0] = '\0';
				token++;
			}
			if (strtoul(optarg, NULL, 0) != 0)
				iids[0].num = strtoul(optarg, NULL, 0);
			else
				strncpy(iids[0].text, optarg, sizeof(iids[0].text));
			if (token) {
				if (strtoul(token, NULL, 0) != 0)
					iids[1].num = strtoul(token, NULL, 0);
				else
					strncpy(iids[1].text, token, sizeof(iids[1].text));
				token--;
				token[0] = ',';
			}
			break;
		}
		case 'A':	/* -A, --aspid ASPID */
			aspid = strtoul(optarg, NULL, 0);
			break;
#endif
		case 'u':	/* -u, --iut */
			iut_connects = 1;
			break;
		case 'F':	/* -F, --decl-std */
			ss7_pvar = strtoul(optarg, NULL, 0);
			switch (ss7_pvar) {
			case SS7_PVAR_ITUT_88:
			case SS7_PVAR_ITUT_93:
			case SS7_PVAR_ITUT_96:
			case SS7_PVAR_ITUT_00:
			case SS7_PVAR_ETSI_88:
			case SS7_PVAR_ETSI_93:
			case SS7_PVAR_ETSI_96:
			case SS7_PVAR_ETSI_00:
			case SS7_PVAR_ANSI_92:
			case SS7_PVAR_ANSI_96:
			case SS7_PVAR_ANSI_00:
			case SS7_PVAR_JTTC_94:
			case SS7_PVAR_CHIN_00:
			case SS7_PVAR_SING | SS7_PVAR_88:
			case SS7_PVAR_SPAN | SS7_PVAR_88:
				break;
			default:
				if (!strcmp(optarg, "SS7_PVAR_ITUT_88"))
					ss7_pvar = SS7_PVAR_ITUT_88;
				else if (!strcmp(optarg, "SS7_PVAR_ITUT_93"))
					ss7_pvar = SS7_PVAR_ITUT_93;
				else if (!strcmp(optarg, "SS7_PVAR_ITUT_96"))
					ss7_pvar = SS7_PVAR_ITUT_96;
				else if (!strcmp(optarg, "SS7_PVAR_ITUT_00"))
					ss7_pvar = SS7_PVAR_ITUT_00;
				else if (!strcmp(optarg, "SS7_PVAR_ETSI_88"))
					ss7_pvar = SS7_PVAR_ETSI_88;
				else if (!strcmp(optarg, "SS7_PVAR_ETSI_93"))
					ss7_pvar = SS7_PVAR_ETSI_93;
				else if (!strcmp(optarg, "SS7_PVAR_ETSI_96"))
					ss7_pvar = SS7_PVAR_ETSI_96;
				else if (!strcmp(optarg, "SS7_PVAR_ETSI_00"))
					ss7_pvar = SS7_PVAR_ETSI_00;
				else if (!strcmp(optarg, "SS7_PVAR_ANSI_92"))
					ss7_pvar = SS7_PVAR_ANSI_92;
				else if (!strcmp(optarg, "SS7_PVAR_ANSI_96"))
					ss7_pvar = SS7_PVAR_ANSI_96;
				else if (!strcmp(optarg, "SS7_PVAR_ANSI_00"))
					ss7_pvar = SS7_PVAR_ANSI_00;
				else if (!strcmp(optarg, "SS7_PVAR_JTTC_94"))
					ss7_pvar = SS7_PVAR_JTTC_94;
				else if (!strcmp(optarg, "SS7_PVAR_CHIN_00"))
					ss7_pvar = SS7_PVAR_CHIN_00;
				else if (!strcmp(optarg, "SS7_PVAR_SING_88"))
					ss7_pvar = SS7_PVAR_SING | SS7_PVAR_88;
				else if (!strcmp(optarg, "SS7_PVAR_SPAN_88"))
					ss7_pvar = SS7_PVAR_SPAN | SS7_PVAR_88;
				else
					goto bad_option;
			}
			break;
#if 0
		case 'D':	/* -D, --draft */
			m2pa_version = strtoul(optarg, NULL, 0);
			switch (m2pa_version) {
			case M2PA_VERSION_DRAFT3:
			case M2PA_VERSION_DRAFT3_1:
			case M2PA_VERSION_DRAFT4:
			case M2PA_VERSION_DRAFT4_1:
			case M2PA_VERSION_DRAFT4_9:
			case M2PA_VERSION_DRAFT5:
			case M2PA_VERSION_DRAFT5_1:
			case M2PA_VERSION_DRAFT6:
			case M2PA_VERSION_DRAFT6_1:
			case M2PA_VERSION_DRAFT6_9:
			case M2PA_VERSION_DRAFT7:
			case M2PA_VERSION_DRAFT9:
			case M2PA_VERSION_DRAFT10:
			case M2PA_VERSION_DRAFT11:
			case M2PA_VERSION_RFC4165:
				break;
			default:
				if (!strcmp(optarg, "M2PA_VERSION_DRAFT3"))
					m2pa_version = M2PA_VERSION_DRAFT3;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT3_1"))
					m2pa_version = M2PA_VERSION_DRAFT3_1;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT4"))
					m2pa_version = M2PA_VERSION_DRAFT4;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT4_1"))
					m2pa_version = M2PA_VERSION_DRAFT4_1;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT4_9"))
					m2pa_version = M2PA_VERSION_DRAFT4_9;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT5"))
					m2pa_version = M2PA_VERSION_DRAFT5;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT5_1"))
					m2pa_version = M2PA_VERSION_DRAFT5_1;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT6"))
					m2pa_version = M2PA_VERSION_DRAFT6;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT6_1"))
					m2pa_version = M2PA_VERSION_DRAFT6_1;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT6_9"))
					m2pa_version = M2PA_VERSION_DRAFT6_9;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT7"))
					m2pa_version = M2PA_VERSION_DRAFT7;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT9"))
					m2pa_version = M2PA_VERSION_DRAFT9;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT10"))
					m2pa_version = M2PA_VERSION_DRAFT10;
				else if (!strcmp(optarg, "M2PA_VERSION_DRAFT11"))
					m2pa_version = M2PA_VERSION_DRAFT11;
				else if (!strcmp(optarg, "M2PA_VERSION_RFC4165"))
					m2pa_version = M2PA_VERSION_RFC4165;
				else
					goto bad_option;
			}
			break;
#endif
		case 'c':	/* -c, --client */
			client_exec = 1;
			break;
		case 'S':	/* -S, --server */
			server_exec = 1;
			break;
		case 'a':	/* -a, --again */
			repeat_verbose = 1;
			break;
		case 'w':	/* -w, --wait */
			test_duration = INFINITE_WAIT;
			break;
#if 1
		case 'p':	/* -p, --client-port [PORT] */
			client_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[0] = ports[3];
			break;
		case 'P':	/* -P, --server-port [PORT] */
			server_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[1] = ports[3];
			ports[2] = ports[3] + 1;
			break;
		case 'i':	/* -i, --client-host [HOSTNAME[,HOSTNAME]*] */
			client_host_specified = 1;
			strncpy(hostbufc, optarg, HOST_BUF_LEN);
			hostc = hostbufc;
			break;
		case 'I':	/* -I, --server-host [HOSTNAME[,HOSTNAME]*] */
			server_host_specified = 1;
			strncpy(hostbufs, optarg, HOST_BUF_LEN);
			hosts = hostbufs;
			break;
#endif
		case 'r':	/* -r, --repeat */
			repeat_on_success = 1;
			repeat_on_failure = 1;
			break;
		case 'R':	/* -R, --repeat-fail */
			repeat_on_failure = 1;
			break;
		case 'd':	/* -d, --device [DEVICE] */
			if (optarg) {
				snprintf(devname, sizeof(devname), "%s", optarg);
				break;
			}
			goto bad_option;
#if 1
		case 'x':	/* -x, --transport [DEVICE] */
			if (optarg) {
				snprintf(xptname, sizeof(xptname), "%s", optarg);
				break;
			}
			goto bad_option;
#endif
		case 'e':	/* -e, --exit */
			exit_on_failure = 1;
			break;
		case 'l':	/* -l, --list [RANGE] */
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
		case 'f':	/* -f, --fast [SCALE] */
			if (optarg)
				timer_scale = atoi(optarg);
			else
				timer_scale = 50;
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", (long) timer_scale);
			break;
		case 's':	/* -s, --summary */
			summary = 1;
			break;
		case 'o':	/* -o, --onetest [TESTCASE] */
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
		case 'q':	/* -q, --quiet */
			verbose = 0;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 't':	/* -t, --tests [RANGE] */
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
		case 'm':	/* -m, --messages */
			show_msg = 1;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help, -?, --? */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
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
#if 0
	if (client_ppa_specified) {
	}
	if (server_ppa_specified) {
	}
#endif
#if 1
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
#endif
	exit(do_tests(tests_to_run));
}
