/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

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

#include <sys/xti.h>
#include <sys/xti_inet.h>
#include <sys/xti_sctp.h>

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
static const char *lstdname = "draft-bidulock-sigtran-m2ua-ss7test";
static const char *sstdname = "Q.781";
static const char *shortname = "M2UA";
#ifdef LFS
static char devname[256] = "/dev/streams/clone/sctp_t";
#else
static char devname[256] = "/dev/sctp_t";
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

static int client_exec = 0;		/* execute client side */
static int server_exec = 0;		/* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_fisus = 1;

//static int show_data = 1;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
static int last_prio = 0;

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
static int msu_len = MSU_LEN;

/*
   some globals for compressing events 
 */
static int oldact = 0;			/* previous action */
static int cntact = 0;			/* repeats of previous action */
static int oldevt = 0;
static int cntevt = 0;

static int count = 0;
static int tries = 0;

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

struct test_config {
	lmi_option_t opt;
	sdl_config_t sdl;
	sdt_config_t sdt;
	sl_config_t sl;
} iutconf = {
	{
		SS7_PVAR_ITUT_96,	/* pvar */
		    0,		/* popt */
	},			/* opt */
	{
		.ifname = NULL,	/* */
		    .ifflags = 0,	/* */
#if TEST_X400
		    .iftype = SDL_TYPE_DS0,	/* */
		    .ifrate = 64000,	/* */
		    .ifgtype = SDL_GTYPE_NONE,	/* */
		    .ifgrate = 0,	/* */
		    .ifmode = SDL_MODE_NONE,	/* */
#endif				/* TEST_X400 */
#if TEST_M2PA
		    .iftype = SDL_TYPE_PACKET,	/* */
		    .ifrate = 10000000,	/* */
		    .ifgtype = SDL_GTYPE_SCTP,	/* */
		    .ifgrate = 10000000,	/* */
		    .ifmode = SDL_MODE_PEER,	/* */
#endif				/* TEST_M2PA */
		    .ifgmode = SDL_GMODE_NONE,	/* */
		    .ifgcrc = SDL_GCRC_NONE,	/* */
		    .ifclock = SDL_CLOCK_NONE,	/* */
		    .ifcoding = SDL_CODING_NONE,	/* */
		    .ifframing = SDL_FRAMING_NONE,	/* */
		    .ifblksize = 0,	/* */
		    .ifleads = 0,	/* */
		    .ifbpv = 0,	/* */
		    .ifalarms = 0,	/* */
		    .ifrxlevel = 0,	/* */
		    .iftxlevel = 0,	/* */
		    .ifsync = 0,	/* */
	},			/* sdl */
	{
		.t8 = 100,	/* t8 - T8 timeout (milliseconds) */
		    .Tin = 4,	/* Tin - AERM normal proving threshold */
		    .Tie = 1,	/* Tie - AERM emergency proving threshold */
		    .T = 64,	/* T - SUERM error threshold */
		    .D = 256,	/* D - SUERM error rate parameter */
		    .Te = 577169,	/* Te - EIM error threshold */
		    .De = 9308000,	/* De - EIM correct decrement */
		    .Ue = 144292000,	/* Ue - EIM error increment */
		    .N = 16,	/* N */
		    .m = 272,	/* m */
		    .b = 64,	/* b */
		    .f = SDT_FLAGS_ONE,	/* f */
	},			/* sdt */
	{
		.t1 = 45 * 1000,	/* t1 - timer t1 duration (milliseconds) */
		    .t2 = 5 * 1000,	/* t2 - timer t2 duration (milliseconds) */
		    .t2l = 20 * 1000,	/* t2l - timer t2l duration (milliseconds) */
		    .t2h = 100 * 1000,	/* t2h - timer t2h duration (milliseconds) */
		    .t3 = 1 * 1000,	/* t3 - timer t3 duration (milliseconds) */
		    .t4n = 8 * 1000,	/* t4n - timer t4n duration (milliseconds) */
		    .t4e = 500 * 1000 / 1000,	/* t4e - timer t4e duration (milliseconds) */
		    .t5 = 125 * 1000 / 1000,	/* t5 - timer t5 duration (milliseconds) */
		    .t6 = 4 * 1000,	/* t6 - timer t6 duration (milliseconds) */
		    .t7 = 2 * 1000,	/* t7 - timer t7 duration (milliseconds) */
		    .rb_abate = 3,	/* rb_abate - RB cong abatement (#msgs) */
		    .rb_accept = 6,	/* rb_accept - RB cong onset accept (#msgs) */
		    .rb_discard = 9,	/* rb_discard - RB cong discard (#msgs) */
		    .tb_abate_1 = 128 * 272,	/* tb_abate_1 - lev 1 cong abate (#bytes) */
		    .tb_onset_1 = 256 * 272,	/* tb_onset_1 - lev 1 cong onset (#bytes) */
		    .tb_discd_1 = 384 * 272,	/* tb_discd_1 - lev 1 cong discard (#bytes) */
		    .tb_abate_2 = 512 * 272,	/* tb_abate_2 - lev 1 cong abate (#bytes) */
		    .tb_onset_2 = 640 * 272,	/* tb_onset_2 - lev 1 cong onset (#bytes) */
		    .tb_discd_2 = 768 * 272,	/* tb_discd_2 - lev 1 cong discard (#bytes) */
		    .tb_abate_3 = 896 * 272,	/* tb_abate_3 - lev 1 cong abate (#bytes) */
		    .tb_onset_3 = 1024 * 272,	/* tb_onset_3 - lev 1 cong onset (#bytes) */
		    .tb_discd_3 = 1152 * 272,	/* tb_discd_3 - lev 1 cong discard (#bytes) */
		    .N1 = 31,	/* N1 - PCR/RTBmax messages (#msg) */
		    .N2 = 8192,	/* N2 - PCR/RTBmax octets (#bytes) */
		    .M = 5	/* M - IAC normal proving periods */
} /* sl */ };

struct test_config *config = &iutconf;
struct test_stats  *stats = &iutstat;

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

static timer_range_t timer[tmax] = {
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

static int
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

static long beg_time = 0;

