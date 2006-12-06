/*****************************************************************************

 @(#) $RCSfile: test-x400p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/10/31 21:04:54 $

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

 Last Modified $Date: 2006/10/31 21:04:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-x400p.c,v $
 Revision 0.9.2.7  2006/10/31 21:04:54  brian
 - changes for 32-bit compatibility and remove HZ dependency

 Revision 0.9.2.6  2006/10/27 22:56:39  brian
 - changes for 32-bit compatibility

 Revision 0.9.2.5  2005/06/22 07:58:44  brian
 - unsigned/signed pointer corrections for gcc4 on FC4

 Revision 0.9.2.4  2005/05/14 08:31:40  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-x400p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/10/31 21:04:54 $"

static char const ident[] = "$RCSfile: test-x400p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/10/31 21:04:54 $";

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

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

/*
 *  The following test configuration is for use with a loopback cable between
 *  span 0 and span 1 on the first E/T400P-SS7 in a host.  The E1/T1 loopback
 *  cable is built as follows:
 *
 *
 *                  JACK 1                      JACK 2
 *            _________________             __________________
 *           | | | | | | | | | |           | | | | | | | | | |
 *           | | | | | | | | | |           | | | | | | | | | |
 *           | | | | | | | | | |           | | | | | | | | | |
 *           | | | | | | | | | |           | | | | | | | | | |
 *           | | | | | | | | | |           | | | | | | | | | |
 *           | | | | | | | | | |           | | | | | | | | | |
 *           |                 |           |                 |
 *           | 1 2 3 4 5 6 7 8 |           | 1 2 3 4 5 6 7 8 |
 *           | | |   | |       |           | | |   | |       |
 *           | | |   | |                     | |   | |
 *              X     X                       X     X
 *             | |   | |                     | |   | |
 *              X     X                       X     X
 *             | |   | |                     | |   | |
 *              X     X                       X     X
 *             | |   | |                     | |   | |
 *              X     X                       X     X
 *             | |   | |                     | |   | |
 *              X     X                       X     X
 *             | |   | |_ _ _ _ _ _ _ _ _ _ _| |   | |
 *              X    |___X_X_X_X_X_X_X_X_X_X___|    X
 *             | |                                 | |
 *              X                                   X
 *             | |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| |
 *             |___X_X_X_X_X_X_X_X_X_X_X_X_X_X_X_X___|
 *
 *
 *  The above arrangement will allow the settings below where the PT is on
 *  span 0 (Jack 1) and the IUT is no span 1 (Jack 2).  Channel 19 is used
 *  rather arbitrarily (its a big prime number).  This allows a single card to
 *  be used to test itself.
 */

#define PTU_TEST_SLOT	0
#define PTU_TEST_SPAN	0
#define PTU_TEST_CHAN	19

#define IUT_TEST_SLOT	0
#define IUT_TEST_SPAN	1
#define IUT_TEST_CHAN	19


/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "SS7 MTP Level 2";

/* static const char *spkgname = "SIGTRAN"; */
static const char *lstdname = "Q.703/ANSI T1.111.3/JQ.703";
static const char *sstdname = "Q.781";
static const char *shortname = "MTP2";
static char devname[256] = "/dev/x400p-sl";

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

#if 0
static int client_port_specified = 0;
static int server_port_specified = 0;
static int client_host_specified = 0;
static int server_host_specified = 0;
#endif

static int verbose = 1;

static int client_exec = 0;		/* execute client side */
static int server_exec = 0;		/* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_fisus = 1;
static int show_msus = 1;

//static int show_data = 1;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
static int PRIM_type = 0;
#if 0
static int NPI_error = 0;
static int CONIND_number = 2;
static int TOKEN_value = 0;
static int SEQ_number = 1;
static int SERV_type = N_CLNS;
static int CURRENT_state = NS_UNBND;
N_info_ack_t last_info = { 0, };
#endif
static int last_prio = 0;

#if 0
static int DATA_xfer_flags = 0;
static int BIND_flags = 0;
static int RESET_orig = N_UNDEFINED;
static int RESET_reason = 0;
static int DISCON_reason = 0;
static int CONN_flags = 0;
static int ERROR_type = 0;
static int RESERVED_field[2] = { 0, 0 };
#endif

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

#define BUFSIZE 32*4096

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	120000
#define INFINITE_WAIT	-1

static int test_duration = TEST_DURATION;	/* wait on other side */

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
#if 0
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
#else
static unsigned short addrs[3][1] = {
	{  (PTU_TEST_SLOT << 12) | (PTU_TEST_SPAN << 8) | (PTU_TEST_CHAN << 0) },
	{  (IUT_TEST_SLOT << 12) | (IUT_TEST_SPAN << 8) | (IUT_TEST_CHAN << 0) },
	{  (IUT_TEST_SLOT << 12) | (IUT_TEST_SPAN << 8) | (IUT_TEST_CHAN << 0) }
};
static int anums[3] = { 1, 1, 1 };
static unsigned short *ADDR_buffer = NULL;
static size_t ADDR_length = sizeof(unsigned short);
#endif
#if 0
static int test_resfd = -1;
#endif
static int test_timout = 200;
#if 0
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

#if 0
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
#endif

typedef unsigned short ppa_t;

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
	__TEST_NO_CONG, __TEST_CLEARB, __TEST_SYNC,
};

enum {
	__TEST_ATTACH_REQ = 400, __TEST_DETACH_REQ, __TEST_ENABLE_REQ, __TEST_ENABLE_CON,
	__TEST_DISABLE_REQ, __TEST_DISABLE_CON, __TEST_ERROR_IND, __TEST_SDL_OPTIONS,
	__TEST_SDL_CONFIG, __TEST_SDT_OPTIONS, __TEST_SDT_CONFIG, __TEST_SL_OPTIONS,
	__TEST_SL_CONFIG,
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static int ss7_pvar = SS7_PVAR_ITUT_96;

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
		    .iftype = SDL_TYPE_DS0,	/* */
		    .ifrate = 64000,	/* */
		    .ifgtype = SDL_GTYPE_E1,	/* */
		    .ifgrate = 2048000,	/* */
		    .ifmode = SDL_MODE_NONE,	/* */
		    .ifgmode = SDL_GMODE_NONE,	/* */
		    .ifgcrc = SDL_GCRC_CRC4,	/* */
		    .ifclock = SDL_CLOCK_NONE,	/* */
		    .ifcoding = SDL_CODING_HDB3,	/* */
		    .ifframing = SDL_FRAMING_CCS,	/* */
		    .ifblksize = 0,	/* */
		    .ifleads = 0,	/* */
		    .ifbpv = 0,	/* */
		    .ifalarms = 0,	/* */
		    .ifrxlevel = 0,	/* */
		    .iftxlevel = 1,	/* */
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
		    .f = 1,	/* f */
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
	{400, 600, "T4(Pe)"},	/* Timer T4e 50 */
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

static int event = 0;
static int tries = 0;
static int expand = 0;
static long beg_time = 0;

int pt_fd = 0;
unsigned char pt_fib = 0x80;
unsigned char pt_fsn = 0x7f;
unsigned char pt_bib = 0x80;
unsigned char pt_bsn = 0x7f;
unsigned char pt_li = 0;
unsigned char pt_sio = 0;

int iut_fd = 0;
unsigned char iut_fib = 0x80;
unsigned char iut_fsn = 0x7f;
unsigned char iut_bib = 0x80;
unsigned char iut_bsn = 0x7f;
unsigned char iut_li = 0;
unsigned char iut_sio = 0;
unsigned char iut_len = 0;

#if 0
static int oldpsb = 0;
#endif
static int oldmsg = 0;
static int cntmsg = 0;
static int oldisb = 0;
static int oldret = 0;
static int cntret = 0;
#if 0
static int oldprm = 0;
static int cntprm = 0;
#endif

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

#if 0
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
#endif

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
lmerrno_string(ulong lmerr, long uerr)
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

const char *
event_string(int child, int event)
{
	switch (child) {
	case CHILD_IUT:
		switch (event) {
		case __EVENT_IUT_OUT_OF_SERVICE:
			return ("!out of service");
		case __EVENT_IUT_IN_SERVICE:
			return ("!in service");
		case __EVENT_IUT_RPO:
			return ("!rpo");
		case __EVENT_IUT_RPR:
			return ("!rpr");
		case __EVENT_IUT_DATA:
			return ("!msu");
		case __TEST_ENABLE_CON:
			return ("!enable con");
		case __TEST_DISABLE_CON:
			return ("!disable con");
		case __TEST_OK_ACK:
			return ("!ok ack");
		case __TEST_ERROR_ACK:
			return ("!error ack");
		case __TEST_ERROR_IND:
			return ("!error ind");
		}
		break;
	default:
	case CHILD_PTU:
		switch (event) {
		case __STATUS_OUT_OF_SERVICE:
			return ("OUT-OF-SERVICE");
		case __STATUS_ALIGNMENT:
			return ("ALIGNMENT");
		case __STATUS_PROVING_NORMAL:
			return ("PROVING-NORMAL");
		case __STATUS_PROVING_EMERG:
			return ("PROVING-EMERGENCY");
		case __STATUS_IN_SERVICE:
			return ("IN-SERVICE");
		case __STATUS_PROCESSOR_OUTAGE:
			return ("PROCESSOR-OUTAGE");
		case __STATUS_PROCESSOR_ENDED:
			return ("PROCESSOR-ENDED");
		case __STATUS_BUSY:
			return ("BUSY");
		case __STATUS_BUSY_ENDED:
			return ("BUSY-ENDED");
		case __STATUS_SEQUENCE_SYNC:
			return ("READY");
		case __MSG_PROVING:
			return ("PROVING");
		case __TEST_ACK:
			return ("ACK");
		case __TEST_DATA:
			return ("DATA");
		case __TEST_SIO:
			return ("SIO");
		case __TEST_SIN:
			return ("SIN");
		case __TEST_SIE:
			return ("SIE");
		case __TEST_SIOS:
			return ("SIOS");
		case __TEST_SIPO:
			return ("SIPO");
		case __TEST_SIB:
			return ("SIB");
		case __TEST_SIX:
			return ("SIX");
		case __TEST_SIO2:
			return ("SIO2");
		case __TEST_SIN2:
			return ("SIN2");
		case __TEST_SIE2:
			return ("SIE2");
		case __TEST_SIOS2:
			return ("SIOS2");
		case __TEST_SIPO2:
			return ("SIPO2");
		case __TEST_SIB2:
			return ("SIB2");
		case __TEST_SIX2:
			return ("SIX2");
		case __TEST_FISU:
			return ("FISU");
		case __TEST_FISU_S:
			return ("FISU_S");
		case __TEST_FISU_BAD_FIB:
			return ("FISU_BAD_FIB");
		case __TEST_FISU_CORRUPT:
			return ("FISU_CORRUPT");
		case __TEST_FISU_CORRUPT_S:
			return ("FISU_CORRUPT_S");
		case __TEST_LSSU_CORRUPT:
			return ("LSSU_CORRUPT");
		case __TEST_LSSU_CORRUPT_S:
			return ("LSSU_CORRUPT_S");
		case __TEST_MSU:
			return ("MSU");
		case __TEST_MSU_SEVEN_ONES:
			return ("MSU_SEVEN_ONES");
		case __TEST_MSU_TOO_LONG:
			return ("MSU_TOO_LONG");
		case __TEST_MSU_TOO_SHORT:
			return ("MSU_TOO_SHORT");
		case __TEST_TX_BREAK:
			return ("TX_BREAK");
		case __TEST_TX_MAKE:
			return ("TX_MAKE");
		case __TEST_FISU_FISU_1FLAG:
			return ("FISU_FISU_1FLAG");
		case __TEST_FISU_FISU_2FLAG:
			return ("FISU_FISU_2FLAG");
		case __TEST_MSU_MSU_1FLAG:
			return ("MSU_MSU_1FLAG");
		case __TEST_MSU_MSU_2FLAG:
			return ("MSU_MSU_2FLAG");
		}
		break;
	}
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
	case __TEST_COUNT:
		return ("COUNT");
	case __TEST_TRIES:
		return ("TRIES");
	case __TEST_ETC:
		return ("ETC");
	case __TEST_SIB_S:
		return ("SIB_S");
	case __TEST_POWER_ON:
		return ("POWER_ON");
	case __TEST_START:
		return ("START");
	case __TEST_STOP:
		return ("STOP");
	case __TEST_LPO:
		return ("LPO");
	case __TEST_LPR:
		return ("LPR");
	case __TEST_EMERG:
		return ("EMERG");
	case __TEST_CEASE:
		return ("CEASE");
	case __TEST_SEND_MSU:
		return ("SEND_MSU");
	case __TEST_SEND_MSU_S:
		return ("SEND_MSU_S");
	case __TEST_CONG_A:
		return ("CONG_A");
	case __TEST_CONG_D:
		return ("CONG_D");
	case __TEST_NO_CONG:
		return ("NO_CONG");
	case __TEST_CLEARB:
		return ("CLEARB");
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

#if 1
void print_string(int child, const char *string);
void
print_ppa(int child, ppa_t *ppa) {
	char buf[32];

	snprintf(buf, sizeof(buf), "%d:%d:%d", ((*ppa) >> 12) & 0x0f, ((*ppa) >> 8) & 0x0f, (*ppa) & 0x0ff);
	print_string(child, buf);
}
#endif

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
#endif

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

const char *
prim_string(int prim)
{
	switch (prim) {
#if 0
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
#endif
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
#if 1
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
#endif
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
	default:
		return ("?_????_??? -----");
	}
}

#if 0
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
#endif

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

	if (show_timeout || verbose > 0) {
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

	if (show && verbose > 1)
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

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

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
		if (fsn[child] != 0x7f || bsn[child] != 0x7f || fib[child] != 0 || bib[child] != 0)
			print_tx_msg_sn(child, string, bib[child] | bsn[child], fib[child] | fsn[child]);
		else
			print_string_state(child, msgs, string);
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
		if (fsn[other] != 0x7f || bsn[other] != 0x7f || fib[other] != 0 || bib[other] != 0)
			print_rx_msg_sn(child, string, bib[other] | bsn[other], fib[other] | fsn[other]);
		else
			print_string_state(child, msgs, string);
	}
}

void
print_ack_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s-/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-%16s> [%d:%03d]\n",
		"                    |                                |\\-+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
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
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ /|/ / / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 0 && show) {
		float delay;

		delay = time->tv_nsec;
		delay = delay / 1000000000;
		delay = delay + time->tv_sec;
		print_float_state(child, msgs, delay);
	}
}

#if 0
void
print_mgmtflag(int child, np_ulong flag)
{
	print_string(child, mgmtflag_string(flag));
}

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

#if 0
void
print_info(int child, N_info_ack_t * info)
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
#endif

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
			fprintf(stdout, "%02X", ctrl->buf[i]);
		fprintf(stdout, "]\n");
		fprintf(stdout, "[");
		for (i = 0; i < (data ? data->len : 0); i++)
			fprintf(stdout, "%02X", data->buf[i]);
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

#if 0
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
#endif

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
	switch (child) {
	case 1:
	case 2:
	case 0:
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

static long old_test_duration = 0;

static int
begin_sanity(int index)
{
	old_test_duration = test_duration;
	test_duration = 5000;
	return begin_tests(index);
}

static int
end_sanity(int index)
{
	test_duration = old_test_duration;
	return end_tests(index);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Injected event encoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

union primitives {
	lmi_ulong prim;
	union LMI_primitives lmi;
	union SDL_primitives sdl;
	union SDT_primitives sdt;
	union SL_primitives sl;
};

static int pt_config(void);

static int
do_signal(int child, int action)
{
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	union primitives *p = (typeof(p)) cbuf;
	struct strioctl ic;

	if (action != oldact) {
		oldact = action;
		show = 1;
		if (verbose > 1) {
			if (cntact > 0) {
				char buf[64];

				snprintf(buf, sizeof(buf), "Ct=%5d", cntact + 1);
				print_command_state(child, buf);
			}
		}
		cntact = 0;
	} else if (!show)
		cntact++;

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
#if 1
	case __TEST_SIO:
		if (!cntmsg)
			print_tx_msg(child, "SIO");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIO;
		data->len = 4;
		goto send_message;
	case __TEST_SIN:
		if (!cntmsg)
			print_tx_msg(child, "SIN");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIN;
		data->len = 4;
		goto send_message;
	case __TEST_SIE:
		if (!cntmsg)
			print_tx_msg(child, "SIE");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIE;
		data->len = 4;
		goto send_message;
	case __TEST_SIOS:
		if (!cntmsg)
			print_tx_msg(child, "SIOS");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIOS;
		data->len = 4;
		goto send_message;
	case __TEST_SIPO:
		if (!cntmsg)
			print_tx_msg(child, "SIPO");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIPO;
		data->len = 4;
		goto send_message;
	case __TEST_SIB:
		if (!cntmsg)
			print_tx_msg(child, "SIB");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIB;
		data->len = 4;
		goto send_message;
	case __TEST_SIX:
		if (!cntmsg)
			print_tx_msg(child, "LSSU(invalid)");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = 6;
		data->len = 4;
		goto send_message;
	case __TEST_SIO2:
		if (!cntmsg)
			print_tx_msg(child, "SIO[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIO;
		data->len = 5;
		goto send_message;
	case __TEST_SIN2:
		if (!cntmsg)
			print_tx_msg(child, "SIN[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIN;
		data->len = 5;
		goto send_message;
	case __TEST_SIE2:
		if (!cntmsg)
			print_tx_msg(child, "SIE[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIE;
		data->len = 5;
		goto send_message;
	case __TEST_SIOS2:
		if (!cntmsg)
			print_tx_msg(child, "SIOS[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIOS;
		data->len = 5;
		goto send_message;
	case __TEST_SIPO2:
		if (!cntmsg)
			print_tx_msg(child, "SIPO[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIPO;
		data->len = 5;
		goto send_message;
	case __TEST_SIB2:
		if (!cntmsg)
			print_tx_msg(child, "SIB[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = LSSU_SIB;
		data->len = 5;
		goto send_message;
	case __TEST_SIX2:
		if (!cntmsg)
			print_tx_msg(child, "LSSU(invalid)[2]");
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 2;
		dbuf[3] = 0;
		dbuf[4] = 6;
		data->len = 5;
		goto send_message;
	case __TEST_SIB_S:
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 1;
		dbuf[3] = LSSU_SIB;
		data->len = 4;
		goto send_message;
	case __TEST_FISU:
		if (!cntmsg)
			print_tx_msg(child, "FISU");
	case __TEST_FISU_S:
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 0;
		data->len = 3;
		goto send_message;
	case __TEST_FISU_BAD_FIB:
		if (!cntmsg)
			print_tx_msg_sn(child, "FISU(bad fib)", bib[child]|bsn[child], (fib[child]|fsn[child])^0x80);
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = (fib[child] | fsn[child]) ^ 0x80;
		dbuf[2] = 0;
		data->len = 3;
		goto send_message;
	case __TEST_LSSU_CORRUPT:
		if (!cntmsg)
			print_tx_msg(child, "LSSU(corrupt)");
	case __TEST_LSSU_CORRUPT_S:
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 0xff;
		dbuf[3] = 0xff;
		data->len = 4;
		goto send_message;
	case __TEST_FISU_CORRUPT:
		if (!cntmsg)
			print_tx_msg(child, "FISU(corrupt)");
	case __TEST_FISU_CORRUPT_S:
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 0xff;
		data->len = 3;
		goto send_message;
	case __TEST_MSU:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		fsn[child] = (fsn[child] + 1) & 0x7f;
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = msu_len;
		memset(&dbuf[3], 'B', msu_len);
		data->len = msu_len + 3;
		if (!cntmsg)
			print_tx_msg(child, "MSU");
		goto send_message;
	case __TEST_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		fsn[child] = (fsn[child] + 1) & 0x7f;
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = msu_len;
		memset(&dbuf[3], 'B', msu_len);
		data->len = msu_len + 3;
		goto send_message;
	case __TEST_MSU_TOO_LONG:
		fsn[child] = (fsn[child] + 1) & 0x7f;
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		dbuf[2] = 63;
		memset(&dbuf[3], 'A', 280);
		data->len = 283;
		if (!cntmsg)
			print_tx_msg(child, "MSU(too long)");
		goto send_message;
	case __TEST_MSU_SEVEN_ONES:
		msu_len = 256;
		fsn[child] = (fsn[child] + 1) & 0x7f;
		if (!cntmsg)
			print_tx_msg(child, "MSU(7 ones)");
		fsn[child] = (fsn[child] - 1) & 0x7f;
		{
			struct strioctl ctl;

			ctl.ic_cmd = SDT_IOCCABORT;
			ctl.ic_timout = 0;
			ctl.ic_len = 0;
			ctl.ic_dp = NULL;
			do_signal(child, __TEST_MSU_S);
			if (ioctl(test_fd[child], I_STR, &ctl) < 0)
				return __RESULT_INCONCLUSIVE;
		}
		return __RESULT_SUCCESS;
	case __TEST_MSU_TOO_SHORT:
		fsn[child] = (fsn[child] + 1) & 0x7f;
		dbuf[0] = bib[child] | bsn[child];
		dbuf[1] = fib[child] | fsn[child];
		data->len = 2;
		if (!cntmsg)
			print_tx_msg(child, "MSU(too short)");
		goto send_message;
	case __TEST_FISU_FISU_1FLAG:
		print_tx_msg(child, "FISU-F-FISU");
		do_signal(child, __TEST_FISU_S);
		do_signal(child, __TEST_FISU_S);
		return __RESULT_SUCCESS;
	case __TEST_FISU_FISU_2FLAG:
		print_tx_msg(child, "FISU-F-F-FISU");
		config->sdt.f = SDT_FLAGS_TWO;
		if (pt_config() != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		do_signal(child, __TEST_FISU_S);
		do_signal(child, __TEST_FISU_S);
		config->sdt.f = SDT_FLAGS_ONE;
		if (pt_config() != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		return __RESULT_SUCCESS;
	case __TEST_MSU_MSU_1FLAG:
		print_tx_msg(child, "MSU-F-MSU");
		do_signal(child, __TEST_MSU_S);
		do_signal(child, __TEST_MSU_S);
		return __RESULT_SUCCESS;
	case __TEST_MSU_MSU_2FLAG:
		print_tx_msg(child, "MSU-F-F-MSU");
		config->sdt.f = SDT_FLAGS_TWO;
		if (pt_config() != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		do_signal(child, __TEST_MSU_S);
		do_signal(child, __TEST_MSU_S);
		config->sdt.f = SDT_FLAGS_ONE;
		if (pt_config() != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		return __RESULT_SUCCESS;
	      send_message:
		data->maxlen = BUFSIZE;
		data->len = data->len;
		data->buf = dbuf;
		ctrl->maxlen = BUFSIZE;
		ctrl->len = sizeof(p->sdt.daedt_transmission_req);
		ctrl->buf = cbuf;
		p->sdt.daedt_transmission_req.sdt_primitive = SDT_DAEDT_TRANSMISSION_REQ;
		return test_putmsg(child, ctrl, data, 0);
#endif
	case __TEST_POWER_ON:

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
		return test_push(child, "m2pa-sl");
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

#if 0
	case __TEST_CONN_REQ:
		ctrl->len = sizeof(p->npi.conn_req)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->npi.conn_req.PRIM_type = N_CONN_REQ;
		p->npi.conn_req.DEST_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.conn_req.DEST_offset = ADDR_buffer ? sizeof(p->npi.conn_req) : 0;
		p->npi.conn_req.CONN_flags = CONN_flags;
		p->npi.conn_req.QOS_length = QOS_buffer ? QOS_length : 0;
		p->npi.conn_req.QOS_offset = QOS_buffer ? sizeof(p->npi.conn_req) + p->npi.conn_req.DEST_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.conn_req.DEST_offset, p->npi.conn_req.DEST_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->npi.conn_req.QOS_offset, p->npi.conn_req.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->npi.conn_req.DEST_offset, p->npi.conn_req.DEST_length));
#else
		print_addrs(child, cbuf + p->npi.conn_req.DEST_offset, p->npi.conn_req.DEST_length);
#endif
		print_options(child, cbuf, p->npi.conn_req.QOS_offset, p->npi.conn_req.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_IND:
		ctrl->len = sizeof(p->npi.conn_ind)
		    + (DEST_buffer ? DEST_length : 0)
		    + (SRC_buffer ? SRC_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->npi.conn_ind.PRIM_type = N_CONN_IND;
		p->npi.conn_ind.DEST_length = DEST_buffer ? DEST_length : 0;
		p->npi.conn_ind.DEST_offset = DEST_buffer ? sizeof(p->npi.conn_ind) : 0;
		p->npi.conn_ind.SRC_length = SRC_buffer ? SRC_length : 0;
		p->npi.conn_ind.SRC_offset = SRC_buffer ? sizeof(p->npi.conn_ind) + p->npi.conn_ind.DEST_length : 0;
		p->npi.conn_ind.SEQ_number = SEQ_number;
		p->npi.conn_ind.CONN_flags = CONN_flags;
		p->npi.conn_ind.QOS_length = QOS_buffer ? QOS_length : 0;
		p->npi.conn_ind.QOS_offset = QOS_buffer ? sizeof(p->npi.conn_ind) + p->npi.conn_ind.DEST_length + p->npi.conn_ind.SRC_length : 0;
		if (DEST_buffer)
			bcopy(DEST_buffer, ctrl->buf + p->npi.conn_ind.DEST_offset, p->npi.conn_ind.DEST_length);
		if (SRC_buffer)
			bcopy(SRC_buffer, ctrl->buf + p->npi.conn_ind.SRC_offset, p->npi.conn_ind.SRC_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->npi.conn_ind.QOS_offset, p->npi.conn_ind.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_addrs(child, cbuf + p->npi.conn_ind.DEST_offset, p->npi.conn_ind.DEST_length);
		print_addrs(child, cbuf + p->npi.conn_ind.SRC_offset, p->npi.conn_ind.SRC_length);
		print_options(child, cbuf, p->npi.conn_ind.QOS_offset, p->npi.conn_ind.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_RES:
		ctrl->len = sizeof(p->npi.conn_res)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->npi.conn_res.PRIM_type = N_CONN_RES;
		p->npi.conn_res.TOKEN_value = TOKEN_value;
		p->npi.conn_res.RES_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.conn_res.RES_offset = ADDR_buffer ? sizeof(p->npi.conn_res) : 0;
		p->npi.conn_res.CONN_flags = CONN_flags;
		p->npi.conn_res.QOS_length = QOS_buffer ? QOS_length : 0;
		p->npi.conn_res.QOS_offset = QOS_buffer ? sizeof(p->npi.conn_res) + p->npi.conn_res.RES_length : 0;
		p->npi.conn_res.SEQ_number = SEQ_number;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.conn_res.RES_offset, p->npi.conn_res.RES_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->npi.conn_res.QOS_offset, p->npi.conn_res.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_options(child, cbuf, p->npi.conn_res.QOS_offset, p->npi.conn_res.QOS_length);
		if (test_resfd == -1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		else
			return test_insertfd(child, test_resfd, 4, ctrl, data, 0);
	case __TEST_CONN_CON:
		ctrl->len = sizeof(p->npi.conn_con);
		p->npi.conn_con.PRIM_type = N_CONN_CON;
		p->npi.conn_con.RES_length = 0;
		p->npi.conn_con.RES_offset = 0;
		p->npi.conn_con.CONN_flags = 0;
		p->npi.conn_con.QOS_length = 0;
		p->npi.conn_con.QOS_offset = 0;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Connection confirmation test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_addrs(child, cbuf + p->npi.conn_con.RES_offset, p->npi.conn_con.RES_length);
		print_options(child, cbuf, p->npi.conn_con.QOS_offset, p->npi.conn_con.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_REQ:
		ctrl->len = sizeof(p->npi.discon_req)
		    + (ADDR_buffer ? ADDR_length : 0);
		p->npi.discon_req.PRIM_type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = DISCON_reason;
		p->npi.discon_req.RES_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.discon_req.RES_offset = ADDR_buffer ? sizeof(p->npi.discon_req) : 0;
		p->npi.discon_req.SEQ_number = SEQ_number;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.discon_req.RES_offset, p->npi.discon_req.RES_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_IND:
		ctrl->len = sizeof(p->npi.discon_ind);
		p->npi.discon_ind.PRIM_type = N_DISCON_IND;
		p->npi.discon_ind.DISCON_orig = 0;
		p->npi.discon_ind.DISCON_reason = 0;
		p->npi.discon_ind.RES_length = 0;
		p->npi.discon_ind.RES_offset = 0;
		p->npi.discon_ind.SEQ_number = SEQ_number;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Disconnection indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATACK_REQ:
		ctrl->len = sizeof(p->npi.datack_req) + QOS_length;
		p->npi.datack_req.PRIM_type = N_DATACK_REQ;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->npi.datack_req), QOS_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATACK_IND:
		ctrl->len = sizeof(p->npi.datack_ind);
		p->npi.datack_ind.PRIM_type = N_DATACK_IND;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_REQ:
		ctrl->len = sizeof(p->npi.data_req) + QOS_length;
		p->npi.data_req.PRIM_type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = DATA_xfer_flags;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->npi.data_req), QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_IND:
		ctrl->len = sizeof(p->npi.data_ind);
		p->npi.data_ind.PRIM_type = N_DATA_IND;
		p->npi.data_ind.DATA_xfer_flags = DATA_xfer_flags;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Normal test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_REQ:
		ctrl->len = sizeof(p->npi.exdata_req) + QOS_length;
		p->npi.exdata_req.PRIM_type = N_EXDATA_REQ;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->npi.exdata_req), QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_IND:
		ctrl->len = sizeof(p->npi.exdata_ind);
		p->npi.data_ind.PRIM_type = N_EXDATA_IND;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Expedited test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_REQ:
		ctrl->len = sizeof(p->npi.info_req);
		p->npi.info_req.PRIM_type = N_INFO_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_ACK:
		ctrl->len = sizeof(p->npi.info_ack);
		p->npi.info_ack.PRIM_type = N_INFO_ACK;
		p->npi.info_ack.NSDU_size = test_bufsize;
		p->npi.info_ack.ENSDU_size = test_bufsize;
		p->npi.info_ack.CDATA_size = test_bufsize;
		p->npi.info_ack.DDATA_size = test_bufsize;
		p->npi.info_ack.ADDR_size = test_bufsize;
		p->npi.info_ack.NIDU_size = test_nidu;
		p->npi.info_ack.SERV_type = SERV_type;
		p->npi.info_ack.CURRENT_state = CURRENT_state;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_info(child, &p->npi.info_ack);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_REQ:
		ctrl->len = sizeof(p->npi.bind_req)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (PROTOID_buffer ? PROTOID_length : 0);
		p->npi.bind_req.PRIM_type = N_BIND_REQ;
		p->npi.bind_req.ADDR_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.bind_req.ADDR_offset = ADDR_buffer ? sizeof(p->npi.bind_req) : 0;
		p->npi.bind_req.CONIND_number = CONIND_number;
		p->npi.bind_req.BIND_flags = BIND_flags;
		p->npi.bind_req.PROTOID_length = PROTOID_buffer ? PROTOID_length : 0;
		p->npi.bind_req.PROTOID_offset = PROTOID_buffer ? sizeof(p->npi.bind_req) + p->npi.bind_req.ADDR_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.bind_req.ADDR_offset, p->npi.bind_req.ADDR_length);
		if (PROTOID_buffer)
			bcopy(PROTOID_buffer, ctrl->buf + p->npi.bind_req.PROTOID_offset, p->npi.bind_req.PROTOID_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_prots(child, cbuf + p->npi.bind_ack.PROTOID_offset, p->npi.bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length);
#endif
		if (show && verbose > 3) {
			char buf[64];

			snprintf(buf, sizeof(buf), "CONIND_number = %d", CONIND_number);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), "BIND_flags = %x", BIND_flags);
			print_string(child, buf);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_ACK:
		ctrl->len = sizeof(p->npi.bind_ack)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (PROTOID_buffer ? PROTOID_length : 0);
		p->npi.bind_ack.PRIM_type = N_BIND_ACK;
		p->npi.bind_ack.ADDR_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.bind_ack.ADDR_offset = ADDR_buffer ? sizeof(p->npi.bind_ack) : 0;
		p->npi.bind_ack.CONIND_number = CONIND_number;
		p->npi.bind_ack.TOKEN_value = TOKEN_value;
		p->npi.bind_ack.PROTOID_length = PROTOID_buffer ? PROTOID_length : 0;
		p->npi.bind_ack.PROTOID_offset = PROTOID_buffer ? sizeof(p->npi.bind_ack) + p->npi.bind_ack.ADDR_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length);
		if (PROTOID_buffer)
			bcopy(PROTOID_buffer, ctrl->buf + p->npi.bind_ack.PROTOID_offset, p->npi.bind_ack.PROTOID_length);
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_prots(child, cbuf + p->npi.bind_ack.PROTOID_offset, p->npi.bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length);
#endif
		if (show && verbose > 3) {
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNBIND_REQ:
		ctrl->len = sizeof(p->npi.unbind_req);
		p->npi.unbind_req.PRIM_type = N_UNBIND_REQ;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ERROR_ACK:
		ctrl->len = sizeof(p->npi.error_ack);
		p->npi.error_ack.PRIM_type = N_ERROR_ACK;
		p->npi.error_ack.ERROR_prim = PRIM_type;
		p->npi.error_ack.NPI_error = NPI_error;
		p->npi.error_ack.UNIX_error = last_errno;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_string(child, nerrno_string(p->npi.error_ack.NPI_error, p->npi.error_ack.UNIX_error));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OK_ACK:
		ctrl->len = sizeof(p->npi.ok_ack);
		p->npi.ok_ack.PRIM_type = N_OK_ACK;
		p->npi.ok_ack.CORRECT_prim = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_REQ:
		ctrl->len = sizeof(p->npi.unitdata_req)
		    + (ADDR_buffer ? ADDR_length : 0);
		p->npi.unitdata_req.PRIM_type = N_UNITDATA_REQ;
		p->npi.unitdata_req.DEST_length = ADDR_buffer ? ADDR_length : 0;
		p->npi.unitdata_req.DEST_offset = ADDR_buffer ? sizeof(p->npi.unitdata_req) : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->npi.unitdata_req.DEST_offset, p->npi.unitdata_req.DEST_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->npi.unitdata_req.DEST_offset, p->npi.unitdata_req.DEST_length));
#else
		print_addrs(child, cbuf + p->npi.unitdata_req.DEST_offset, p->npi.unitdata_req.DEST_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_IND:
		ctrl->len = sizeof(p->npi.unitdata_ind);
		p->npi.unitdata_ind.PRIM_type = N_UNITDATA_IND;
		p->npi.unitdata_ind.SRC_length = 0;
		p->npi.unitdata_ind.SRC_offset = 0;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Unit test data indication.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->npi.unitdata_ind.SRC_offset, p->npi.unitdata_ind.SRC_length));
#else
		print_addrs(child, cbuf + p->npi.unitdata_ind.SRC_offset, p->npi.unitdata_ind.SRC_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UDERROR_IND:
		ctrl->len = sizeof(p->npi.uderror_ind);
		p->npi.uderror_ind.PRIM_type = N_UDERROR_IND;
		p->npi.uderror_ind.DEST_length = 0;
		p->npi.uderror_ind.DEST_offset = 0;
		p->npi.uderror_ind.ERROR_type = 0;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_REQ:
		ctrl->len = sizeof(p->npi.optmgmt_req)
		    + (QOS_buffer ? QOS_length : 0);
		p->npi.optmgmt_req.PRIM_type = N_OPTMGMT_REQ;
		p->npi.optmgmt_req.QOS_length = QOS_buffer ? QOS_length : 0;
		p->npi.optmgmt_req.QOS_offset = QOS_buffer ? sizeof(p->npi.optmgmt_req) : 0;
		p->npi.optmgmt_req.OPTMGMT_flags = OPTMGMT_flags;
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->npi.optmgmt_req.QOS_offset, p->npi.optmgmt_req.QOS_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_mgmtflag(child, p->npi.optmgmt_req.OPTMGMT_flags);
		print_options(child, cbuf, p->npi.optmgmt_req.QOS_offset, p->npi.optmgmt_req.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_REQ:
		ctrl->len = sizeof(p->npi.reset_req);
		p->npi.reset_req.PRIM_type = N_RESET_REQ;
		p->npi.reset_req.RESET_reason = RESET_reason;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_reset_reason(child, p->npi.reset_req.RESET_reason);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_IND:
		ctrl->len = sizeof(p->npi.reset_ind);
		p->npi.reset_ind.PRIM_type = N_RESET_IND;
		p->npi.reset_ind.RESET_orig = RESET_orig;
		p->npi.reset_ind.RESET_reason = RESET_reason;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_reset_orig(child, p->npi.reset_ind.RESET_orig);
		print_reset_reason(child, p->npi.reset_ind.RESET_reason);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_RES:
		ctrl->len = sizeof(p->npi.reset_res);
		p->npi.reset_res.PRIM_type = N_RESET_RES;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_CON:
		ctrl->len = sizeof(p->npi.reset_con);
		p->npi.reset_con.PRIM_type = N_RESET_CON;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_TOO_SHORT:
		ctrl->len = sizeof(p->npi.type);
		p->npi.type = PRIM_type;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_WAY_TOO_SHORT:
		ctrl->len = sizeof(p->npi.type) >> 1;
		p->npi.type = PRIM_type;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
#endif

/*
 *  The following group cannot really be performed on the PT (child == 1) and
 *  they are only used to print the messages that would appear if the PT were
 *  functioning similar to the IUT.
 */

	case __TEST_POWER_ON:
		ctrl->len = sizeof(p->sl.power_on_req);
		p->sl.power_on_req.sl_primitive = SL_POWER_ON_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":power on");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_START:
		ctrl->len = sizeof(p->sl.start_req);
		p->sl.start_req.sl_primitive = SL_START_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":start");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_STOP:
		ctrl->len = sizeof(p->sl.stop_req);
		p->sl.stop_req.sl_primitive = SL_STOP_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":stop");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_LPO:
		ctrl->len = sizeof(p->sl.local_proc_outage_req);
		p->sl.local_proc_outage_req.sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":set lpo");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_LPR:
		ctrl->len = sizeof(p->sl.resume_req);
		p->sl.resume_req.sl_primitive = SL_RESUME_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear lpo");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CONG_A:
		ctrl->len = sizeof(p->sl.cong_accept_req);
		p->sl.cong_accept_req.sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":make congested");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CONG_D:
		ctrl->len = sizeof(p->sl.cong_discard_req);
		p->sl.cong_discard_req.sl_primitive = SL_CONGESTION_DISCARD_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":make congested");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_NO_CONG:
		ctrl->len = sizeof(p->sl.no_cong_req);
		p->sl.no_cong_req.sl_primitive = SL_NO_CONGESTION_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear congested");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_EMERG:
		ctrl->len = sizeof(p->sl.emergency_req);
		p->sl.emergency_req.sl_primitive = SL_EMERGENCY_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":set emerg");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CEASE:
		ctrl->len = sizeof(p->sl.emergency_ceases_req);
		p->sl.emergency_ceases_req.sl_primitive = SL_EMERGENCY_CEASES_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear emerg");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CLEARB:
		ctrl->len = sizeof(p->sl.clear_buffers_req);
		p->sl.clear_buffers_req.sl_primitive = SL_CLEAR_BUFFERS_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear buffers");
		if (child == 1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;

	case __TEST_COUNT:
	{
		char buf[64];

		snprintf(buf, sizeof(buf), "Ct = %5d", count);
		print_string(child, buf);
		return __RESULT_SUCCESS;
	}
	case __TEST_TRIES:
	{
		char buf[64];

		snprintf(buf, sizeof(buf), "%d iterations", tries);
		print_string(child, buf);
		return __RESULT_SUCCESS;
	}
	case __TEST_ETC:
		print_string(child, ".");
		print_string(child, ".");
		print_string(child, ".");
		return __RESULT_SUCCESS;

	case __TEST_SEND_MSU:
	case __TEST_SEND_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		print_command_state(child, ":msu");
		ctrl->len = sizeof(p->sl.pdu_req);
		p->sl.pdu_req.sl_primitive = SL_PDU_REQ;
		data->len = msu_len;
		memset(dbuf, 'B', msu_len);
		test_pflags = MSG_BAND;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);

#if 0
	case __STATUS_ALIGNMENT:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_ALIGNMENT);
	case __STATUS_PROVING_NORMAL:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_PROVING_NORMAL);
	case __STATUS_PROVING_EMERG:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_PROVING_EMERGENCY);
	case __STATUS_OUT_OF_SERVICE:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_OUT_OF_SERVICE);
	case __STATUS_IN_SERVICE:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_IN_SERVICE);
	case __STATUS_SEQUENCE_SYNC:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			qos[child].data.sid = 0;
			return test_m2pa_status(child, M2PA_STATUS_IN_SERVICE);
		default:
			return __RESULT_SUCCESS;
		}
	case __STATUS_PROCESSOR_OUTAGE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			qos[child].data.sid = 1;
			break;
		default:
			qos[child].data.sid = 0;
			break;
		}
		return test_m2pa_status(child, M2PA_STATUS_PROCESSOR_OUTAGE);
	case __STATUS_PROCESSOR_ENDED:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT9:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			qos[child].data.sid = 1;
			break;
		default:
			qos[child].data.sid = 0;
			break;
		}
		return test_m2pa_status(child, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED);
	case __STATUS_BUSY:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_BUSY);
	case __STATUS_BUSY_ENDED:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_BUSY_ENDED);
	case __STATUS_INVALID_STATUS:
		qos[child].data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_INVALID);

	case __TEST_MSU_TOO_SHORT:
		msu_len = 1;
		return test_m2pa_data(child);
	case __TEST_DATA:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		return test_m2pa_data(child);

	case __TEST_BAD_ACK:
		fsn[child] = 0xffff;
		/* fall through */
	case __TEST_ACK:
		return test_m2pa_ack(child);
#endif
	case __TEST_TX_BREAK:
		print_command_state(child, ":break Tx");
		ic.ic_cmd = SDL_IOCCDISCTX;
		ic.ic_timout = 0;
		ic.ic_len = 0;
		ic.ic_dp = NULL;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
		return __RESULT_FAILURE;
	case __TEST_TX_MAKE:
		print_command_state(child, ":reconnect Tx");
		ic.ic_cmd = SDL_IOCCCONNTX;
		ic.ic_timout = 0;
		ic.ic_len = 0;
		ic.ic_dp = NULL;
		return test_ioctl(child, I_STR, (intptr_t) &ic);

	case __TEST_SDL_OPTIONS:
		if (show && verbose > 1)
			print_command_state(child, ":options sdl");
		ic.ic_cmd = SDL_IOCSOPTIONS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->opt);
		ic.ic_dp = (char *) &config->opt;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_SDL_CONFIG:
		if (show && verbose > 1)
			print_command_state(child, ":config sdl");
		ic.ic_cmd = SDL_IOCSCONFIG;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->sdl);
		ic.ic_dp = (char *) &config->sdl;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_SDT_OPTIONS:
		if (show && verbose > 1)
			print_command_state(child, ":options sdt");
		ic.ic_cmd = SDT_IOCSOPTIONS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->opt);
		ic.ic_dp = (char *) &config->opt;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_SDT_CONFIG:
		if (show && verbose > 1)
			print_command_state(child, ":config sdt");
		ic.ic_cmd = SDT_IOCSCONFIG;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->sdt);
		ic.ic_dp = (char *) &config->sdt;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_SL_OPTIONS:
		if (show && verbose > 1)
			print_command_state(child, ":options sl");
		ic.ic_cmd = SL_IOCSOPTIONS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->opt);
		ic.ic_dp = (char *) &config->opt;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_SL_CONFIG:
		if (show && verbose > 1)
			print_command_state(child, ":config sl");
		ic.ic_cmd = SL_IOCSCONFIG;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(config->sl);
		ic.ic_dp = (char *) &config->sl;
		return test_ioctl(child, I_STR, (intptr_t) &ic);

	case __TEST_ATTACH_REQ:
		ctrl->len = sizeof(p->lmi.attach_req) + (ADDR_buffer ? ADDR_length : 0);
		p->lmi.attach_req.lmi_primitive = LMI_ATTACH_REQ;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + sizeof(p->lmi.attach_req), ADDR_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if (show && verbose > 1) {
			print_command_state(child, ":attach");
#if 0
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + sizeof(p->lmi.attach_req), ADDR_length));
#else
			print_addrs(child, cbuf + sizeof(p->lmi.attach_req), ADDR_length);
#endif
#else
			print_ppa(child, (ppa_t *)p->lmi.attach_req.lmi_ppa);
#endif
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DETACH_REQ:
		ctrl->len = sizeof(p->lmi.detach_req);
		p->lmi.detach_req.lmi_primitive = LMI_DETACH_REQ;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ":detach");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ENABLE_REQ:
		ctrl->len = sizeof(p->lmi.enable_req) + (ADDR_buffer ? ADDR_length : 0);
		p->lmi.enable_req.lmi_primitive = LMI_ENABLE_REQ;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + sizeof(p->lmi.enable_req), ADDR_length);
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1) {
			print_command_state(child, ":enable");
#if 0
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + sizeof(p->lmi.enable_req), ADDR_length));
#else
			print_addrs(child, cbuf + sizeof(p->lmi.enable_req), ADDR_length);
#endif
#else
			print_ppa(child, (ppa_t *)p->lmi.enable_req.lmi_rem);
#endif
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISABLE_REQ:
		ctrl->len = sizeof(p->lmi.disable_req);
		p->lmi.disable_req.lmi_primitive = LMI_DISABLE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ":disable");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);

#if 0
	case __TEST_BIND_REQ:
		ctrl->len = sizeof(p->npi.bind_req) + anums[child] * sizeof(addrs[child][0]);
		data = NULL;
		p->npi.bind_req.PRIM_type = N_BIND_REQ;
		p->npi.bind_req.ADDR_length = anums[child] * sizeof(addrs[child][0]);
		p->npi.bind_req.ADDR_offset = sizeof(p->npi.bind_req);
		p->npi.bind_req.CONIND_number = (child == 2) ? 2 : 0;
		p->npi.bind_req.BIND_flags = TOKEN_REQUEST;
		p->npi.bind_req.PROTOID_length = 0;
		p->npi.bind_req.PROTOID_offset = 0;
		bcopy(&addrs[child], (&p->npi.bind_req + 1), anums[child] * sizeof(addrs[child][0]));
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ">bind");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_REQ:
		ctrl->len = sizeof(p->npi.conn_req) + sizeof(qos[child].conn);
		data = NULL;
		p->npi.conn_req.PRIM_type = N_CONN_REQ;
		p->npi.conn_req.DEST_length = anums[(child + 1) % 2] * sizeof(addrs[child][0]);
		p->npi.conn_req.DEST_offset = sizeof(p->npi.conn_req);
		p->npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->npi.conn_req.QOS_length = sizeof(qos[child].conn);
		p->npi.conn_req.QOS_offset = sizeof(p->npi.conn_req) + anums[(child + 1) % 2] * sizeof(addrs[child][0]);
		bcopy(&addrs[(child + 1) % 2], (&p->npi.conn_req + 1), anums[(child + 1) % 2] * sizeof(addrs[child][0]));
		bcopy(&qos[child].conn, (caddr_t) (&p->npi.conn_req + 1) + anums[(child + 1) % 2] * sizeof(addrs[child][0]), sizeof(qos[child].conn));
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ">connect");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_RES:
		ctrl->len = sizeof(p->npi.conn_res) + sizeof(qos[child].conn);
		data = NULL;
		p->npi.conn_res.PRIM_type = N_CONN_RES;
		p->npi.conn_res.RES_length = 0;
		p->npi.conn_res.RES_offset = 0;
		p->npi.conn_res.SEQ_number = seq[child];
		p->npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->npi.conn_res.QOS_length = sizeof(qos[child].conn);
		p->npi.conn_res.QOS_offset = sizeof(p->npi.conn_res);
		bcopy(&qos[child].conn, (&p->npi.conn_res + 1), sizeof(qos[child].conn));
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ">accept");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_REQ:
		ctrl->len = sizeof(p->npi.discon_req);
		data = NULL;
		p->npi.discon_req.PRIM_type = N_DISCON_REQ;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if (show && verbose > 1)
			print_command_state(child, ">disconnect");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_REQ:
		ctrl->len = sizeof(p->npi.optmgmt_req) + sizeof(qos[child].info);
		p->npi.optmgmt_req.QOS_length = sizeof(qos[child].info);
		p->npi.optmgmt_req.QOS_offset = sizeof(p->npi.optmgmt_req);
		p->npi.optmgmt_req.OPTMGMT_flags = 0;
		bcopy(&qos[child].info, (&p->npi.optmgmt_req + 1), sizeof(qos[child].info));
		if (show && verbose > 1)
			print_command_state(child, ">optmgmt");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
#endif

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
		switch (child) {
		default:
			event = __TEST_DATA;
			print_rx_data(child, "M_DATA----------", data->len);
			break;
		case 0:
		{
#if 1
			bib[child] = data->buf[0] & 0x80;
			bsn[child] = data->buf[0] & 0x7f;
			fib[child] = data->buf[1] & 0x80;
			fsn[child] = data->buf[1] & 0x7f;
			li[child] = data->buf[2] & 0x3f;
			sio[child] = data->buf[3] & 0x7;
			bsn[(child + 1) % 2] = fsn[child];
			switch (li[child]) {
			case 0:
				event = __TEST_FISU;
				break;
			case 1:
				event = sio[child] = data->buf[3] & 0x7;
				switch (sio[child]) {
				case LSSU_SIO:
					event = __TEST_SIO;
					break;
				case LSSU_SIN:
					event = __TEST_SIN;
					break;
				case LSSU_SIE:
					event = __TEST_SIE;
					break;
				case LSSU_SIOS:
					event = __TEST_SIOS;
					break;
				case LSSU_SIPO:
					event = __TEST_SIPO;
					break;
				case LSSU_SIB:
					event = __TEST_SIB;
					break;
				default:
					event = __TEST_SIX;
					break;
				}
				break;
			case 2:
				event = sio[child] = data->buf[4] & 0x7;
				switch (sio[child]) {
				case LSSU_SIO:
					event = __TEST_SIO2;
					break;
				case LSSU_SIN:
					event = __TEST_SIN2;
					break;
				case LSSU_SIE:
					event = __TEST_SIE2;
					break;
				case LSSU_SIOS:
					event = __TEST_SIOS2;
					break;
				case LSSU_SIPO:
					event = __TEST_SIPO2;
					break;
				case LSSU_SIB:
					event = __TEST_SIB2;
					break;
				default:
					event = __TEST_SIX2;
					break;
				}
				break;
			default:
				event = __TEST_MSU;
				break;
			}
			if (show_fisus || event != __TEST_FISU) {
				if (event != oldret || oldisb != (((bib[child] | bsn[child]) << 8) | (fib[child] | fsn[child]))) {
					// if (oldisb == (((bib[child] | bsn[child]) << 8) | (fib[child] |
					// fsn[child])) &&
					// ((event == __TEST_FISU && oldret == __TEST_MSU) ||
					// (event == __TEST_MSU && oldret == __TEST_FISU))) {
					// if (event == __TEST_MSU && !expand)
					// cntmsg++;
					// } else
					// cntret = 0;
					oldret = event;
					oldisb = ((bib[child] | bsn[child]) << 8) | (fib[child] | fsn[child]);
					// if (cntret) {
					// printf(" Ct=%d\n", cntret + 1);
					// fflush(stdout);
					// }
					cntret = 0;
				} else if (!expand)
					cntret++;
			}
			if (!cntret) {
				char buf[32];
				switch (event) {
				case __TEST_FISU:
					if (show_fisus) {
						snprintf(buf, sizeof(buf), "FISU");
						print_rx_msg(child, buf);
					}
					break;
				case __TEST_SIO:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIO");
					else
						snprintf(buf, sizeof(buf), "SIO[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIN:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIN");
					else
						snprintf(buf, sizeof(buf), "SIN[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIE:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIE");
					else
						snprintf(buf, sizeof(buf), "SIE[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIOS:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIOS");
					else
						snprintf(buf, sizeof(buf), "SIOS[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIPO:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIPO");
					else
						snprintf(buf, sizeof(buf), "SIPO[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIB:
					if (li[child] == 1)
						snprintf(buf, sizeof(buf), "SIB");
					else
						snprintf(buf, sizeof(buf), "SIB[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIX:
					snprintf(buf, sizeof(buf), "LSSU(invalid)[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIX2:
					snprintf(buf, sizeof(buf), "LSSU(invalid)[%d]", li[child]);
					print_rx_msg(child, buf);
					break;
				case __TEST_MSU:
					if (show_msus) {
						snprintf(buf, sizeof(buf), "MSU[%d]", li[child]);
						print_rx_msg(child, buf);
					}
					break;
				}
			}
#endif
#if 0
			uint32_t msg;

			switch ((msg = ((uint32_t *) data->buf)[0])) {
			case M2PA_STATUS_MESSAGE:
			{
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT3:
				case M2PA_VERSION_DRAFT3_1:
				case M2PA_VERSION_DRAFT6_9:
				case M2PA_VERSION_DRAFT7:
					mystatus = ((uint32_t *) data->buf)[2];
					break;
				case M2PA_VERSION_DRAFT4:
				case M2PA_VERSION_DRAFT4_1:
					mystatus = ((uint32_t *) data->buf)[3];
					bsn[child] = ntohs(((uint16_t *) data->buf)[4]);
					fsn[child] = ntohs(((uint16_t *) data->buf)[5]);
					break;
				case M2PA_VERSION_DRAFT4_9:
				case M2PA_VERSION_DRAFT5:
				case M2PA_VERSION_DRAFT5_1:
				case M2PA_VERSION_DRAFT6:
				case M2PA_VERSION_DRAFT6_1:
				case M2PA_VERSION_DRAFT9:
				case M2PA_VERSION_DRAFT10:
				case M2PA_VERSION_DRAFT11:
					mystatus = ((uint32_t *) data->buf)[4];
					bsn[child] = ntohl(((uint32_t *) data->buf)[2]);
					fsn[child] = ntohl(((uint32_t *) data->buf)[3]);
					break;
				case M2PA_VERSION_RFC4165:
					mystatus = ((uint32_t *) data->buf)[4];
					// bsn[child] = ntohl(((uint32_t *) data->buf)[2]);
					// fsn[child] = ntohl(((uint32_t *) data->buf)[3]);
					break;
				default:
					return __RESULT_SCRIPT_ERROR;
				}
				switch (mystatus) {
				case M2PA_STATUS_OUT_OF_SERVICE:
					event = __STATUS_OUT_OF_SERVICE;
					break;
				case M2PA_STATUS_IN_SERVICE:
					event = __STATUS_IN_SERVICE;
					break;
				case M2PA_STATUS_PROVING_NORMAL:
					event = __STATUS_PROVING_NORMAL;
					break;
				case M2PA_STATUS_PROVING_EMERGENCY:
					event = __STATUS_PROVING_EMERG;
					break;
				case M2PA_STATUS_ALIGNMENT:
					event = __STATUS_ALIGNMENT;
					break;
				case M2PA_STATUS_PROCESSOR_OUTAGE:
					event = __STATUS_PROCESSOR_OUTAGE;
					break;
				case M2PA_STATUS_BUSY:
					event = __STATUS_BUSY;
					break;
				case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
					event = __STATUS_PROCESSOR_ENDED;
					break;
				case M2PA_STATUS_BUSY_ENDED:
					event = __STATUS_BUSY_ENDED;
					break;
				case M2PA_STATUS_NONE:
					event = __TEST_ACK;
					break;
				case M2PA_STATUS_INVALID:
					event = __STATUS_INVALID_STATUS;
					break;
				default:
					return __RESULT_DECODE_ERROR;
				}
				break;
			}
			case M2PA_ACK_MESSAGE:
				print_rx_msg(child, msg_string(msg));
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT3_1:
					event = __TEST_ACK;
					break;
				default:
					return __RESULT_DECODE_ERROR;
				}
				break;
			case M2PA_PROVING_MESSAGE:
				print_rx_msg(child, msg_string(msg));
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT3:
				case M2PA_VERSION_DRAFT3_1:
					event = __MSG_PROVING;
					break;
				default:
					return __RESULT_DECODE_ERROR;
				}
				break;
			case M2PA_DATA_MESSAGE:
				event = __TEST_DATA;
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT3:
				case M2PA_VERSION_DRAFT3_1:
					break;
				case M2PA_VERSION_DRAFT4:
				case M2PA_VERSION_DRAFT4_1:
					bsn[child] = ntohs(((uint16_t *) data->buf)[4]);
					fsn[child] = ntohs(((uint16_t *) data->buf)[5]);
					break;
				case M2PA_VERSION_DRAFT4_9:
				case M2PA_VERSION_DRAFT5:
				case M2PA_VERSION_DRAFT5_1:
					bsn[child] = ntohl(((uint32_t *) data->buf)[2]);
					fsn[child] = ntohl(((uint32_t *) data->buf)[3]);
					break;
				case M2PA_VERSION_DRAFT6:
				case M2PA_VERSION_DRAFT6_1:
				case M2PA_VERSION_DRAFT6_9:
				case M2PA_VERSION_DRAFT7:
				case M2PA_VERSION_DRAFT9:
				case M2PA_VERSION_DRAFT10:
				case M2PA_VERSION_DRAFT11:
				case M2PA_VERSION_RFC4165:
					bsn[child] = ntohl(((uint32_t *) data->buf)[2]);
					fsn[child] = ntohl(((uint32_t *) data->buf)[3]);
					if (ntohl(((uint32_t *) data->buf)[1]) == 4 * sizeof(uint32_t)) {
						event = __TEST_ACK;
					}
					break;
				}
				break;
			}
			if (event != oldevt) {
				if ((event != __TEST_DATA && event != __TEST_ACK) || show)
					cntevt = 0;
				oldevt = event;
				show = 1;
				if (verbose > 1) {
					if (cntevt) {
						char buf[32];

						snprintf(buf, sizeof(buf), "Ct=%5d", cntevt + 1);
						print_command_state(child, buf);
					}
				}
				cntevt = 0;
			} else if (!show)
				cntevt++;
			if (!cntevt) {
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT4:
				case M2PA_VERSION_DRAFT4_1:
					bsn[CHILD_IUT] = ntohl(((uint32_t *) data->buf)[2]) >> 16;
					fsn[CHILD_IUT] = ntohl(((uint32_t *) data->buf)[2]) & 0x0000ffff;
					break;
				default:
					bsn[CHILD_IUT] = ntohl(((uint32_t *) data->buf)[2]) & 0x00ffffff;
					fsn[CHILD_IUT] = ntohl(((uint32_t *) data->buf)[3]) & 0x00ffffff;
					break;
				}
				switch (m2pa_version) {
				case M2PA_VERSION_DRAFT3:
				case M2PA_VERSION_DRAFT3_1:
					switch (event) {
					case __STATUS_IN_SERVICE:
						print_rx_msg(child, "IN-SERVICE");
						return event;
					case __STATUS_PROCESSOR_ENDED:
						print_rx_msg(child, "PROCESSOR-OUTAGE-ENDED");
						return event;
					case __MSG_PROVING:
					case __STATUS_PROVING_EMERG:
					case __STATUS_PROVING_NORMAL:
					{
						char buf[32];

						snprintf(buf, sizeof(buf), "PROVING [%d bytes]", (int) (ntohl(((uint32_t *) data->buf)[1]) - 2 * sizeof(uint32_t)));
						print_rx_msg(child, buf);
						return event;
					}
					case __STATUS_INVALID_STATUS:
					{
						char buf[32];

						snprintf(buf, sizeof(buf), "[INVALID STATUS %u]", ntohl(((uint32_t *) data->buf)[2]));
						print_rx_msg(child, buf);
						return event;
					}
					case __TEST_ACK:
					{
						char buf[32];

						snprintf(buf, sizeof(buf), "ACK [%u msgs]", ntohl(((uint32_t *) data->buf)[2]));
						print_rx_msg(child, buf);
						return event;
					}
					}
					break;
				case M2PA_VERSION_DRAFT9:
				case M2PA_VERSION_DRAFT10:
				case M2PA_VERSION_DRAFT11:
				case M2PA_VERSION_RFC4165:
					break;
				case M2PA_VERSION_DRAFT4:
				case M2PA_VERSION_DRAFT4_1:
				case M2PA_VERSION_DRAFT4_9:
					if (event == __TEST_DATA) {
						bsn[(child + 1) % 2] = fsn[child];
						break;
					}
					/* fall through */
				case M2PA_VERSION_DRAFT5:
				case M2PA_VERSION_DRAFT5_1:
					switch (event) {
					case __TEST_ACK:
						print_rx_msg(child, "IN-SERVICE");
						return event;
					case __STATUS_PROCESSOR_ENDED:
						print_rx_msg(child, "PROCESSOR-OUTAGE-ENDED");
						return event;
					}
					break;
				case M2PA_VERSION_DRAFT6:
				case M2PA_VERSION_DRAFT6_1:
				case M2PA_VERSION_DRAFT6_9:
				case M2PA_VERSION_DRAFT7:
					switch (event) {
					case __TEST_ACK:
						break;
					}
					break;
				default:
					return __RESULT_DECODE_ERROR;
				}
				switch (event) {
				case __STATUS_IN_SERVICE:
				case __STATUS_PROVING_NORMAL:
				case __STATUS_PROVING_EMERG:
				case __STATUS_ALIGNMENT:
				case __STATUS_OUT_OF_SERVICE:
				case __STATUS_PROCESSOR_OUTAGE:
				case __STATUS_PROCESSOR_ENDED:
				case __STATUS_BUSY:
				case __STATUS_BUSY_ENDED:
					print_rx_msg(child, status_string(mystatus));
					break;
				case __MSG_PROVING:
					event = __RESULT_DECODE_ERROR;
					break;
				case __TEST_DATA:
				{
					char buf[32];

					snprintf(buf, sizeof(buf), "DATA [%d bytes]", (int) (ntohl(((uint32_t *) data->buf)[1]) - 2 * sizeof(uint32_t)));
					print_rx_msg(child, buf);
					break;
				}
				case __TEST_ACK:
					print_rx_msg(child, "ACK");
					break;
				}
				return event;
			}
#endif
		}
		}
	}
	return ((last_event = event));
}


static int
do_decode_ctrl(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;
	union primitives *p = (union primitives *) ctrl->buf;

	if (ctrl->len >= sizeof(p->prim)) {
		switch ((last_prim = PRIM_type = p->prim)) {
#if 0
		case N_CONN_REQ:
			event = __TEST_CONN_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
			print_options(child, cbuf, p->npi.conn_req.QOS_offset, p->npi.conn_req.QOS_length);
			break;
		case N_CONN_RES:
			event = __TEST_CONN_RES;
			print_rx_prim(child, prim_string(p->npi.type));
			print_options(child, cbuf, p->npi.conn_res.QOS_offset, p->npi.conn_res.QOS_length);
			break;
		case N_DISCON_REQ:
			event = __TEST_DISCON_REQ;
			SEQ_number = p->npi.discon_req.SEQ_number;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_DATA_REQ:
			event = __TEST_DATA_REQ;
			DATA_xfer_flags = p->npi.data_req.DATA_xfer_flags;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_EXDATA_REQ:
			event = __TEST_EXDATA_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_INFO_REQ:
			event = __TEST_INFO_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_BIND_REQ:
			event = __TEST_BIND_REQ;
			CONIND_number = p->npi.bind_req.CONIND_number;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_UNBIND_REQ:
			event = __TEST_UNBIND_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_UNITDATA_REQ:
			event = __TEST_UNITDATA_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.unitdata_req.DEST_offset, p->npi.unitdata_req.DEST_length));
#else
			print_addrs(child, cbuf + p->npi.unitdata_req.DEST_offset, p->npi.unitdata_req.DEST_length);
#endif
			break;
		case N_OPTMGMT_REQ:
			event = __TEST_OPTMGMT_REQ;
			print_rx_prim(child, prim_string(p->npi.type));
			print_mgmtflag(child, p->npi.optmgmt_req.OPTMGMT_flags);
			print_options(child, cbuf, p->npi.optmgmt_req.QOS_offset, p->npi.optmgmt_req.QOS_length);
			break;
		case N_CONN_IND:
			event = __TEST_CONN_IND;
			SEQ_number = p->npi.conn_ind.SEQ_number;
			CONN_flags = p->npi.conn_ind.CONN_flags;
			DEST_buffer = (typeof(DEST_buffer)) (ctrl->buf + p->npi.conn_ind.DEST_offset);
			DEST_length = p->npi.conn_ind.DEST_length;
			SRC_buffer = (typeof(SRC_buffer)) (ctrl->buf + p->npi.conn_ind.SRC_offset);
			SRC_length = p->npi.conn_ind.SRC_length;
			QOS_buffer = (typeof(QOS_buffer)) (ctrl->buf + p->npi.conn_ind.QOS_offset);
			QOS_length = p->npi.conn_ind.QOS_length;
			DATA_buffer = data->buf;
			DATA_length = data->len;
			print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.conn_ind.DEST_offset, p->npi.conn_ind.DEST_length));
#else
			print_addrs(child, cbuf + p->npi.conn_ind.DEST_offset, p->npi.conn_ind.DEST_length);
#endif
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.conn_ind.SRC_offset, p->npi.conn_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->npi.conn_ind.SRC_offset, p->npi.conn_ind.SRC_length);
#endif
			print_options(child, cbuf, p->npi.conn_ind.QOS_offset, p->npi.conn_ind.QOS_length);
			break;
		case N_CONN_CON:
			event = __TEST_CONN_CON;
			print_rx_prim(child, prim_string(p->npi.type));
			print_addrs(child, cbuf + p->npi.conn_con.RES_offset, p->npi.conn_con.RES_length);
			print_options(child, cbuf, p->npi.conn_con.QOS_offset, p->npi.conn_con.QOS_length);
			break;
		case N_DISCON_IND:
			event = __TEST_DISCON_IND;
			print_rx_prim(child, prim_string(p->npi.type));
			break;
		case N_DATA_IND:
			event = __TEST_DATA_IND;
			DATA_xfer_flags = p->npi.data_ind.DATA_xfer_flags;
			print_rx_prim(child, prim_string(p->npi.type));
			sid[child] = ((N_qos_sel_data_sctp_t *) (cbuf + sizeof(p->npi.data_ind)))->sid;
			// print_options(child, cbuf, sizeof(p->npi.data_ind),
			// sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_EXDATA_IND:
			event = __TEST_EXDATA_IND;
			print_rx_prim(child, prim_string(p->npi.type));
			sid[child] = ((N_qos_sel_data_sctp_t *) (cbuf + sizeof(p->npi.exdata_ind)))->sid;
			// print_options(child, cbuf, sizeof(p->npi.exdata_ind),
			// sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_DATACK_IND:
			event = __TEST_DATACK_IND;
			print_rx_prim(child, prim_string(p->npi.type));
			sid[child] = ((N_qos_sel_data_sctp_t *) (cbuf + sizeof(p->npi.datack_ind)))->sid;
			// print_options(child, cbuf, sizeof(p->npi.datack_ind),
			// sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_INFO_ACK:
			event = __TEST_INFO_ACK;
			last_info = p->npi.info_ack;
			print_ack_prim(child, prim_string(p->npi.type));
			print_info(child, &p->npi.info_ack);
			break;
		case N_BIND_ACK:
			event = __TEST_BIND_ACK;
			CONIND_number = p->npi.bind_ack.CONIND_number;
			TOKEN_value = p->npi.bind_ack.TOKEN_value;
			tok[child] = TOKEN_value;
			ADDR_buffer = (typeof(ADDR_buffer)) (ctrl->buf + p->npi.bind_ack.ADDR_offset);
			ADDR_length = p->npi.bind_ack.ADDR_length;
			PROTOID_buffer = (typeof(PROTOID_buffer)) (ctrl->buf + p->npi.bind_ack.PROTOID_offset);
			PROTOID_length = p->npi.bind_ack.PROTOID_length;
			print_ack_prim(child, prim_string(p->npi.type));
			print_prots(child, cbuf + p->npi.bind_ack.PROTOID_offset, p->npi.bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length));
#else
			print_addrs(child, cbuf + p->npi.bind_ack.ADDR_offset, p->npi.bind_ack.ADDR_length);
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
			NPI_error = p->npi.error_ack.NPI_error;
			last_errno = p->npi.error_ack.UNIX_error;
			print_ack_prim(child, prim_string(p->npi.type));
			print_string(child, nerrno_string(p->npi.error_ack.NPI_error, p->npi.error_ack.UNIX_error));
			break;
		case N_OK_ACK:
			event = __TEST_OK_ACK;
			print_ack_prim(child, prim_string(p->npi.type));
			break;
		case N_UNITDATA_IND:
			event = __TEST_UNITDATA_IND;
			print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.unitdata_ind.SRC_offset, p->npi.unitdata_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->npi.unitdata_ind.SRC_offset, p->npi.unitdata_ind.SRC_length);
#endif
			break;
		case N_UDERROR_IND:
			event = __TEST_UDERROR_IND;
			ADDR_buffer = (typeof(ADDR_buffer)) (ctrl->buf + p->npi.uderror_ind.DEST_offset);
			ADDR_length = p->npi.uderror_ind.DEST_length;
			RESERVED_field[0] = p->npi.uderror_ind.RESERVED_field;
			ERROR_type = p->npi.uderror_ind.ERROR_type;
			print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->npi.uderror_ind.DEST_offset, p->npi.uderror_ind.DEST_length));
#else
			print_addrs(child, cbuf + p->npi.uderror_ind.DEST_offset, p->npi.uderror_ind.DEST_length);
#endif
			break;
#endif

		case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_IUT_RPO;
			break;
		case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_IUT_RPR;
			break;
		case SL_IN_SERVICE_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_IUT_IN_SERVICE;
			break;
		case SL_OUT_OF_SERVICE_IND:
			print_command_state(child, oos_string(p->sl.out_of_service_ind.sl_reason));
			event = __EVENT_IUT_OUT_OF_SERVICE;
			break;
		case SL_PDU_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_IUT_DATA;
			break;
		case SL_LINK_CONGESTED_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_LINK_CONGESTION_CEASED_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_RETRIEVED_MESSAGE_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_RETRIEVAL_COMPLETE_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_RB_CLEARED_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_BSNT_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SL_RTB_CLEARED_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;

#if 1
		case SDT_RC_SIGNAL_UNIT_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_IAC_CORRECT_SU_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_IAC_ABORT_PROVING_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_LSC_LINK_FAILURE_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_TXC_TRANSMISSION_REQUEST_IND:
			// print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_RC_CONGESTION_ACCEPT_IND:
		case SDT_RC_CONGESTION_DISCARD_IND:
		case SDT_RC_NO_CONGESTION_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
#endif

		case LMI_INFO_ACK:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case LMI_OK_ACK:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __TEST_OK_ACK;
			break;
		case LMI_ERROR_ACK:
			if (show && verbose > 1) {
				print_command_state(child, prim_string(p->prim));
				print_string(child, lmerrno_string(p->lmi.error_ack.lmi_errno, p->lmi.error_ack.lmi_reason));
			}
			event = __TEST_ERROR_ACK;
			break;
		case LMI_ENABLE_CON:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __TEST_ENABLE_CON;
			break;
		case LMI_DISABLE_CON:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __TEST_DISABLE_CON;
			break;
		case LMI_ERROR_IND:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __TEST_ERROR_IND;
			break;
		case LMI_STATS_IND:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case LMI_EVENT_IND:
			if (show && verbose > 1)
				print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		default:
			event = __EVENT_UNKNOWN;
			print_no_prim(child, p->prim);
			break;
		}
		if (data && data->len >= 0)
			if ((last_event = do_decode_data(child, ctrl, data)) != __EVENT_UNKNOWN)
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
		if ((last_event = do_decode_data(child, ctrl, data)) != __EVENT_UNKNOWN)
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

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_unbound(int child)
{
#if 0
	if (do_signal(child, __TEST_INFO_REQ))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_UNBND) {
		failure_string = FAILURE_STRING("bad CURRENT_state");
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
#endif
	return __RESULT_FAILURE;
}

static int
postamble_unbound(int child)
{
	return __RESULT_SUCCESS;
}

#if 0
static int
preamble_bind(int child)
{
	unsigned char proto = TEST_PROTOCOL;
	unsigned char prot[] = { proto };
	int coninds = (child == CHILD_IUT) ? !iut_connects : iut_connects;

	ADDR_buffer = addrs[child];
	ADDR_length = anums[child] * sizeof(addrs[child][0]);
	CONIND_number = coninds;
	BIND_flags = TOKEN_REQUEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);

	if (preamble_unbound(child))
		goto failure;
	state++;
	if (do_signal(child, __TEST_BIND_REQ))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK))
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

static int
postamble_unbind(int child)
{
	int failed = 0;

	if (stop_tt())
		failed = failed ? : state;
	state++;
	if (do_signal(child, __TEST_UNBIND_REQ))
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
		failed = failed ? : state;
	state++;
	if (postamble_unbound(child))
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;

}

static int
preamble_connect(int child)
{
	if ((child == CHILD_PTU && iut_connects == 0) || (child == CHILD_IUT && iut_connects != 0)) {
		/* keep trying to connect until successful or the child times out */
		for (;;) {
			ADDR_buffer = addrs[(child + 1) % 2];
			ADDR_length = anums[(child + 1) % 2] * sizeof(addrs[(child + 1) % 2][0]);
			CONN_flags = 0;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = NULL;
			DATA_length = 0;

			if (do_signal(child, __TEST_CONN_REQ))
				goto failure;
			state++;
			switch (get_event(child)) {
			case __TEST_CONN_CON:
				state++;
				break;
			case __TEST_DISCON_IND:
				state++;
				test_msleep(child, NORMAL_WAIT);
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		}
		if (do_signal(child, __TEST_INFO_REQ))
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_DATA_XFER) {
			failure_string = FAILURE_STRING("bad CURRENT_state");
			goto failure;
		}
	} else {
		N_qos_sel_conn_sctp_t test_qos = {
			.n_qos_type = N_QOS_SEL_CONN_SCTP,
			.i_streams = 2,
			.o_streams = 2,
		};

		if (expect(child, INFINITE_WAIT, __TEST_CONN_IND))
			goto failure;
		state++;

		TOKEN_value = tok[child];
		ADDR_buffer = addrs[child];
		ADDR_length = anums[child] * sizeof(addrs[child][0]);
		CONN_flags = 0;
		QOS_buffer = &test_qos;
		QOS_length = sizeof(test_qos);
		DATA_buffer = NULL;
		DATA_length = 0;

		if (do_signal(child, __TEST_CONN_RES))
			goto failure;
		SEQ_number = 0;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ))
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
			goto failure;
		state++;
		if (last_info.CURRENT_state != NS_DATA_XFER) {
			failure_string = FAILURE_STRING("bad CURRENT_state");
			goto failure;
		}
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_disconnect(int child)
{
	int failed = 0;

	if (child == CHILD_IUT) {
		ADDR_buffer = NULL;
		ADDR_length = 0;
		if (do_signal(child, __TEST_DISCON_REQ))
			failed = failed ? : state;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
			failed = failed ? : state;
	} else {
		if (expect(child, LONGEST_WAIT, __TEST_DISCON_IND)) {
			ADDR_buffer = NULL;
			ADDR_length = 0;
			if (do_signal(child, __TEST_DISCON_REQ))
				failed = failed ? : state;
			state++;
			if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
				failed = failed ? : state;
		}
	}
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
#endif

static int
preamble_push(int child)
{
#if 0
	if (child == CHILD_IUT) {
		state++;
		if (do_signal(child, __TEST_PUSH))
			goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
#endif
	return __RESULT_FAILURE;
}

static int
postamble_pop(int child)
{
#if 0
	int failed = 0;

	if (child == CHILD_IUT) {
		state++;
		if (do_signal(child, __TEST_POP))
			failed = failed ? : state;
	}
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
#endif
	return __RESULT_FAILURE;
}

static int
preamble_config(int child)
{
#if 0
	if (preamble_push(child))
		goto failure;
	state++;
#endif
	if (do_signal(child, __TEST_SDL_OPTIONS))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SDL_CONFIG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SDT_OPTIONS))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SDT_CONFIG))
		goto failure;
	state++;
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_SL_OPTIONS))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SL_CONFIG))
			goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
preamble_attach(int child)
{
	int failed = 0;

	if (preamble_config(child))
		goto failure;
	state++;
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child] * sizeof(addrs[child][0]);
	if (do_signal(child, __TEST_ATTACH_REQ))
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
		failed = failed ? : state;
#if 0
	if (child == CHILD_PTU) {
		return preamble_bind(child);
	}
#endif
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_detach(int child)
{
	int failed = 0;

	state++;
	if (do_signal(child, __TEST_DETACH_REQ))
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
		failed = failed ? : state;
#if 0
	if (child == CHILD_PTU) {
		state++;
		if (postamble_unbind(child))
			failed = failed ? : state;
	}
	state++;
	if (postamble_pop(child))
		failed = failed ? : state;
#endif
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
preamble_enable(int child)
{
	if (preamble_attach(child))
		goto failure;
	state++;
	ADDR_buffer = addrs[(child + 1) % 2];
	ADDR_length = anums[(child + 1) % 2] * sizeof(addrs[(child + 1) % 2][0]);
	if (do_signal(child, __TEST_ENABLE_REQ))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_ENABLE_CON))
		goto failure;
#if 0
	if (child == CHILD_PTU) {
		if (preamble_connect(child))
			goto failure;
	}
#endif
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_disable(int child)
{
	int failed = 0;

	state++;
	if (do_signal(child, __TEST_DISABLE_REQ))
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_DISABLE_CON))
		failed = failed ? : state;
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW))
		failed = failed ? : state;
#if 0
	if (child == CHILD_PTU) {
		if (postamble_disconnect(child))
			failed = failed ? : state;
	}
	state++;
#endif
	if (postamble_detach(child))
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_link_power_off(int child)
{
	int failed = 0;

	if (stop_tt())
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		failed = failed ? : state;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	if (postamble_disable(child))
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
preamble_none(int child)
{
	switch (child) {
	case CHILD_PTU:
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
}

static int
postamble_none(int child)
{
	switch (child) {
	case CHILD_PTU:
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
}

static int
preamble_link_power_off(int child)
{
	show_timeout = 0;

	bib[0] = fib[0] = bib[1] = fib[1] = 0x0;
#if 0
	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT9:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0xffffff;
		break;
	default:
		bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x0;
		break;
	}
#else
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x7f;
#endif

	switch (child) {
	case CHILD_PTU:
		if (preamble_enable(child))
			goto failure;
		break;
	case CHILD_IUT:
		if (preamble_enable(child))
			goto failure;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#if 0
static int
preamble_link_power_off_pr(int child)
{
	config->opt.popt = 0;
	return preamble_link_power_off(child);
}
static int
preamble_link_power_off_np(int child)
{
	config->opt.popt = SS7_POPT_NOPR;
	return preamble_link_power_off(child);
}
#endif
static int
preamble_link_power_on(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (preamble_link_power_off(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (preamble_link_power_off(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#if 0
static int
preamble_link_power_on_pr(int child)
{
	config->opt.popt = 0;
	return preamble_link_power_on(child);
}
#endif
static int
preamble_link_power_on_np(int child)
{
	config->opt.popt = SS7_POPT_NOPR;
	return preamble_link_power_on(child);
}
static int
preamble_link_out_of_service(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (preamble_link_power_on(child))
			goto failure;
		state++;
		if (expect(child, LONGEST_WAIT, __TEST_SIOS))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIOS))
			goto failure;
		break;
	case CHILD_IUT:
		if (preamble_link_power_on(child))
			goto failure;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#if 0
static int
preamble_link_out_of_service_pr(int child)
{
	config->opt.popt = 0;
	return preamble_link_out_of_service(child);
}
static int
preamble_link_out_of_service_np(int child)
{
	config->opt.popt = SS7_POPT_NOPR;
	return preamble_link_out_of_service(child);
}
#endif
static int
postamble_link_out_of_service(int child)
{
	int failed = 0;

	if (stop_tt())
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		failed = failed ? : state;
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	if (postamble_disable(child))
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
preamble_link_in_service(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (preamble_link_out_of_service(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, __TEST_SIO))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIO))
			goto failure;
		state++;
		if (!(config->opt.popt & SS7_POPT_NOPR)) {
			for (;;) {
				switch (get_event(child)) {
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (do_signal(child, __TEST_SIE))
						goto failure;
					state++;
					print_less(child);
					continue;
				case __STATUS_IN_SERVICE:
					state++;
					print_more(child);
					break;
				}
				break;
			}
		}
		if (do_signal(child, __STATUS_IN_SERVICE))
			goto failure;
		break;
	case CHILD_IUT:
		if (preamble_link_out_of_service(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
			goto failure;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}

#if 0
static int
preamble_link_in_service_pr(int child)
{
	config->opt.popt = 0;
	return preamble_link_in_service(child);
}
static int
preamble_link_in_service_np(int child)
{
	config->opt.popt = SS7_POPT_NOPR;
	return preamble_link_in_service(child);
}
static int
preamble_link_in_service_basic(int child)
{
	config->opt.popt = 0;
	return preamble_link_in_service(child);
}
#endif
static int
preamble_link_in_service_pcr(int child)
{
	config->opt.popt = SS7_POPT_PCR;
	return preamble_link_in_service(child);
}
static int
postamble_link_in_service(int child)
{
	int failed = 0;

	if (stop_tt())
		failed = failed ? : state;
	state++;
	if (child == CHILD_IUT) {
		if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
			failed = failed ? : state;
		state++;
	} else {
		if (do_signal(child, __TEST_SIOS))
			failed = failed ? : state;
		state++;
		if (expect(child, INFINITE_WAIT, __TEST_SIOS))
			failed = failed ? : state;
		state++;
	}
	if (postamble_link_out_of_service(child))
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}


int iut_showmsg(struct strbuf *ctrl, struct strbuf *data);

#if 0
static int
pt_open(void)
{
	// printf(" :open\n");
	// fflush(stdout);
	if ((test_fd[0] = open(devname, O_NONBLOCK | O_RDWR)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	// printf(" :ioctl\n");
	// fflush(stdout);
	if (ioctl(test_fd[0], I_SRDOPT, RMSGD) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
pt_close(void)
{
	// printf(" :close\n");
	// fflush(stdout);
	if (close(test_fd[0]) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
pt_attach(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	ppa_t ppa;
	ppa = (PTU_TEST_SLOT << 12) | (PTU_TEST_SPAN << 8) | (PTU_TEST_CHAN << 0);
	// printf(" :attach\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa_t);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&ppa, p->attach_req.lmi_ppa, sizeof(ppa_t));
	if ((ret = putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(test_fd[0], &ctrl, &data, &flags)) < 0) {
			printf("%s: %s\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			return __RESULT_FAILURE;
		}
		if (p->lmi_primitive != LMI_OK_ACK) {
			iut_showmsg(&ctrl, &data);
			return __RESULT_FAILURE;
		} else {
			return __RESULT_SUCCESS;
		}
	}
}

static int
pt_detach(void)
{
	int ret;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	// printf(" :detach\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
pt_enable(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	// printf(" :enable\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->enable_req);
	ctrl.buf = cbuf;
	p->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	ctrl.len = sizeof(p->enable_req);
	if ((ret = putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(test_fd[0], &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf("%s: %s\n", __FUNCTION__, strerror(errno));
				fflush(stdout);
				return __RESULT_FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_ENABLE_CON && p->lmi_primitive != LMI_OK_ACK) {
			printf("%s: %s\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			iut_showmsg(&ctrl, &data);
		} else
			return __RESULT_SUCCESS;
	}
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
pt_stats(void)
{
	struct strioctl ctl;
	sdt_stats_t stats;
	printf("    :stats sdt\n");
	fflush(stdout);
	ctl.ic_cmd = SDT_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(stats);
	ctl.ic_dp = (char *) &stats;
	if (ioctl(test_fd[0], I_STR, &ctl) < 0) {
		printf("****ERROR: stats for sdt failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		exit(2);
	}
	fflush(stdout);
	printf("tx_bytes = %lu\n", stats.tx_bytes);
	printf("tx_sus = %lu\n", stats.tx_sus);
	printf("tx_sus_repeated = %lu\n", stats.tx_sus_repeated);
	printf("tx_underruns = %lu\n", stats.tx_underruns);
	printf("tx_aborts = %lu\n", stats.tx_aborts);
	printf("tx_buffer_overflows = %lu\n", stats.tx_buffer_overflows);
	printf("tx_sus_in_error = %lu\n", stats.tx_sus_in_error);
	printf("rx_bytes = %lu\n", stats.rx_bytes);
	printf("rx_sus = %lu\n", stats.rx_sus);
	printf("rx_sus_compressed = %lu\n", stats.rx_sus_compressed);
	printf("rx_overruns = %lu\n", stats.rx_overruns);
	printf("rx_aborts = %lu\n", stats.rx_aborts);
	printf("rx_buffer_overflows = %lu\n", stats.rx_buffer_overflows);
	printf("rx_sus_in_error = %lu\n", stats.rx_sus_in_error);
	printf("rx_sync_transitions = %lu\n", stats.rx_sync_transitions);
	printf("rx_bits_octet_counted = %lu\n", stats.rx_bits_octet_counted);
	printf("rx_crc_errors = %lu\n", stats.rx_crc_errors);
	printf("rx_frame_errors = %lu\n", stats.rx_frame_errors);
	printf("rx_frame_overflows = %lu\n", stats.rx_frame_overflows);
	printf("rx_frame_too_long = %lu\n", stats.rx_frame_too_long);
	printf("rx_frame_too_short = %lu\n", stats.rx_frame_too_short);
	printf("rx_residue_errors = %lu\n", stats.rx_residue_errors);
	printf("rx_length_error = %lu\n", stats.rx_length_error);
	printf("carrier_cts_lost = %lu\n", stats.carrier_cts_lost);
	printf("carrier_dcd_lost = %lu\n", stats.carrier_dcd_lost);
	printf("carrier_lost = %lu\n", stats.carrier_lost);
	fflush(stdout);
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
pt_disable(void)
{
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct strbuf ctrl = { sizeof(*cbuf), sizeof(p->disable_req), cbuf };
	// printf(" :disable\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	ctrl.buf = cbuf;
	p->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	if (putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}
#endif

static int
pt_config(void)
{
	struct strioctl ctl;

	// printf(" :config\n");
	// fflush(stdout);
	// printf(" :options sdl\n");
	// fflush(stdout);
	config->opt.pvar = SS7_PVAR_ITUT_96;
	config->opt.popt = 0;
	ctl.ic_cmd = SDL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[0], I_STR, &ctl) < 0) {
		printf("****ERROR: options sdl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		exit(2);
	}
	/* go with defaults */
#if 0
	printf("    :config sdl\n");
	fflush(stdout);
	config->sdl.ifflags = 0;
	config->sdl.iftype = SDL_TYPE_DS0;
	config->sdl.ifrate = 64000;
	config->sdl.ifgtype = SDL_GTYPE_NONE;
	config->sdl.ifgrate = 1544000;
	conficonfigDL_MODE_PEER;
	config->sdl.ifgmode = SDL_GMODE_NONE;
	config->sdl.ifgcrc = SDL_GCRC_CRC4;
	config->sdl.ifcoding = SDL_CODING_B8ZS;
	config->sdl.ifframing = SDL_FRAMING_ESF;
	config->sdl.ifleads = 0;
	config->sdl.ifalarms = 0;
	config->sdl.ifrxlevel = 0;
	config->sdl.iftxlevel = 0;
	config->sdl.ifsync = 0;
	config->sdl.ifsyncsrc[0] = 0;
	config->sdl.ifsyncsrc[1] = 0;
	config->sdl.ifsyncsrc[2] = 0;
	config->sdl.ifsyncsrc[3] = 0;
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdl);
	ctl.ic_dp = (char *) &config->sdl;
	if (ioctl(test_fd[0], I_STR, &ctl) < 0) {
		printf("****ERROR: config sdl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		return __RESULT_FAILURE;
	}
#endif
	// printf(" :config sdt\n");
	// fflush(stdout);
	config->sdt.N = 16;
	config->sdt.m = 272;
	config->sdt.t8 = 100;
	config->sdt.Tin = 4;
	config->sdt.Tie = 1;
	config->sdt.T = 64;
	config->sdt.D = 256;
	config->sdt.Te = 577169;
	config->sdt.De = 9308000;
	config->sdt.Ue = 144292000;
	config->sdt.b = 8;
	config->sdt.f = SDT_FLAGS_ONE;
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdt);
	ctl.ic_dp = (char *) &config->sdt;
	if (ioctl(test_fd[0], I_STR, &ctl) < 0) {
		printf("****ERROR: config sdt failed\n");
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

/*
 *  The PT is enabled in SDL mode.  This will allow most of the test cases.
 *  For several test cases in Q.781, however, it is necessary to enable the
 *  device in raw mode so that the PT can have control over the number of
 *  flags sent between frames as well as the validity of the CRC bits.  For
 *  those test cases, a raw mode is used where the PT writes bits directly to
 *  the line.
 */
int
pt_power_on(void)
{
	int ret;
	char cbuf[BUFSIZE];
	// char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	// struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union SDT_primitives *d = (union SDT_primitives *) cbuf;

	// printf(" :power on\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(d->daedt_start_req);
	ctrl.buf = cbuf;
	d->daedt_start_req.sdt_primitive = SDT_DAEDT_START_REQ;
	if ((ret = putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}

	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(d->daedr_start_req);
	ctrl.buf = cbuf;
	d->daedr_start_req.sdt_primitive = SDT_DAEDR_START_REQ;
	if ((ret = putmsg(test_fd[0], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

#if 0
static int
pt_start(void)
{
	if (pt_open() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_attach() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_config() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_enable() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_power_on() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int
pt_end(void)
{
	if (pt_disable() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_detach() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (pt_close() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_open(void)
{
	return test_open(CHILD_IUT, devname, O_NONBLOCK | O_RDWR);
}
#endif

#if 0
static int
iut_close(void)
{
	return test_close(CHILD_IUT);
}
#endif

#if 0
static int
iut_attach(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	ppa_t ppa;
	ppa = (IUT_TEST_SLOT << 12) | (IUT_TEST_SPAN << 8) | (IUT_TEST_CHAN << 0);
	// printf(" :attach\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa_t);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&ppa, p->attach_req.lmi_ppa, sizeof(ppa_t));
	if ((ret = putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(test_fd[1], &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf
				    ("                                   ****ERROR: getmsg failed\n");
				printf("                                              %s: %s\n",
				       __FUNCTION__, strerror(errno));
				fflush(stdout);
				return __RESULT_FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_OK_ACK) {
			printf("                                   ****ERROR: getmsg failed\n");
			fflush(stdout);
			printf("                                              %s: %s\n",
			       __FUNCTION__, strerror(errno));
			iut_showmsg(&ctrl, &data);
		} else
			return __RESULT_SUCCESS;
	}
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_detach(void)
{
	int ret;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	// printf(" :detach\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_enable(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	// printf(" :enable\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->enable_req);
	ctrl.buf = cbuf;
	p->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	ctrl.len = sizeof(p->enable_req);
	if ((ret = putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		fflush(stdout);
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		return __RESULT_FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(test_fd[1], &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf
				    ("                                   ****ERROR: getmsg failed\n");
				printf("                                              %s: %s\n",
				       __FUNCTION__, strerror(errno));
				fflush(stdout);
				return __RESULT_FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_ENABLE_CON && p->lmi_primitive != LMI_OK_ACK) {
			printf("                                   ****ERROR: getmsg failed\n");
			fflush(stdout);
			printf("                                              %s: %s\n",
			       __FUNCTION__, strerror(errno));
			iut_showmsg(&ctrl, &data);
		} else
			return __RESULT_SUCCESS;
	}
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_disable(void)
{
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct strbuf ctrl = { sizeof(*cbuf), sizeof(p->disable_req), cbuf };
	// printf(" :disable\n");
	// fflush(stdout);
	ctrl.maxlen = BUFSIZE;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	ctrl.buf = cbuf;
	p->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	if (putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_config(void)
{
	struct strioctl ctl;
	// printf(" :config\n");
	// fflush(stdout);
	// printf(" :options sdl\n");
	// fflush(stdout);
	config->opt.pvar = SS7_PVAR_ITUT_96;
	config->opt.popt = 0;
	ctl.ic_cmd = SL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
	      option_sdl_failed:
		printf("                                   ****ERROR: options sdl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	ctl.ic_cmd = SL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto option_sdl_failed;
	if (config->opt.pvar != SS7_PVAR_ITUT_96 || config->opt.popt != 0)
		goto option_sdl_failed;
	// printf(" :options sdt\n");
	// fflush(stdout);
	config->opt.pvar = SS7_PVAR_ITUT_96;
	config->opt.popt = 0;
	ctl.ic_cmd = SDT_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
	      option_sdt_failed:
		printf("                                   ****ERROR: options sdt failed\n");
		fflush(stdout);
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		return __RESULT_FAILURE;
	}
	ctl.ic_cmd = SDT_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto option_sdt_failed;
	if (config->opt.pvar != SS7_PVAR_ITUT_96 || config->opt.popt != 0)
		goto option_sdt_failed;
	// printf(" :options sl\n");
	// fflush(stdout);
	config->opt.pvar = SS7_PVAR_ITUT_96;
	config->opt.popt = 0;
	ctl.ic_cmd = SL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
	      option_sl_failed:
		printf("                                   ****ERROR: options sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	ctl.ic_cmd = SL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->opt);
	ctl.ic_dp = (char *) &config->opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto option_sl_failed;
	if (config->opt.pvar != SS7_PVAR_ITUT_96 || config->opt.popt != 0)
		goto option_sl_failed;
	// printf(" :config sdl\n");
	// fflush(stdout);
	/* go with defaults */
#if 0
	config->sdl.ifflags = 0;
	config->sdl.iftype = SDL_TYPE_DS0;
	config->sdl.ifrate = 64000;
	config->sdl.ifgtype = SDL_GTYPE_NONE;
	config->sdl.ifgrate = 1544000;
	config->sdl.ifmode = SDL_MODE_PEER;
	config->sdl.ifgmode = SDL_GMODE_NONE;
	config->sdl.ifgcrc = SDL_GCRC_CRC4;
	config->sdl.ifcoding = SDL_CODING_B8ZS;
	config->sdl.ifframing = SDL_FRAMING_ESF;
	config->sdl.ifleads = 0;
	config->sdl.ifalarms = 0;
	config->sdl.ifrxlevel = 0;
	config->sdl.iftxlevel = 0;
	config->sdl.ifsync = 0;
	config->sdl.ifsyncsrc[0] = 0;
	config->sdl.ifsyncsrc[1] = 0;
	config->sdl.ifsyncsrc[2] = 0;
	config->sdl.ifsyncsrc[3] = 0;
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdl);
	ctl.ic_dp = (char *) &config->sdl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto config_sdl_failed;
#endif
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdl);
	ctl.ic_dp = (char *) &config->sdl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		// config_sdl_failed:
		printf("                                   ****ERROR: config sdl failed\n");
		fflush(stdout);
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		return __RESULT_FAILURE;
	}
	/* go with defaults */
#if 0
	if (config->sdl.iftype != SDL_TYPE_DS0
	    || config->sdl.ifrate != 64000
	    || config->sdl.ifgtype != SDL_GTYPE_NONE
	    || config->sdl.ifgrate != 1544000
	    || config->sdl.ifmode != SDL_MODE_PEER
	    || config->sdl.ifgmode != SDL_GMODE_NONE
	    || config->sdl.ifgcrc != SDL_GCRC_CRC4
	    || config->sdl.ifcoding != SDL_CODING_B8ZS || config->sdl.ifframing != SDL_FRAMING_ESF)
		goto config_sdl_failed;
#endif
	// printf(" :config sdt\n");
	// fflush(stdout);
#if 0
	config->sdt.t8 = 100;
	config->sdt.Tin = 4;
	config->sdt.Tie = 1;
	config->sdt.T = 64;
	config->sdt.D = 256;
	config->sdt.Te = 577169;
	config->sdt.De = 9308000;
	config->sdt.Ue = 144292000;
	config->sdt.N = 16;
	config->sdt.m = 272;
	config->sdt.b = 8;
	config->sdt.f = iut_flags;
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdt);
	ctl.ic_dp = (char *) &config->sdt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto config_sdt_failed;
#endif
	ctl.ic_cmd = SDT_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sdt);
	ctl.ic_dp = (char *) &config->sdt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		// config_sdt_failed:
		printf("                                   ****ERROR: config sdt failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
#if 0
	if (config->sdt.t8 != 100
	    || config->sdt.Tin != 4
	    || config->sdt.Tie != 1
	    || config->sdt.T != 64
	    || config->sdt.D != 256
	    || config->sdt.Te != 577169
	    || config->sdt.De != 9308000
	    || config->sdt.Ue != 144292000
	    || config->sdt.N != 16
	    || config->sdt.m != 272 || config->sdt.b != 8 || config->sdt.f != iut_flags)
		goto config_sdt_failed;
#endif
	// printf(" :config sl\n");
	// fflush(stdout);
	config->sl.t1 = 45 * 1000;	/* milliseconds */
	config->sl.t2 = 5 * 1000;	/* milliseconds */
	config->sl.t2l = 20 * 1000;	/* milliseconds */
	config->sl.t2h = 100 * 1000;	/* milliseconds */
	config->sl.t3 = 1 * 1000;	/* milliseconds */
	config->sl.t4n = 8 * 1000;	/* milliseconds */
	config->sl.t4e = 500;		/* milliseconds */
	config->sl.t5 = 100;		/* milliseconds */
	config->sl.t6 = 4 * 1000;	/* milliseconds */
	config->sl.t7 = 1 * 1000;		/* milliseconds */
	config->sl.rb_abate = 3;	/* messages */
	config->sl.rb_accept = 6;	/* messages */
	config->sl.rb_discard = 9;	/* messages */
	config->sl.tb_abate_1 = 128 * 272;	/* octets */
	config->sl.tb_onset_1 = 256 * 272;	/* octets */
	config->sl.tb_discd_1 = 384 * 272;	/* octets */
	config->sl.tb_abate_2 = 512 * 272;	/* octets */
	config->sl.tb_onset_2 = 640 * 272;	/* octets */
	config->sl.tb_discd_2 = 768 * 272;	/* octets */
	config->sl.tb_abate_3 = 896 * 272;	/* octets */
	config->sl.tb_onset_3 = 1024 * 272;	/* octets */
	config->sl.tb_discd_3 = 1152 * 272;	/* octets */
	config->sl.N1 = 127;	/* messages */
	config->sl.N2 = 8192;	/* octets */
	config->sl.M = 5;
	ctl.ic_cmd = SL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sl);
	ctl.ic_dp = (char *) &config->sl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
	      config_sl_failed:
		printf("                                   ****ERROR: config sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__,
		       strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	ctl.ic_cmd = SL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(config->sl);
	ctl.ic_dp = (char *) &config->sl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0)
		goto config_sl_failed;
	if (config->sl.t1 != 45 * 1000
	    || config->sl.t2 != 5 * 1000
	    || config->sl.t2l != 20 * 1000
	    || config->sl.t2h != 100 * 1000
	    || config->sl.t3 != 1 * 1000
	    || config->sl.t4n != 8 * 1000
	    || config->sl.t4e != 500
	    || config->sl.t5 != 100
	    || config->sl.t6 != 4 * 1000
	    || config->sl.t7 != 1 * 1000
	    || config->sl.rb_abate != 3
	    || config->sl.rb_accept != 6
	    || config->sl.rb_discard != 9
	    || config->sl.tb_abate_1 != 128 * 272
	    || config->sl.tb_onset_1 != 256 * 272
	    || config->sl.tb_discd_1 != 384 * 272
	    || config->sl.tb_abate_2 != 512 * 272
	    || config->sl.tb_onset_2 != 640 * 272
	    || config->sl.tb_discd_2 != 768 * 272
	    || config->sl.tb_abate_3 != 896 * 272
	    || config->sl.tb_onset_3 != 1024 * 272
	    || config->sl.tb_discd_3 != 1152 * 272
	    || config->sl.N1 != 127 || config->sl.N2 != 8192 || config->sl.M != 5)
		goto config_sl_failed;
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
iut_power_off(void)
{
	do_signal(CHILD_IUT, __TEST_STOP);
	stop_tt();
	show_msus = 0;
	show_fisus = 1;
	iut_disable();
	iut_detach();
	while (wait_event(CHILD_IUT, 250) != __EVENT_NO_MSG) ;
	ioctl(test_fd[1], I_FLUSH, FLUSHRW);	/* flush IUT */
	iut_close();
	ioctl(test_fd[0], I_FLUSH, FLUSHRW);	/* flush PT */
	return __RESULT_SUCCESS;
}

static int
link_power_off(void)
{
	int ret = __RESULT_SUCCESS;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x80;
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x7f;
	if ((ret = iut_open()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_attach()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	config->opt.popt = 0;
	config->sl.t7 = 1 * 1000;
	if ((ret = iut_config()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_enable()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	sleep(10);
	// while (wait_event(child, 0) != __EVENT_NO_MSG);
	// ioctl(test_fd[0], I_FLUSH, FLUSHRW); /* flush PT */
	start_tt(10000);
	return __RESULT_SUCCESS;
}

static int
link_out_of_service(void)
{
	int ret = __RESULT_SUCCESS;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x80;
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x7f;
	if ((ret = iut_open()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_attach()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	config->opt.popt = 0;
	config->sl.t7 = 1 * 1000;
	if ((ret = iut_config()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_enable()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	// sleep(1);
	if ((ret = do_signal(CHILD_IUT, __TEST_POWER_ON)) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	// while (wait_event(child, 0) != __EVENT_NO_MSG);
	// get_event();
	// ioctl(test_fd[0], I_FLUSH, FLUSHRW); /* flush PT */
	start_tt(10000);
	return __RESULT_SUCCESS;
}
#endif

#if 0
static int
link_in_service(void)
{
	int ret = __RESULT_SUCCESS;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x80;
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x7f;
	if ((ret = iut_open()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_attach()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_config()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if ((ret = iut_enable()) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	// sleep(1);
	if ((ret = do_signal(CHILD_IUT, __TEST_POWER_ON)) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	// while (wait_event(child, 0) != __EVENT_NO_MSG);
	// get_event();
	// ioctl(test_fd[0], I_FLUSH, FLUSHRW); /* flush PT */
	start_tt(10000);
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event(CHILD_PTU))) {
			case __TEST_SIOS:
				do_signal(CHILD_PTU, __TEST_SIOS);
				do_signal(CHILD_IUT, __TEST_START);
				state = 1;
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_INCONCLUSIVE;
			default:
				break;
			}
			break;
		case 1:
			switch ((event = get_event(CHILD_PTU))) {
			case __TEST_SIOS:
				do_signal(CHILD_PTU, __TEST_SIOS);
				break;
			case __TEST_SIO:
				do_signal(CHILD_PTU, __TEST_SIO);
				state = 2;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event(CHILD_PTU))) {
			case __TEST_SIO:
				do_signal(CHILD_PTU, __TEST_SIO);
				break;
			case __TEST_SIN:
				do_signal(CHILD_PTU, __TEST_SIE);
				start_tt(config->sl.t4e * 20);
				state = 3;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event(CHILD_PTU))) {
			case __TEST_SIN:
				do_signal(CHILD_PTU, __TEST_SIE);
				break;
			case __TEST_FISU:
				do_signal(CHILD_PTU, __TEST_FISU);
				break;
			case __EVENT_IUT_IN_SERVICE:
				start_tt(10000);
				return __RESULT_SUCCESS;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		default:
			return __RESULT_INCONCLUSIVE;
		}
	}
	return __RESULT_INCONCLUSIVE;
}
#endif

#if 0
static int
link_in_service_basic(void)
{
	config->opt.popt = 0;
	config->sl.t7 = 1 * 1000;
	return link_in_service();
}

static int
link_in_service_pcr(void)
{
	config->opt.popt = SS7_POPT_PCR;
	config->sl.t7 = 2 * 1000;
	return link_in_service();
}

static int
link_in_service_basic_long_ack(void)
{
	config->opt.popt = 0;
	config->sl.t7 = 4 * 1000;
	return link_in_service();
}

static int
link_in_service_pcr_long_ack(void)
{
	config->opt.popt = SS7_POPT_PCR;
	config->sl.t7 = 8 * 1000;
	return link_in_service();
}
#endif

#if 0
static int
run_test(test_case_t * tcase)
{
	int ret = 0;
	printf(tcase->title);
	fflush(stdout);
	state = 0;
	event = 0;
	count = 0;
	tries = 0;
	beg_time = 0;
	expand = 0;
	oldmsg = 0;
	cntmsg = 0;
	oldpsb = 0;
	oldact = 0;
	cntact = 0;
	oldret = 0;
	cntret = 0;
	oldisb = 0;
	oldprm = 0;
	cntprm = 0;
	msu_len = MSU_LEN;
	if (pt_start() == __RESULT_SUCCESS) {
		printf("\nPrecondition:\n");
		fflush(stdout);
		if ((ret = (*tcase->precond) ()) == __RESULT_SUCCESS) {
			state = 0;
			event = 0;
			count = 0;
			tries = 0;
			beg_time = 0;
			expand = 0;
			printf("\nTest case:\n");
			fflush(stdout);
			ret = (*tcase->test) ();
		}
	} else
		ret = __RESULT_INCONCLUSIVE;
	switch (ret) {
	case __RESULT_SUCCESS:
		printf("                 =====SUCCESS=====\n");
		break;
	case __RESULT_FAILURE:
		printf("                 +++++__RESULT_FAILURE+++++\n");
		break;
	case __RESULT_INCONCLUSIVE:
		printf("                 ??INCONCLUSIVE???\n");
		break;
	default:
		printf("                 =+=+=+ERROR+=+=+=\n");
		break;
	}
	printf("\nPostcondition:\n");
	fflush(stdout);
	iut_power_off();
	// if (ret != __RESULT_SUCCESS)
	// pt_stats();
	pt_end();
	return ret;
}
#endif

int
iut_showmsg(struct strbuf *ctrl, struct strbuf *data)
{
	union LMI_primitives *p = (union LMI_primitives *) ctrl->buf;
	union SL_primitives *l = (union SL_primitives *) ctrl->buf;

	if (ctrl->len > 0) {
		switch (p->lmi_primitive) {
		case LMI_INFO_ACK:
		{
			int ppalen = ctrl->len - sizeof(p->info_ack);
			printf("LMI_INFO_ACK:\n");
			printf("Version = 0x%08lx\n", (ulong)p->info_ack.lmi_version);
			printf("State = %lu\n", (ulong)p->info_ack.lmi_state);
			printf("Max sdu = %lu\n", (ulong)p->info_ack.lmi_max_sdu);
			printf("Min sdu = %lu\n", (ulong)p->info_ack.lmi_min_sdu);
			printf("Header len = %lu\n", (ulong)p->info_ack.lmi_header_len);
			printf("PPA style = %lu\n", (ulong)p->info_ack.lmi_ppa_style);
			printf("PPA length = %u\n", ppalen);
			fflush(stdout);
			print_ppa(CHILD_IUT, (ppa_t *) p->info_ack.lmi_ppa_addr);
		}
			return (p->lmi_primitive);
		case LMI_OK_ACK:
		{
			printf("LMI_OK_ACK:\n");
			printf("Correct primitive = %lu\n", (ulong)p->ok_ack.lmi_correct_primitive);
			printf("State = %lu\n", (ulong)p->ok_ack.lmi_state);
			fflush(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ERROR_ACK:
		{
			printf("LMI_ERROR_ACK:\n");
			printf("Error number = %lu\n", (ulong)p->error_ack.lmi_errno);
			printf("Error string = %s\n", strerror(p->error_ack.lmi_errno));
			printf("Reason number = %lu\n", (ulong)p->error_ack.lmi_reason);
			printf("Reason string = %s\n", lmi_strreason(p->error_ack.lmi_reason));
			printf("Error primitive = %lu\n", (ulong)p->error_ack.lmi_error_primitive);
			printf("State = %lu\n", (ulong)p->error_ack.lmi_state);
			fflush(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ERROR_IND:
		{
			printf("LMI_ERROR_IND:\n");
			printf("Error number = %lu\n", (ulong)p->error_ind.lmi_errno);
			printf("Error string = %s\n", strerror(p->error_ind.lmi_errno));
			printf("Reason number = %lu\n", (ulong)p->error_ind.lmi_reason);
			printf("Reason string = %s\n", lmi_strreason(p->error_ind.lmi_reason));
			printf("State = %lu\n", (ulong)p->error_ind.lmi_state);
			fflush(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ENABLE_CON:
		{
			printf("LMI_ENABLE_CON:\n");
			printf("State = %lu\n", (ulong)p->enable_con.lmi_state);
			fflush(stdout);
		}
			return (p->lmi_primitive);
		case LMI_DISABLE_CON:
		{
			printf("LMI_DISABLE_CON:\n");
			printf("State = %lu\n", (ulong)p->enable_con.lmi_state);
			fflush(stdout);
		}
			return (p->lmi_primitive);
		default:
			switch (l->sl_primitive) {
			case SL_PDU_IND:
			{
				int i;
				char *c = data->buf;
				printf("SL_PDU_IND:\n");
				printf("  Data: ");
				for (i = 0; i < data->len; i++, c++)
					printf("%02x", *c);
				printf("\n");
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_LINK_CONGESTED_IND:
			{
				printf("SL_LINK_CONGESTED_IND:\n");
				printf("  timestamp = %lu\n", (ulong)l->link_cong_ind.sl_timestamp);
				printf("  cong stat = %lu\n", (ulong)l->link_cong_ind.sl_cong_status);
				printf("  disc stat = %lu\n", (ulong)l->link_cong_ind.sl_disc_status);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_LINK_CONGESTION_CEASED_IND:
			{
				printf("SL_LINK_CONGESTION_CEASED_IND:\n");
				printf("  timestamp = %lu\n", (ulong)l->link_cong_ceased_ind.sl_timestamp);
				printf("  cong stat = %lu\n", (ulong)l->link_cong_ceased_ind.sl_cong_status);
				printf("  disc stat = %lu\n", (ulong)l->link_cong_ceased_ind.sl_disc_status);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_RETRIEVED_MESSAGE_IND:
			{
				int i;
				char *c = data->buf;
				printf("SL_RETRIEVED_MESSAGE_IND:\n");
				printf("  Data: ");
				for (i = 0; i < data->len; i++, c++)
					printf("%02x", *c);
				printf("\n");
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_RB_CLEARED_IND:
			{
				printf("SL_RB_CLEARED_IND\n");
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_BSNT_IND:
			{
				printf("SL_BSNT_IND:\n");
				printf("  bsnt = %lu\n", (ulong)l->bsnt_ind.sl_bsnt);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_IN_SERVICE_IND:
			{
				printf("SL_IN_SERVICE_IND\n");
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_OUT_OF_SERVICE_IND:
			{
				printf("SL_OUT_OF_SERVICE_IND:\n");
				printf("  timestamp = %lu\n", (ulong)l->out_of_service_ind.sl_timestamp);
				printf("  reason    = %lu\n", (ulong)l->out_of_service_ind.sl_reason);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			{
				printf("SL_REMOTE_PROCESSOR_OUTAGE_IND:\n");
				printf("  timestamp = %lu\n", (ulong)l->rem_proc_out_ind.sl_timestamp);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			{
				printf("SL_REMOTE_PROCESSOR_RECOVERED_IND:\n");
				printf("  timestamp = %lu\n", (ulong)l->rem_proc_recovered_ind.sl_timestamp);
				fflush(stdout);
			}
				return (l->sl_primitive);
			case SL_RTB_CLEARED_IND:
			{
				printf("SL_RTB_CLEARED_IND\n");
				fflush(stdout);
			}
				return (l->sl_primitive);
			default:
			{
				printf("Unrecognized primitive %lu!\n", (ulong)l->sl_primitive);
				fflush(stdout);
			}
				return (l->sl_primitive);
			}
		}
	}
	return (0);
}
/*
 *  =========================================================================
 *
 *  The test cases...
 *
 *  =========================================================================
 */

struct test_stream {
	int (*preamble) (int);		/* test preamble */
	int (*testcase) (int);		/* test case */
	int (*postamble) (int);		/* test postamble */
};

static int
check_snibs(int child, unsigned char bsnib, unsigned char fsnib)
{
	print_rx_msg_sn(child, "check b/f sn/ib", bsnib, fsnib);
	if ((bib[1] | bsn[1]) == bsnib && (fib[1] | fsn[1]) == fsnib)
		return __RESULT_SUCCESS;
	return __RESULT_FAILURE;
}

/*
 *  Check test case guard timer.
 */
#define test_group_0 "0. Sanity checks"
#define test_group_0_1 "0.1. Guard timers"
#define tgrp_case_0_1 test_group_0
#define sgrp_case_0_1 test_group_0_1
#define numb_case_0_1 "0.1"
#define name_case_0_1 "Check test case guard timer."
#define xtra_case_0_1 NULL
#define sref_case_0_1 "(none)"
#define desc_case_0_1 "\
Checks that the test case guard timer will fire and bring down the children."

int
test_0_1_ptu(int child)
{
	if (test_duration == INFINITE_WAIT)
		return __RESULT_NOTAPPL;
	test_msleep(child, test_duration + 1000);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_0_1_iut(int child)
{
	if (test_duration == INFINITE_WAIT)
		return __RESULT_NOTAPPL;
	test_msleep(child, test_duration + 1000);
	state++;
	return (__RESULT_SUCCESS);
}

struct test_stream test_case_0_1_ptu = { preamble_none, &test_0_1_ptu, postamble_none };
struct test_stream test_case_0_1_iut = { preamble_none, &test_0_1_iut, postamble_none };

#define test_case_0_1 { &test_case_0_1_ptu, &test_case_0_1_iut, NULL }

#define test_group_0_2 "0.2. Preambles and Postambles"
#define tgrp_case_0_2_1 test_group_0
#define sgrp_case_0_2_1 test_group_0_2
#define numb_case_0_2_1 "0.2.1"
#define name_case_0_2_1 "Base preamble, base postamble."
#define xtra_case_0_2_1 NULL
#define sref_case_0_2_1 "(none)"
#define desc_case_0_2_1 "\
Checks that two Streams can be opened and closed."

int
test_0_2_1_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_1_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_1_ptu = { preamble_none, test_0_2_1_ptu, postamble_none };
struct test_stream test_case_0_2_1_iut = { preamble_none, test_0_2_1_iut, postamble_none };

#define test_case_0_2_1 { &test_case_0_2_1_ptu, &test_case_0_2_1_iut, NULL }

#define tgrp_case_0_2_2 test_group_0
#define sgrp_case_0_2_2 test_group_0_2
#define numb_case_0_2_2 "0.2.2"
#define name_case_0_2_2 "Unbound preamble, unbound postamble."
#define xtra_case_0_2_2 NULL
#define sref_case_0_2_2 "(none)"
#define desc_case_0_2_2 "\
Checks that two Streams can be opened, information obtained, and closed."

int
test_0_2_2_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_2_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_2_ptu = { preamble_unbound, test_0_2_2_ptu, postamble_unbound };
struct test_stream test_case_0_2_2_iut = { preamble_unbound, test_0_2_2_iut, postamble_unbound };

#define test_case_0_2_2 { &test_case_0_2_2_ptu, &test_case_0_2_2_iut, NULL }

#define tgrp_case_0_2_3 test_group_0
#define sgrp_case_0_2_3 test_group_0_2
#define numb_case_0_2_3 "0.2.3"
#define name_case_0_2_3 "Push preamble, pop postamble."
#define xtra_case_0_2_3 NULL
#define sref_case_0_2_3 "(none)"
#define desc_case_0_2_3 "\
Checks that two Streams can be opened, information obtained, module\n\
pushed, popped, and closed."

int
test_0_2_3_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_3_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_3_ptu = { preamble_push, test_0_2_3_ptu, postamble_pop };
struct test_stream test_case_0_2_3_iut = { preamble_push, test_0_2_3_iut, postamble_pop };

#define test_case_0_2_3 { &test_case_0_2_3_ptu, &test_case_0_2_3_iut, NULL }

#define tgrp_case_0_2_4 test_group_0
#define sgrp_case_0_2_4 test_group_0_2
#define numb_case_0_2_4 "0.2.4"
#define name_case_0_2_4 "Config preamble, pop postamble."
#define xtra_case_0_2_4 NULL
#define sref_case_0_2_4 "(none)"
#define desc_case_0_2_4 "\
Checks that two Streams can be opened, information obtained, module\n\
pushed, configured, popped and closed."

int
test_0_2_4_ptu(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

int
test_0_2_4_iut(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_4_ptu = { preamble_config, test_0_2_4_ptu, postamble_pop };
struct test_stream test_case_0_2_4_iut = { preamble_config, test_0_2_4_iut, postamble_pop };

#define test_case_0_2_4 { &test_case_0_2_4_ptu, &test_case_0_2_4_iut, NULL }

#define tgrp_case_0_2_5 test_group_0
#define sgrp_case_0_2_5 test_group_0_2
#define numb_case_0_2_5 "0.2.5"
#define name_case_0_2_5 "Attach preamble, detach postamble."
#define xtra_case_0_2_5 NULL
#define sref_case_0_2_5 "(none)"
#define desc_case_0_2_5 "\
Checks that two Streams can be opened, information obtained, module\n\
pushed, configured, bound/attached, unbound/detached, module popped,\n\
and closed."

int
test_0_2_5_ptu(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

int
test_0_2_5_iut(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_5_ptu = { preamble_attach, test_0_2_5_ptu, postamble_detach };
struct test_stream test_case_0_2_5_iut = { preamble_attach, test_0_2_5_iut, postamble_detach };

#define test_case_0_2_5 { &test_case_0_2_5_ptu, &test_case_0_2_5_iut, NULL }

#define tgrp_case_0_2_6 test_group_0
#define sgrp_case_0_2_6 test_group_0_2
#define numb_case_0_2_6 "0.2.6"
#define name_case_0_2_6 "Enable preamble, disable postamble."
#define xtra_case_0_2_6 NULL
#define sref_case_0_2_6 "(none)"
#define desc_case_0_2_6 "\
Checks that two Streams can be opened, information obtained, module\n\
pushed, configured, bound/attached, connected/enabled, disconnected/\n\
disabled, unbound/detached, module popped and closed."

int
test_0_2_6_ptu(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

int
test_0_2_6_iut(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_6_ptu = { preamble_enable, test_0_2_6_ptu, postamble_disable };
struct test_stream test_case_0_2_6_iut = { preamble_enable, test_0_2_6_iut, postamble_disable };

#define test_case_0_2_6 { &test_case_0_2_6_ptu, &test_case_0_2_6_iut, NULL }

#define tgrp_case_0_2_7 test_group_0
#define sgrp_case_0_2_7 test_group_0_2
#define numb_case_0_2_7 "0.2.7"
#define name_case_0_2_7 "Link power on preamble, out of service postamble."
#define xtra_case_0_2_7 NULL
#define sref_case_0_2_7 "(none)"
#define desc_case_0_2_7 "\
Checks that two Streams can be opened, information obtained, module\n\
pushed, configured, bound/attached, connected/enabled, link power on,\n\
disconnected/disabled, unbound/detached, module popped and closed."

int
test_0_2_7_ptu(int child)
{
	if (expect(child, INFINITE_WAIT, __TEST_SIOS))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIOS))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
failure:
	return __RESULT_FAILURE;
}

int
test_0_2_7_iut(int child)
{
	test_sleep(child, 1);
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_7_ptu = { preamble_link_power_on, test_0_2_7_ptu, postamble_link_out_of_service };
struct test_stream test_case_0_2_7_iut = { preamble_link_power_on, test_0_2_7_iut, postamble_link_out_of_service };

#define test_case_0_2_7 { &test_case_0_2_7_ptu, &test_case_0_2_7_iut, NULL }

#define test_group_1 "1. Link State Control"
#define test_subgroup_1 "Expected signal units/orders"
#define tgrp_case_1_1a test_group_1
#define sgrp_case_1_1a test_subgroup_1
#define sref_case_1_1a "Q.781/1.1"
#define numb_case_1_1a "1.1(a)"
#define name_case_1_1a "Initialization (power-up)"
#define xtra_case_1_1a "Forward"
#define desc_case_1_1a "\
Arrange the IUT to power on and send an SIOS message.  Power on the IUT\n\
and check that the IUT sends an SIOS message.  Check that the IUT\n\
maintains the \"Out of Service\" state."
static int
test_power_on_pt(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, __TEST_SIOS) || check_snibs(child, 0xff, 0xff))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIOS))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_1a_ptu(int child)
{
	if (test_power_on_pt(child))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_1a_iut(int child)
{
	if (test_power_on_pt(child))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_1a_ptu = { preamble_link_power_off, test_1_1a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_1a_iut = { preamble_link_power_off, test_1_1a_iut, postamble_link_out_of_service };

#define test_case_1_1a { &test_case_1_1a_ptu, &test_case_1_1a_iut, NULL }

#define tgrp_case_1_1b test_group_1
#define sgrp_case_1_1b test_subgroup_1
#define sref_case_1_1b "Q.781/1.1"
#define numb_case_1_1b "1.1(b)"
#define name_case_1_1b "Initialization (power-up)"
#define xtra_case_1_1b "Reverse"
#define desc_case_1_1b "\
Power on the IUT and check that the IUT sends an SIOS message.  Arrange\n\
the IUT to power on and send an SIOS message.  Check that the IUT\n\
maintains the \"Out of Service\" state."

static int
test_power_on_sut(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, __TEST_SIOS) || check_snibs(child, 0xff, 0xff))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIOS))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (do_signal(child, __TEST_POWER_ON))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_1b_ptu(int child)
{
	if (test_power_on_sut(child))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_1b_iut(int child)
{
	if (test_power_on_sut(child))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_1b_ptu = { preamble_link_power_off, test_1_1b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_1b_iut = { preamble_link_power_off, test_1_1b_iut, postamble_link_out_of_service };

#define test_case_1_1b { &test_case_1_1b_ptu, &test_case_1_1b_iut, NULL }


#define tgrp_case_1_2 test_group_1
#define sgrp_case_1_2 test_subgroup_1
#define sref_case_1_2 "Q.781/1.2"
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Timer T2"
#define xtra_case_1_2 NULL
#define desc_case_1_2 "\
Arrange the IUT to send an SIO message to initiate alignment.  Arrange\n\
the PT to not respond to the SIO message.  When the T2 timer expires at\n\
the IUT, SIOS should be received by the PT.  The timer T2 duration can\n\
be measured as the time between the PT receiving SIO and the PT\n\
receiving SIOS."
static int
test_out_of_service_sut(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (expect(child, INFINITE_WAIT, __TEST_SIOS) || check_snibs(child, 0xff, 0xff))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIOS))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_aligned_sut(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (test_out_of_service_sut(child))
			goto failure;
		state++;
		for (;;) {
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				state++;
				continue;
			case __TEST_SIO:
				break;
			default:
				goto failure;
			}
			break;
		}
		state++;
		break;
	case CHILD_IUT:
		if (test_out_of_service_sut(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_2_ptu(int child)
{
	long beg_time = 0;

	if (test_aligned_sut(child))
		goto failure;
	state++;
	beg_time = milliseconds(child, t2);
	start_tt(config->sl.t2 * 2);
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIO:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (check_time(child, "T2  ", beg_time, timer[t2].lo, timer[t2].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_2_iut(int child)
{
	if (test_aligned_sut(child))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_2_ptu = { preamble_link_power_on, test_1_2_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_2_iut = { preamble_link_power_on, test_1_2_iut, postamble_link_out_of_service };

#define test_case_1_2 { &test_case_1_2_ptu, &test_case_1_2_iut, NULL }

#define tgrp_case_1_3 test_group_1
#define sgrp_case_1_3 test_subgroup_1
#define sref_case_1_3 "Q.781/1.3"
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Timer T3"
#define xtra_case_1_3 NULL
#define desc_case_1_3 "\
Arrange the IUT to send an SIO message to initiate alignment.  Arrange\n\
the PT to send an SIO message in response, but to not send proving\n\
messages.  When the T3 timer expires at the IUT, SIOS should be received\n\
by the PT.  The timer T3 duration can be measured as the time between\n\
the PT sending SIO and the IUT sending SIOS."
static int
test_proving_sut(int child, int proving)
{
	switch (child) {
	case CHILD_PTU:
		if (test_aligned_sut(child))
			goto failure;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIO))
			goto failure;
		state++;
		switch (proving) {
		case 0:
			break;
		case 1:
		case 2:
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIO))
						goto failure;
					state++;
					continue;
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					state++;
					break;
				default:
					goto failure;
				}
				break;
			}
		default:
			return __RESULT_SCRIPT_ERROR;
		}
		break;
	case CHILD_IUT:
		if (test_aligned_sut(child))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_3_ptu(int child)
{
	long beg_time = 0;

	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	beg_time = milliseconds(child, t3);
	start_tt(config->sl.t3 * 2);
	state++;
	for (;;) {
		switch(get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIE:
		case __TEST_SIN:
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (check_time(child, "T3  ", beg_time, timer[t3].lo, timer[t3].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_3_iut(int child)
{
	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_3_ptu = { preamble_link_power_on, test_1_3_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_3_iut = { preamble_link_power_on, test_1_3_iut, postamble_link_out_of_service };

#define test_case_1_3 { &test_case_1_3_ptu, &test_case_1_3_iut, NULL }

#define tgrp_case_1_4 test_group_1
#define sgrp_case_1_4 test_subgroup_1
#define sref_case_1_4 "Q.781/1.4"
#define numb_case_1_4 "1.4"
#define name_case_1_4 "Timer T1 & Timer T4"
#define xtra_case_1_4 "Normal"
#define desc_case_1_4 "\
Arrange the IUT to start the link and send an SIO.  Arrange the PT to\n\
respond with SIO, normal proving messages, SIN, but to not issue the\n\
FISU or MSU necessary to complete alignment.  T4 is the period from the\n\
first SIN received from the IUT to the first FISU or MSU received from\n\
the IUT.  T1 is the period form the first FISU or MSU received from the\n\
IUT until the SIOS received from the IUT."
static int
test_1_4_ptu(int child)
{
	long beg_time = 0;

	if (test_aligned_sut(child))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIO))
		goto failure;
	state++;
	beg_time = milliseconds(child, t4n);
	start_tt(config->sl.t4n * 2);
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIN:
			if (do_signal(child, last_event))
				goto failure;
			state++;
			print_less(child);
			continue;
		case __TEST_FISU:
			state++;
			print_more(child);
			break;
		default:
			goto failure;
		}
		break;
	}
	if (check_time(child, "T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
		goto failure;
	beg_time = milliseconds(child, t1);
	start_tt(config->sl.t1 * 2);
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (check_time(child, "T1  ", beg_time, timer[t1].lo, timer[t1].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_4_iut(int child)
{
	if (test_aligned_sut(child))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_4_ptu = { preamble_link_power_on, test_1_4_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_4_iut = { preamble_link_power_on, test_1_4_iut, postamble_link_out_of_service };

#define test_case_1_4 { &test_case_1_4_ptu, &test_case_1_4_iut, NULL }

#define desc_case_1_5a "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\
"
static int
test_out_of_service_pt(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (expect(child, INFINITE_WAIT, __TEST_SIOS))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIOS))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_aligned_pt(int child)
{
	switch (child) {
	case CHILD_PTU:
		if (test_out_of_service_pt(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIO))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (test_out_of_service_pt(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_alignment_pt(int child, int proving, int signal, int result)
{
	int signalled = 0;

	switch (child) {
	case CHILD_PTU:
		if (test_aligned_pt(child))
			goto failure;
		state++;
		for (;;) {
			switch(get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				state++;
				continue;
			case __TEST_SIO:
				break;
			default:
				goto failure;
			}
			break;
		}
		state++;
		switch (proving) {
		case 0:
			if (do_signal(child, signal))
				goto failure;
			state++;
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, signal))
						goto failure;
					state++;
					continue;
				default:
					if (last_event == result)
						break;
					goto failure;
				}
				break;
			}
			state++;
			break;
		case 1:
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			for (;;) {
				switch (get_event(child)) {
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (!signalled) {
						if (do_signal(child, signal))
							goto failure;
						signalled = 1;
						state++;
						print_less(child);
					}
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			if (do_signal(child, signal))
				goto failure;
			break;
		case 2:
			if (do_signal(child, __TEST_SIE))
				goto failure;
			state++;
			for (;;) {
				switch (get_event(child)) {
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (!signalled) {
						if (do_signal(child, signal))
							goto failure;
						signalled = 1;
						state++;
						print_less(child);
					}
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
		break;
	case CHILD_IUT:
		if (test_aligned_pt(child))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, result))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_alignment_sut(int child, int proving, int result)
{
	switch (child) {
	case CHILD_PTU:
		if (test_aligned_sut(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		if (do_signal(child, __TEST_SIO))
			goto failure;
		state++;
		switch (proving) {
		case 0:
			for (;;) {
				switch(get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIO))
						goto failure;
					state++;
					continue;
				default:
					if (last_event == result)
						break;
					goto failure;
				}
				break;
			}
			state++;
			break;
		case 1:
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIN))
						goto failure;
					state++;
					continue;
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (do_signal(child, __TEST_SIN))
						goto failure;
					state++;
					// print_less(child);
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			break;
		case 2:
			if (do_signal(child, __TEST_SIE))
				goto failure;
			state++;
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIN))
						goto failure;
					state++;
					continue;
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (do_signal(child, __TEST_SIE))
						goto failure;
					state++;
					// print_less(child);
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
		break;
	case CHILD_IUT:
		if (test_aligned_sut(child))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, result))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_5a_ptu(int child, int proving)
{
	if (test_alignment_pt(child, proving, __TEST_FISU, __TEST_FISU))
		goto failure;
	state++;
	if (expect(child, LONGEST_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_5a_iut(int child, int proving)
{
	if (test_alignment_pt(child, proving, 0, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define desc_case_1_5b "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\
"
static int
test_1_5b_ptu(int child, int proving)
{
	if (test_alignment_sut(child, proving, __TEST_FISU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_5b_iut(int child, int proving)
{
	if (test_alignment_sut(child, proving, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_5a_p test_group_1
#define sgrp_case_1_5a_p test_subgroup_1
#define sref_case_1_5a_p "Q.781/1.5"
#define numb_case_1_5a_p "1.5(a)"
#define name_case_1_5a_p "Normal alignment procedure"
#define xtra_case_1_5a_p "Forward - Proving"
#define desc_case_1_5a_p "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\n\
With proving\
"
static int
test_1_5a_p_ptu(int child)
{
	return test_1_5a_ptu(child, 1);
}
static int
test_1_5a_p_iut(int child)
{
	return test_1_5a_iut(child, 1);
}

static struct test_stream test_case_1_5a_p_ptu = { preamble_link_power_on, test_1_5a_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_5a_p_iut = { preamble_link_power_on, test_1_5a_p_iut, postamble_link_in_service };

#define test_case_1_5a_p { &test_case_1_5a_p_ptu, &test_case_1_5a_p_iut, NULL }

#define tgrp_case_1_5b_p test_group_1
#define sgrp_case_1_5b_p test_subgroup_1
#define sref_case_1_5b_p "Q.781/1.5"
#define numb_case_1_5b_p "1.5(b)"
#define name_case_1_5b_p "Normal alignment procedure"
#define xtra_case_1_5b_p "Reverse - Proving"
#define desc_case_1_5b_p "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\n\
With proving\
"
static int
test_1_5b_p_ptu(int child)
{
	return test_1_5b_ptu(child, 1);
}
static int
test_1_5b_p_iut(int child)
{
	return test_1_5b_iut(child, 1);
}

static struct test_stream test_case_1_5b_p_ptu = { preamble_link_power_on, test_1_5b_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_5b_p_iut = { preamble_link_power_on, test_1_5b_p_iut, postamble_link_in_service };

#define test_case_1_5b_p { &test_case_1_5b_p_ptu, &test_case_1_5b_p_iut, NULL }

#define tgrp_case_1_5a_np test_group_1
#define sgrp_case_1_5a_np test_subgroup_1
#define sref_case_1_5a_np "Q781/1.5"
#define numb_case_1_5a_np "1.5(a)np"
#define name_case_1_5a_np "Normal alignment procedure"
#define xtra_case_1_5a_np "Forward - No Proving"
#define desc_case_1_5a_np "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\n\
Without proving\
"
static int
test_1_5a_np_ptu(int child)
{
	return test_1_5a_ptu(child, 0);
}
static int
test_1_5a_np_iut(int child)
{
	return test_1_5a_iut(child, 0);
}

static struct test_stream test_case_1_5a_np_ptu = { preamble_link_power_on_np, test_1_5a_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_5a_np_iut = { preamble_link_power_on_np, test_1_5a_np_iut, postamble_link_in_service };

#define test_case_1_5a_np { &test_case_1_5a_np_ptu, &test_case_1_5a_np_iut, NULL }

#define tgrp_case_1_5b_np test_group_1
#define sgrp_case_1_5b_np test_subgroup_1
#define sref_case_1_5b_np "Q.781/1.5"
#define numb_case_1_5b_np "1.5(b)np"
#define name_case_1_5b_np "Normal alignment procedure"
#define xtra_case_1_5b_np "Reverse - No Proving"
#define desc_case_1_5b_np "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\n\
With outproving\
"
static int
test_1_5b_np_ptu(int child)
{
	return test_1_5b_ptu(child, 0);
}
static int
test_1_5b_np_iut(int child)
{
	return test_1_5b_iut(child, 0);
}

static struct test_stream test_case_1_5b_np_ptu = { preamble_link_power_on_np, test_1_5b_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_5b_np_iut = { preamble_link_power_on_np, test_1_5b_np_iut, postamble_link_in_service };

#define test_case_1_5b_np { &test_case_1_5b_np_ptu, &test_case_1_5b_np_iut, NULL }

#define desc_case_1_6 "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure - correct procedure (MSU)\
"
static int
test_1_6_ptu(int child, int proving)
{
	if (test_alignment_sut(child, proving, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_ACK))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_6_iut(int child, int proving)
{
	if (test_alignment_sut(child, proving, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_6_p test_group_1
#define sgrp_case_1_6_p test_subgroup_1
#define sref_case_1_6_p "Q.781/1.6"
#define numb_case_1_6_p "1.6"
#define name_case_1_6_p "Normal alignment procedure"
#define xtra_case_1_6_p "MSU - Proving"
#define desc_case_1_6_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure = correct procedure (MSU)\n\
With proving\
"
static int
test_1_6_p_ptu(int child)
{
	return test_1_6_ptu(child, 1);
}
static int
test_1_6_p_iut(int child)
{
	return test_1_6_iut(child, 1);
}

static struct test_stream test_case_1_6_p_ptu = { preamble_link_power_on, test_1_6_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_6_p_iut = { preamble_link_power_on, test_1_6_p_iut, postamble_link_in_service };

#define test_case_1_6_p { &test_case_1_6_p_ptu, &test_case_1_6_p_iut, NULL }

#define tgrp_case_1_6_np test_group_1
#define sgrp_case_1_6_np test_subgroup_1
#define sref_case_1_6_np "Q.781/1.6"
#define numb_case_1_6_np "1.6np"
#define name_case_1_6_np "Normal alignment procedure"
#define xtra_case_1_6_np "MSU - No Proving"
#define desc_case_1_6_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure = correct procedure (MSU)\n\
Without proving\
"
static int
test_1_6_np_ptu(int child)
{
	return test_1_6_ptu(child, 0);
}
static int
test_1_6_np_iut(int child)
{
	return test_1_6_iut(child, 0);
}

static struct test_stream test_case_1_6_np_ptu = { preamble_link_power_on_np, test_1_6_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_6_np_iut = { preamble_link_power_on_np, test_1_6_np_iut, postamble_link_in_service };

#define test_case_1_6_np { &test_case_1_6_np_ptu, &test_case_1_6_np_iut, NULL }

#define tgrp_case_1_7 test_group_1
#define sgrp_case_1_7 test_subgroup_1
#define sref_case_1_7 "Q.781/1.7"
#define numb_case_1_7 "1.7"
#define name_case_1_7 "SIO received during normal proving period"
#define xtra_case_1_7 NULL
#define desc_case_1_7 "\
Link State Control - Expected signal units/orders\n\
SIO received during normal proving period\
"
static int
test_1_7_ptu(int child)
{
	long beg_time = 0;

	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIN))
		goto failure;
	state++;
	start_tt(config->sl.t4n / 2);
	state++;
	show_timeout = 1;
	for (;;) {
		switch (get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIN:
			if (do_signal(child, last_event))
				goto failure;
			state++;
			print_less(child);
			continue;
		case __EVENT_TIMEOUT:
			start_tt(config->sl.t4n * 2);
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			beg_time = milliseconds(child, t4n);
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			break;
		default:
			goto failure;
		}
		break;
	}
	for (;;) {
		switch (get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIN:
			if (do_signal(child, last_event))
				goto failure;
			state++;
			print_less(child);
			continue;
		case __TEST_FISU:
			state++;
			print_more(child);
			if (check_time(child, "T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
				goto failure;
			state++;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			break;
		default:
			goto failure;
		}
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_7_iut(int child)
{
	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_7_ptu = { preamble_link_power_on, test_1_7_ptu, postamble_link_in_service };
static struct test_stream test_case_1_7_iut = { preamble_link_power_on, test_1_7_iut, postamble_link_in_service };

#define test_case_1_7 { &test_case_1_7_ptu, &test_case_1_7_iut, NULL }

#define desc_case_1_8a "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\
"
static int
test_alignment_lpo_sut(int child, int proving, int result)
{
	switch (child) {
	case CHILD_PTU:
		if (test_alignment_sut(child, proving, result))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (do_signal(child, __TEST_LPO))
			goto failure;
		state++;
		if (test_alignment_sut(child, proving, result))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;

}
static int
test_1_8a_ptu(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __TEST_SIPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_8a_iut(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define desc_case_1_8b "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\
"
static int
test_alignment_lpo_pt(int child, int proving, int result)
{
	switch (child) {
	case CHILD_PTU:
		if (test_out_of_service_pt(child))
			goto failure;
		state++;
		if (do_signal(child, __TEST_LPO))
			goto failure;
		state++;
		if (do_signal(child, __TEST_START))
			goto failure;
		state++;
		for (;;) {
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				state++;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				state++;
				break;
			default:
				goto failure;
			}
			break;
		}
		state++;
		/* wait to allow IUT to queue data, for test 1.9(b)np */
		test_msleep(child, LONG_WAIT);
		state++;
		if (do_signal(child, __TEST_SIO))
			goto failure;
		state++;
		switch (proving) {
		case 0:
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIO))
						goto failure;
					state++;
					continue;
				default:
					if (last_event == result)
						break;
					goto failure;
				}
				break;
			}
			state++;
			break;
		case 1:
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIO))
						goto failure;
					state++;
					continue;
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (do_signal(child, __TEST_SIN))
						goto failure;
					state++;
					print_less(child);
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			break;
		case 2:
			for (;;) {
				switch (get_event(child)) {
				case __TEST_SIO:
					if (do_signal(child, __TEST_SIO))
						goto failure;
					state++;
					continue;
				case __MSG_PROVING:
				case __TEST_SIE:
				case __TEST_SIN:
					if (do_signal(child, __TEST_SIE))
						goto failure;
					state++;
					print_less(child);
					continue;
				default:
					if (last_event == result) {
						state++;
						print_more(child);
						break;
					}
					goto failure;
				}
				break;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
		break;
	case CHILD_IUT:
		if (test_aligned_pt(child))
			goto failure;
		state++;
		if (expect(child, INFINITE_WAIT, result))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_8b_ptu(int child, int proving)
{
	if (test_alignment_lpo_pt(child, proving, __TEST_FISU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIPO))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_8b_iut(int child, int proving)
{
	if (test_alignment_lpo_pt(child, proving, __EVENT_IUT_RPO))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_8a_p test_group_1
#define sgrp_case_1_8a_p test_subgroup_1
#define sref_case_1_8a_p "Q.781/1.8"
#define numb_case_1_8a_p "1.8(a)"
#define name_case_1_8a_p "Normal alignment PO set"
#define xtra_case_1_8a_p "FISU - Forward - Proving"
#define desc_case_1_8a_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\n\
With proving\
"
static int
test_1_8a_p_ptu(int child)
{
	return test_1_8a_ptu(child, 1);
}
static int
test_1_8a_p_iut(int child)
{
	return test_1_8a_iut(child, 1);
}

static struct test_stream test_case_1_8a_p_ptu = { preamble_link_power_on, test_1_8a_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_8a_p_iut = { preamble_link_power_on, test_1_8a_p_iut, postamble_link_in_service };

#define test_case_1_8a_p { &test_case_1_8a_p_ptu, &test_case_1_8a_p_iut, NULL }

#define tgrp_case_1_8b_p test_group_1
#define sgrp_case_1_8b_p test_subgroup_1
#define sref_case_1_8b_p "Q.781/1.8"
#define numb_case_1_8b_p "1.8(b)"
#define name_case_1_8b_p "Normal alignment PO set"
#define xtra_case_1_8b_p "FISU - Reverse - Proving"
#define desc_case_1_8b_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\n\
With proving\
"
static int
test_1_8b_p_ptu(int child)
{
	return test_1_8b_ptu(child, 1);
}
static int
test_1_8b_p_iut(int child)
{
	return test_1_8b_iut(child, 1);
}

static struct test_stream test_case_1_8b_p_ptu = { preamble_link_power_on, test_1_8b_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_8b_p_iut = { preamble_link_power_on, test_1_8b_p_iut, postamble_link_in_service };

#define test_case_1_8b_p { &test_case_1_8b_p_ptu, &test_case_1_8b_p_iut, NULL }

#define tgrp_case_1_8a_np test_group_1
#define sgrp_case_1_8a_np test_subgroup_1
#define sref_case_1_8a_np "Q.781/1.8"
#define numb_case_1_8a_np "1.8(a)np"
#define name_case_1_8a_np "Normal alignment PO set"
#define xtra_case_1_8a_np "FISU - Forward - No Proving"
#define desc_case_1_8a_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\n\
Without proving\
"
static int
test_1_8a_np_ptu(int child)
{
	return test_1_8a_ptu(child, 0);
}
static int
test_1_8a_np_iut(int child)
{
	return test_1_8a_iut(child, 0);
}

static struct test_stream test_case_1_8a_np_ptu = { preamble_link_power_on_np, test_1_8a_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_8a_np_iut = { preamble_link_power_on_np, test_1_8a_np_iut, postamble_link_in_service };

#define test_case_1_8a_np { &test_case_1_8a_np_ptu, &test_case_1_8a_np_iut, NULL }

#define tgrp_case_1_8b_np test_group_1
#define sgrp_case_1_8b_np test_subgroup_1
#define sref_case_1_8b_np "Q.781/1.8"
#define numb_case_1_8b_np "1.8(b)np"
#define name_case_1_8b_np "Normal alignemnt PO set"
#define xtra_case_1_8b_np "FISU - Reverse - No Proving"
#define desc_case_1_8b_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\n\
Without proving\
"
static int
test_1_8b_np_ptu(int child)
{
	return test_1_8b_ptu(child, 0);
}
static int
test_1_8b_np_iut(int child)
{
	return test_1_8b_iut(child, 0);
}

static struct test_stream test_case_1_8b_np_ptu = { preamble_link_power_on_np, test_1_8b_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_8b_np_iut = { preamble_link_power_on_np, test_1_8b_np_iut, postamble_link_in_service };

#define test_case_1_8b_np { &test_case_1_8b_np_ptu, &test_case_1_8b_np_iut, NULL }

#define desc_case_1_9a "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\
"
static int
test_1_9a_ptu(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __TEST_SIPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA))
		goto failure;
	state++;
	if (start_tt(config->sl.t7 * 2))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __EVENT_TIMEOUT:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_SIOS))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_9a_iut(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define desc_case_1_9b "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\
"
static int
test_1_9b_ptu(int child, int proving)
{
	if (test_alignment_lpo_pt(child, proving, __TEST_DATA))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIPO))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_9b_iut(int child, int proving)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_9a_p test_group_1
#define sgrp_case_1_9a_p test_subgroup_1
#define sref_case_1_9a_p "Q.781/1.9"
#define numb_case_1_9a_p "1.9(a)"
#define name_case_1_9a_p "Normal alignment PO set"
#define xtra_case_1_9a_p "MSU - Forward - Proving"
#define desc_case_1_9a_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\n\
With proving\
"
static int
test_1_9a_p_ptu(int child)
{
	return test_1_9a_ptu(child, 1);
}
static int
test_1_9a_p_iut(int child)
{
	return test_1_9a_iut(child, 1);
}

static struct test_stream test_case_1_9a_p_ptu = { preamble_link_power_on, test_1_9a_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_9a_p_iut = { preamble_link_power_on, test_1_9a_p_iut, postamble_link_out_of_service };

#define test_case_1_9a_p { &test_case_1_9a_p_ptu, &test_case_1_9a_p_iut, NULL }

#define tgrp_case_1_9b_p test_group_1
#define sgrp_case_1_9b_p test_subgroup_1
#define sref_case_1_9b_p "Q.781/1.9"
#define numb_case_1_9b_p "1.9(b)"
#define name_case_1_9b_p "Normal alignment PO set"
#define xtra_case_1_9b_p "MSU - Reverse - Proving"
#define desc_case_1_9b_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\n\
With proving\
"
static int
test_1_9b_p_ptu(int child)
{
	return test_1_9b_ptu(child, 1);
}
static int
test_1_9b_p_iut(int child)
{
	return test_1_9b_iut(child, 1);
}

static struct test_stream test_case_1_9b_p_ptu = { preamble_link_power_on, test_1_9b_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_9b_p_iut = { preamble_link_power_on, test_1_9b_p_iut, postamble_link_out_of_service };

#define test_case_1_9b_p { &test_case_1_9b_p_ptu, &test_case_1_9b_p_iut, NULL }

#define tgrp_case_1_9a_np test_group_1
#define sgrp_case_1_9a_np test_subgroup_1
#define sref_case_1_9a_np "Q.781/1.9"
#define numb_case_1_9a_np "1.9(a)np"
#define name_case_1_9a_np "Normal alignment PO set"
#define xtra_case_1_9a_np "MSU - Forward - No Proving"
#define desc_case_1_9a_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\n\
Without proving\
"
static int
test_1_9a_np_ptu(int child)
{
	return test_1_9a_ptu(child, 0);
}
static int
test_1_9a_np_iut(int child)
{
	return test_1_9a_iut(child, 0);
}

static struct test_stream test_case_1_9a_np_ptu = { preamble_link_power_on_np, test_1_9a_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_9a_np_iut = { preamble_link_power_on_np, test_1_9a_np_iut, postamble_link_out_of_service };

#define test_case_1_9a_np { &test_case_1_9a_np_ptu, &test_case_1_9a_np_iut, NULL }

#define tgrp_case_1_9b_np test_group_1
#define sgrp_case_1_9b_np test_subgroup_1
#define sref_case_1_9b_np "Q.781/1.9"
#define numb_case_1_9b_np "1.9(b)np"
#define name_case_1_9b_np "Normal alignment PO set"
#define xtra_case_1_9b_np "MSU - Reverse - No Proving"
#define desc_case_1_9b_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\n\
Without proving\
"
static int
test_1_9b_np_ptu(int child)
{
	return test_1_9b_ptu(child, 0);
}
static int
test_1_9b_np_iut(int child)
{
	return test_1_9b_iut(child, 0);
}

static struct test_stream test_case_1_9b_np_ptu = { preamble_link_power_on_np, test_1_9b_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_9b_np_iut = { preamble_link_power_on_np, test_1_9b_np_iut, postamble_link_out_of_service };

#define test_case_1_9b_np { &test_case_1_9b_np_ptu, &test_case_1_9b_np_iut, NULL }

#define desc_case_1_10 "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\
"
static int
test_1_10_ptu(int child, int proving)
{
	if (test_alignment_pt(child, proving, __TEST_FISU, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_10_iut(int child, int proving)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (test_alignment_pt(child, proving, 0, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_10_p test_group_1
#define sgrp_case_1_10_p test_subgroup_1
#define sref_case_1_10_p "Q.781/1.10"
#define numb_case_1_10_p "1.10"
#define name_case_1_10_p "Normal alignment PO set and cleared"
#define xtra_case_1_10_p "Proving"
#define desc_case_1_10_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\n\
With proving\
"
static int
test_1_10_p_ptu(int child)
{
	return test_1_10_ptu(child, 1);
}
static int
test_1_10_p_iut(int child)
{
	return test_1_10_iut(child, 1);
}

static struct test_stream test_case_1_10_p_ptu = { preamble_link_power_on, test_1_10_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_10_p_iut = { preamble_link_power_on, test_1_10_p_iut, postamble_link_in_service };

#define test_case_1_10_p { &test_case_1_10_p_ptu, &test_case_1_10_p_iut, NULL }

#define tgrp_case_1_10_np test_group_1
#define sgrp_case_1_10_np test_subgroup_1
#define sref_case_1_10_np "Q.781/1.10"
#define numb_case_1_10_np "1.10np"
#define name_case_1_10_np "Normal alignment PO set and cleared"
#define xtra_case_1_10_np "No Proving"
#define desc_case_1_10_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\n\
Without proving\
"
static int
test_1_10_np_ptu(int child)
{
	return test_1_10_ptu(child, 0);
}
static int
test_1_10_np_iut(int child)
{
	return test_1_10_iut(child, 0);
}

static struct test_stream test_case_1_10_np_ptu = { preamble_link_power_on_np, test_1_10_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_10_np_iut = { preamble_link_power_on_np, test_1_10_np_iut, postamble_link_in_service };

#define test_case_1_10_np { &test_case_1_10_np_ptu, &test_case_1_10_np_iut, NULL }

#define desc_case_1_11 "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\
"
static int
test_1_11_ptu(int child, int proving)
{
	if (expect(child, INFINITE_WAIT, __TEST_SIOS))
		goto failure;
	state++;
	if (do_signal(child, __TEST_POWER_ON))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIOS))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	for (;;) {
		switch(get_event(child)) {
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case __TEST_SIO:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIO))
		goto failure;
	state++;
	start_tt(config->sl.t4n * 2);
	state++;
	switch (proving) {
	case 0:
		for (;;) {
			switch(get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				state++;
				continue;
			case __TEST_SIPO:
				break;
			default:
				goto failure;
			}
			break;
		}
		state++;
		break;
	case 1:
		for (;;) {
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				state++;
				print_less(child);
				continue;
			case __TEST_SIPO:
				state++;
				print_more(child);
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	case 2:
		for (;;) {
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIE))
					goto failure;
				state++;
				print_less(child);
				continue;
			case __TEST_SIPO:
				state++;
				print_more(child);
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	if (do_signal(child, __TEST_SIPO))
		goto failure;
	state++;
	start_tt(1000);
	state++;
	for (;;) {
		switch(get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			state++;
			continue;
		case __EVENT_TIMEOUT:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_11_iut(int child, int proving)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_11_p test_group_1
#define sgrp_case_1_11_p test_subgroup_1
#define sref_case_1_11_p "Q.781/1.11"
#define numb_case_1_11_p "1.11"
#define name_case_1_11_p "RPO set \"Aligned not ready\""
#define xtra_case_1_11_p "Proving"
#define desc_case_1_11_p "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\n\
With proving\
"
static int
test_1_11_p_ptu(int child)
{
	return test_1_11_ptu(child, 1);
}
static int
test_1_11_p_iut(int child)
{
	return test_1_11_iut(child, 1);
}

static struct test_stream test_case_1_11_p_ptu = { preamble_link_power_on, test_1_11_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_11_p_iut = { preamble_link_power_on, test_1_11_p_iut, postamble_link_in_service };

#define test_case_1_11_p { &test_case_1_11_p_ptu, &test_case_1_11_p_iut, NULL }

#define tgrp_case_1_11_np test_group_1
#define sgrp_case_1_11_np test_subgroup_1
#define sref_case_1_11_np "Q.781/1.11"
#define numb_case_1_11_np "1.11np"
#define name_case_1_11_np "RPO set \"Aligned not ready\""
#define xtra_case_1_11_np "No Proving"
#define desc_case_1_11_np "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\n\
Without proving\
"
static int
test_1_11_np_ptu(int child)
{
	return test_1_11_ptu(child, 0);
}
static int
test_1_11_np_iut(int child)
{
	return test_1_11_iut(child, 0);
}

static struct test_stream test_case_1_11_np_ptu = { preamble_link_power_on_np, test_1_11_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_11_np_iut = { preamble_link_power_on_np, test_1_11_np_iut, postamble_link_in_service };

#define test_case_1_11_np { &test_case_1_11_np_ptu, &test_case_1_11_np_iut, NULL }

#define desc_case_1_12a "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\
"
static int
test_1_12a_ptu(int child, int proving)
{
	switch (proving) {
	case 0:
		if (test_alignment_pt(child, proving, __TEST_SIO, __TEST_SIPO))
			goto failure;
		break;
	case 1:
		if (test_alignment_pt(child, proving, __TEST_SIN, __TEST_SIPO))
			goto failure;
		break;
	case 2:
		if (test_alignment_pt(child, proving, __TEST_SIE, __TEST_SIPO))
			goto failure;
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIOS))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_12a_iut(int child, int proving)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_12a_p test_group_1
#define sgrp_case_1_12a_p test_subgroup_1
#define sref_case_1_12a_p "Q.781/1.12"
#define numb_case_1_12a_p "1.12(a)"
#define name_case_1_12a_p "SIOS received when \"Aligned not ready\""
#define xtra_case_1_12a_p "Forward - Proving"
#define desc_case_1_12a_p "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\n\
With proving\
"
static int
test_1_12a_p_ptu(int child)
{
	return test_1_12a_ptu(child, 1);
}
static int
test_1_12a_p_iut(int child)
{
	return test_1_12a_iut(child, 1);
}

static struct test_stream test_case_1_12a_p_ptu = { preamble_link_power_on, test_1_12a_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_12a_p_iut = { preamble_link_power_on, test_1_12a_p_iut, postamble_link_out_of_service };

#define test_case_1_12a_p { &test_case_1_12a_p_ptu, &test_case_1_12a_p_iut, NULL }

#define tgrp_case_1_12a_np test_group_1
#define sgrp_case_1_12a_np test_subgroup_1
#define sref_case_1_12a_np "Q.781/1.12"
#define numb_case_1_12a_np "1.12(a)np"
#define name_case_1_12a_np "SIOS received when \"Aligned not ready\""
#define xtra_case_1_12a_np "Forward - No Proving"
#define desc_case_1_12a_np "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\n\
Without proving\
"
static int
test_1_12a_np_ptu(int child)
{
	return test_1_12a_ptu(child, 0);
}
static int
test_1_12a_np_iut(int child)
{
	return test_1_12a_iut(child, 0);
}

static struct test_stream test_case_1_12a_np_ptu = { preamble_link_power_on_np, test_1_12a_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_12a_np_iut = { preamble_link_power_on_np, test_1_12a_np_iut, postamble_link_out_of_service };

#define test_case_1_12a_np { &test_case_1_12a_np_ptu, &test_case_1_12a_np_iut, NULL }

#define desc_case_1_12b "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\
"
static int
test_1_12b_ptu(int child, int proving)
{
	if (test_alignment_sut(child, proving, __TEST_FISU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_SIOS))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_12b_iut(int child, int proving)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_12b_p test_group_1
#define sgrp_case_1_12b_p test_subgroup_1
#define sref_case_1_12b_p "Q.781/1.12"
#define numb_case_1_12b_p "1.12(b)"
#define name_case_1_12b_p "SIOS received when \"Aligned not ready\""
#define xtra_case_1_12b_p "Reverse - Proving"
#define desc_case_1_12b_p "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\n\
With proving\
"
static int
test_1_12b_p_ptu(int child)
{
	return test_1_12b_ptu(child, 1);
}
static int
test_1_12b_p_iut(int child)
{
	return test_1_12b_iut(child, 1);
}

static struct test_stream test_case_1_12b_p_ptu = { preamble_link_power_on, test_1_12b_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_12b_p_iut = { preamble_link_power_on, test_1_12b_p_iut, postamble_link_out_of_service };

#define test_case_1_12b_p { &test_case_1_12b_p_ptu, &test_case_1_12b_p_iut, NULL }

#define tgrp_case_1_12b_np test_group_1
#define sgrp_case_1_12b_np test_subgroup_1
#define sref_case_1_12b_np "Q.781/1.12"
#define numb_case_1_12b_np "1.12(b)np"
#define name_case_1_12b_np "SIOS received when \"Aligned not ready\""
#define xtra_case_1_12b_np "Reverse - No Proving"
#define desc_case_1_12b_np "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\n\
Without proving\
"
static int
test_1_12b_np_ptu(int child)
{
	return test_1_12b_ptu(child, 0);
}
static int
test_1_12b_np_iut(int child)
{
	return test_1_12b_iut(child, 0);
}

static struct test_stream test_case_1_12b_np_ptu = { preamble_link_power_on_np, test_1_12b_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_12b_np_iut = { preamble_link_power_on_np, test_1_12b_np_iut, postamble_link_out_of_service };

#define test_case_1_12b_np { &test_case_1_12b_np_ptu, &test_case_1_12b_np_iut, NULL }

#define desc_case_1_13 "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\
"
static int
test_1_13_ptu(int child, int proving)
{
	switch (proving) {
	case 0:
		if (test_alignment_pt(child, proving, __TEST_SIO, __TEST_SIPO))
			goto failure;
		break;
	case 1:
		if (test_alignment_pt(child, proving, __TEST_SIN, __TEST_SIPO))
			goto failure;
		break;
	case 2:
		if (test_alignment_pt(child, proving, __TEST_SIE, __TEST_SIPO))
			goto failure;
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	state++;
	if (do_signal(child, __TEST_SIO))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_13_iut(int child, int proving)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_13_p test_group_1
#define sgrp_case_1_13_p test_subgroup_1
#define sref_case_1_13_p "Q.781/1.13"
#define numb_case_1_13_p "1.13"
#define name_case_1_13_p "SIO received when \"Aligned not ready\""
#define xtra_case_1_13_p "Proving"
#define desc_case_1_13_p "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\n\
With proving\
"
static int
test_1_13_p_ptu(int child)
{
	return test_1_13_ptu(child, 1);
}
static int
test_1_13_p_iut(int child)
{
	return test_1_13_iut(child, 1);
}

static struct test_stream test_case_1_13_p_ptu = { preamble_link_power_on, test_1_13_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_13_p_iut = { preamble_link_power_on, test_1_13_p_iut, postamble_link_out_of_service };

#define test_case_1_13_p { &test_case_1_13_p_ptu, &test_case_1_13_p_iut, NULL }

#define tgrp_case_1_13_np test_group_1
#define sgrp_case_1_13_np test_subgroup_1
#define sref_case_1_13_np "Q.781/1.13"
#define numb_case_1_13_np "1.13np"
#define name_case_1_13_np "SIO received when \"Aligned not ready\""
#define xtra_case_1_13_np "No Proving"
#define desc_case_1_13_np "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\n\
Without proving\
"
static int
test_1_13_np_ptu(int child)
{
	return test_1_13_ptu(child, 0);
}
static int
test_1_13_np_iut(int child)
{
	return test_1_13_iut(child, 0);
}

static struct test_stream test_case_1_13_np_ptu = { preamble_link_power_on_np, test_1_13_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_13_np_iut = { preamble_link_power_on_np, test_1_13_np_iut, postamble_link_out_of_service };

#define test_case_1_13_np { &test_case_1_13_np_ptu, &test_case_1_13_np_iut, NULL }

#define tgrp_case_1_14 test_group_1
#define sgrp_case_1_14 test_subgroup_1
#define sref_case_1_14 "Q.781/1.14"
#define numb_case_1_14 "1.14"
#define name_case_1_14 "Set and clear LPO when \"Initial alignment\""
#define xtra_case_1_14 NULL
#define desc_case_1_14 "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Initial alignment\"\
"
static int
test_1_14_ptu(int child)
{
	if (test_alignment_pt(child, 1, __TEST_FISU, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_14_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	test_msleep(child, SHORT_WAIT);
	state++;
	/* set LPO after proving starts */
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	start_tt(config->sl.t4n / 2 - SHORT_WAIT);
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_TIMEOUT))
		goto failure;
	state++;
	/* set LPR in the middle of normal proving */
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_14_ptu = { preamble_link_power_on, test_1_14_ptu, postamble_link_in_service };
static struct test_stream test_case_1_14_iut = { preamble_link_power_on, test_1_14_iut, postamble_link_in_service };

#define test_case_1_14 { &test_case_1_14_ptu, &test_case_1_14_iut, NULL }

#define desc_case_1_15 "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\
"
static int
test_1_15_ptu(int child, int proving)
{
	if (test_alignment_pt(child, proving, __TEST_FISU, __TEST_FISU))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __TEST_SIPO:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __TEST_FISU:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	test_msleep(child, 5000);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_15_iut(int child, int proving)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	test_msleep(child, 1000);
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	test_msleep(child, 4000);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_15_p test_group_1
#define sgrp_case_1_15_p test_subgroup_1
#define sref_case_1_15_p "Q.781/1.15"
#define numb_case_1_15_p "1.15"
#define name_case_1_15_p "Set and clear LPO when \"Aligned ready\""
#define xtra_case_1_15_p "Proving"
#define desc_case_1_15_p "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\n\
With proving\
"
static int
test_1_15_p_ptu(int child)
{
	return test_1_15_ptu(child, 1);
}
static int
test_1_15_p_iut(int child)
{
	return test_1_15_iut(child, 1);
}

static struct test_stream test_case_1_15_p_ptu = { preamble_link_power_on, test_1_15_p_ptu, postamble_link_in_service };
static struct test_stream test_case_1_15_p_iut = { preamble_link_power_on, test_1_15_p_iut, postamble_link_in_service };

#define test_case_1_15_p { &test_case_1_15_p_ptu, &test_case_1_15_p_iut, NULL }

#define tgrp_case_1_15_np test_group_1
#define sgrp_case_1_15_np test_subgroup_1
#define sref_case_1_15_np "Q.781/1.15"
#define numb_case_1_15_np "1.15np"
#define name_case_1_15_np "Set and clear LPO when \"Aligned ready\""
#define xtra_case_1_15_np "No Proving"
#define desc_case_1_15_np "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\n\
Without proving\
"
static int
test_1_15_np_ptu(int child)
{
	return test_1_15_ptu(child, 0);
}
static int
test_1_15_np_iut(int child)
{
	return test_1_15_iut(child, 0);
}

static struct test_stream test_case_1_15_np_ptu = { preamble_link_power_on_np, test_1_15_np_ptu, postamble_link_in_service };
static struct test_stream test_case_1_15_np_iut = { preamble_link_power_on_np, test_1_15_np_iut, postamble_link_in_service };

#define test_case_1_15_np { &test_case_1_15_np_ptu, &test_case_1_15_np_iut, NULL }

#define desc_case_1_16 "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\
"
static int
test_1_16_ptu(int child, int proving)
{
	long beg_time = 0;

	if (test_alignment_lpo_sut(child, proving, __TEST_SIPO))
		goto failure;
	state++;
	beg_time = milliseconds(child, t1);
	start_tt(config->sl.t1 * 2);
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIPO:
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			continue;
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (check_time(child, "T1  ", beg_time, timer[t1].lo, timer[t1].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;

}
static int
test_1_16_iut(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_16_p test_group_1
#define sgrp_case_1_16_p test_subgroup_1
#define sref_case_1_16_p "Q.781/1.16"
#define numb_case_1_16_p "1.16"
#define name_case_1_16_p "Timer T1 in \"Aligned not ready\" state"
#define xtra_case_1_16_p "Proving"
#define desc_case_1_16_p "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\n\
With proving\
"
static int
test_1_16_p_ptu(int child)
{
	return test_1_16_ptu(child, 1);
}
static int
test_1_16_p_iut(int child)
{
	return test_1_16_iut(child, 1);
}

static struct test_stream test_case_1_16_p_ptu = { preamble_link_power_on, test_1_16_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_16_p_iut = { preamble_link_power_on, test_1_16_p_iut, postamble_link_out_of_service };

#define test_case_1_16_p { &test_case_1_16_p_ptu, &test_case_1_16_p_iut, NULL }

#define tgrp_case_1_16_np test_group_1
#define sgrp_case_1_16_np test_subgroup_1
#define sref_case_1_16_np "Q.781/1.16"
#define numb_case_1_16_np "1.16np"
#define name_case_1_16_np "Timer T1 in \"Aligned not ready\" state"
#define xtra_case_1_16_np "No Proving"
#define desc_case_1_16_np "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\n\
Without proving\
"
static int
test_1_16_np_ptu(int child)
{
	return test_1_16_ptu(child, 0);
}
static int
test_1_16_np_iut(int child)
{
	return test_1_16_iut(child, 0);
}

static struct test_stream test_case_1_16_np_ptu = { preamble_link_power_on_np, test_1_16_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_16_np_iut = { preamble_link_power_on_np, test_1_16_np_iut, postamble_link_out_of_service };

#define test_case_1_16_np { &test_case_1_16_np_ptu, &test_case_1_16_np_iut, NULL }

#define tgrp_case_1_17 test_group_1
#define sgrp_case_1_17 test_subgroup_1
#define sref_case_1_17 "Q.781/1.17"
#define numb_case_1_17 "1.17"
#define name_case_1_17 "No SIO sent during normal proving period"
#define xtra_case_1_17 NULL
#define desc_case_1_17 "\
Link State Control - Expected signal units/orders\n\
No SIO sent during normal proving period\
"
static int
test_1_17_ptu(int child)
{
	long beg_time = 0;

	if (test_out_of_service_sut(child))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case __TEST_SIO:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIN))
		goto failure;
	state++;
	beg_time = 0;
	for (;;) {
		switch (get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIN:
			state++;
			if (!beg_time)
				beg_time = dual_milliseconds(child, t3, t4n);
			if (do_signal(child, last_event))
				goto failure;
			state++;
			print_less(child);
			continue;
		case __TEST_FISU:
			state++;
			print_more(child);
			if (check_time(child, "T3,4", beg_time, timer[t4n].lo, timer[t3].hi + timer[t4n].hi))
				goto failure;
			state++;
			break;
		default:
			goto failure;
		}
		break;
	}
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_17_iut(int child)
{
	if (test_alignment_sut(child, 1, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_17_ptu = { preamble_link_power_on, test_1_17_ptu, postamble_link_in_service };
static struct test_stream test_case_1_17_iut = { preamble_link_power_on, test_1_17_iut, postamble_link_in_service };

#define test_case_1_17 { &test_case_1_17_ptu, &test_case_1_17_iut, NULL }

#define tgrp_case_1_18 test_group_1
#define sgrp_case_1_18 test_subgroup_1
#define sref_case_1_18 "Q.781/1.18"
#define numb_case_1_18 "1.18"
#define name_case_1_18 "Set and cease emergency prior to \"start alignment\""
#define xtra_case_1_18 NULL
#define desc_case_1_18 "\
Link State Control - Expected signal units/orders\n\
Set and cease emergency prior to \"start alignment\"\
"
static int
test_1_18_ptu(int child)
{
	long beg_time = 0;

	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIN))
		goto failure;
	state++;
	beg_time = milliseconds(child, t4n);
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __MSG_PROVING:
		case __TEST_SIN:
			if (do_signal(child, last_event))
				goto failure;
			state++;
			print_less(child);
			continue;
		case __TEST_FISU:
			state++;
			print_more(child);
			break;
		default:
			goto failure;
		}
		break;
	}
	if (check_time(child, "T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
		goto failure;
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_18_iut(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (test_alignment_sut(child, 1, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_18_ptu = { preamble_link_power_on, test_1_18_ptu, postamble_link_in_service };
static struct test_stream test_case_1_18_iut = { preamble_link_power_on, test_1_18_iut, postamble_link_in_service };

#define test_case_1_18 { &test_case_1_18_ptu, &test_case_1_18_iut, NULL }

#define tgrp_case_1_19 test_group_1
#define sgrp_case_1_19 test_subgroup_1
#define sref_case_1_19 "Q.781/1.19"
#define numb_case_1_19 "1.19"
#define name_case_1_19 "Set emergency while in \"not aligned state\""
#define xtra_case_1_19 NULL
#define desc_case_1_19 "\
Link State Control - Expected signal units/orders\n\
Set emergency while in \"not aligned state\"\
"
static int
test_1_19_ptu(int child)
{
	long beg_time = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_aligned_sut(child))
				goto failure;
			if (start_tt(LONG_WAIT))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_START))
					goto failure;
				if (do_signal(child, __TEST_SIO))
					goto failure;
				beg_time = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __MSG_PROVING:
			case __TEST_SIE:
				if (!beg_time)
					beg_time = milliseconds(child, t4e);
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
				if (!beg_time)
					beg_time = milliseconds(child, t4e);
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (check_time(child, "T4  ", beg_time, timer[t4e].lo, timer[t4e].hi))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_19_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_19_ptu = { preamble_link_power_on, test_1_19_ptu, postamble_link_in_service };
static struct test_stream test_case_1_19_iut = { preamble_link_power_on, test_1_19_iut, postamble_link_in_service };

#define test_case_1_19 { &test_case_1_19_ptu, &test_case_1_19_iut, NULL }

#define tgrp_case_1_20 test_group_1
#define sgrp_case_1_20 test_subgroup_1
#define sref_case_1_20 "Q.781/1.20"
#define numb_case_1_20 "1.20"
#define name_case_1_20 "Set emergency when \"aligned\""
#define xtra_case_1_20 NULL
#define desc_case_1_20 "\
Link State Control - Expected signal units/orders\n\
Set emergency when \"aligned\"\
"
static int
test_1_20_ptu(int child)
{
	long beg_time = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_proving_sut(child, 1))
				goto failure;
			if (do_signal(child, last_event))
				goto failure;
			beg_time = 0;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __MSG_PROVING:
				if (!beg_time)
					beg_time = milliseconds(child, t4e);
				if (do_signal(child, __MSG_PROVING))
					goto failure;
				print_less(child);
				continue;
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				print_less(child);
				continue;
			case __TEST_SIE:
				if (!beg_time)
					beg_time = milliseconds(child, t4e);
				if (do_signal(child, __TEST_SIN))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (!beg_time || check_time(child, "T4  ", beg_time, timer[t4e].lo, timer[t4e].hi))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_20_iut(int child)
{
	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_20_ptu = { preamble_link_power_on, test_1_20_ptu, postamble_link_in_service };
static struct test_stream test_case_1_20_iut = { preamble_link_power_on, test_1_20_iut, postamble_link_in_service };

#define test_case_1_20 { &test_case_1_20_ptu, &test_case_1_20_iut, NULL }

#define tgrp_case_1_21 test_group_1
#define sgrp_case_1_21 test_subgroup_1
#define sref_case_1_21 "Q.781/1.21"
#define numb_case_1_21 "1.21"
#define name_case_1_21 "Both ends set emergency."
#define xtra_case_1_21 NULL
#define desc_case_1_21 "\
Link State Control - Expected signal units/orders\n\
Both ends set emergency.\
"
static int
test_in_service(int child, int proving)
{
	switch (child) {
	case CHILD_PTU:
		if (test_alignment_sut(child, proving, __TEST_FISU))
			goto failure;
		state++;
		if (do_signal(child, __TEST_FISU))
			goto failure;
		state++;
		break;
	case CHILD_IUT:
		if (test_alignment_sut(child, proving, __EVENT_IUT_IN_SERVICE))
			goto failure;
		state++;
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_21_ptu(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (test_in_service(child, 2))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_21_iut(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (test_in_service(child, 2))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_21_ptu = { preamble_link_power_on, test_1_21_ptu, postamble_link_in_service };
static struct test_stream test_case_1_21_iut = { preamble_link_power_on, test_1_21_iut, postamble_link_in_service };

#define test_case_1_21 { &test_case_1_21_ptu, &test_case_1_21_iut, NULL }

#define tgrp_case_1_22 test_group_1
#define sgrp_case_1_22 test_subgroup_1
#define sref_case_1_22 "Q.781/1.22"
#define numb_case_1_22 "1.22"
#define name_case_1_22 "Individual end sets emergency"
#define xtra_case_1_22 NULL
#define desc_case_1_22 "\
Link State Control - Expected signal units/orders\n\
Individual end sets emergency\
"
static int
test_1_22_ptu(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (test_in_service(child, 2))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_22_iut(int child)
{
	if (test_in_service(child, 1))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_22_ptu = { preamble_link_power_on, test_1_22_ptu, postamble_link_in_service };
static struct test_stream test_case_1_22_iut = { preamble_link_power_on, test_1_22_iut, postamble_link_in_service };

#define test_case_1_22 { &test_case_1_22_ptu, &test_case_1_22_iut, NULL }

#define tgrp_case_1_23 test_group_1
#define sgrp_case_1_23 test_subgroup_1
#define sref_case_1_23 "Q.781/1.23"
#define numb_case_1_23 "1.23"
#define name_case_1_23 "Set emergency during normal proving"
#define xtra_case_1_23 NULL
#define desc_case_1_23 "\
Link State Control - Expected signal units/orders\n\
Set emergency during normal proving\
"
static int
test_1_23_ptu(int child)
{
	long beg_time = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_proving_sut(child, 1))
				goto failure;
			if (do_signal(child, __TEST_SIN))
				goto failure;
			beg_time = 0;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIE:
				if (!beg_time)
					beg_time = milliseconds(child, t4e);
			case __MSG_PROVING:
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (check_time(child, "T4  ", beg_time, timer[t4e].lo, timer[t4e].hi))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_23_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	/* send in the middle of the normal proving period */
	if (expect(child, config->sl.t4n / 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_23_ptu = { preamble_link_power_on, test_1_23_ptu, postamble_link_in_service };
static struct test_stream test_case_1_23_iut = { preamble_link_power_on, test_1_23_iut, postamble_link_in_service };

#define test_case_1_23 { &test_case_1_23_ptu, &test_case_1_23_iut, NULL }

#define tgrp_case_1_24 test_group_1
#define sgrp_case_1_24 test_subgroup_1
#define sref_case_1_24 "Q.781/1.24"
#define numb_case_1_24 "1.24"
#define name_case_1_24 "No SIO sent during emergency alignment"
#define xtra_case_1_24 NULL
#define desc_case_1_24 "\
Link State Control - Expected signal units/orders\n\
No SIO sent during emergency alignment\
"
static int
test_1_24_ptu(int child)
{
	long beg_time = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_out_of_service_sut(child))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_START))
					goto failure;
				if (do_signal(child, __TEST_SIE))
					goto failure;
				beg_time = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
				if (!beg_time)
					beg_time = dual_milliseconds(child, t3, t4e);
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (check_time(child, "T3,4", beg_time, timer[t4e].lo, timer[t3].hi + timer[t4e].hi))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_24_iut(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (test_alignment_sut(child, 1, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_24_ptu = { preamble_link_power_on, test_1_24_ptu, postamble_link_in_service };
static struct test_stream test_case_1_24_iut = { preamble_link_power_on, test_1_24_iut, postamble_link_in_service };

#define test_case_1_24 { &test_case_1_24_ptu, &test_case_1_24_iut, NULL }

#define tgrp_case_1_25 test_group_1
#define sgrp_case_1_25 test_subgroup_1
#define sref_case_1_25 "Q.781/1.25"
#define numb_case_1_25 "1.25"
#define name_case_1_25 "Deactivation duing intial alignment"
#define xtra_case_1_25 NULL
#define desc_case_1_25 "\
Link State Control - Expected signal units/orders\n\
Deactivation duing intial alignment\
"
static int
test_1_25_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_aligned_sut(child))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_25_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_25_ptu = { preamble_link_power_on, test_1_25_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_25_iut = { preamble_link_power_on, test_1_25_iut, postamble_link_out_of_service };

#define test_case_1_25 { &test_case_1_25_ptu, &test_case_1_25_iut, NULL }

#define tgrp_case_1_26 test_group_1
#define sgrp_case_1_26 test_subgroup_1
#define sref_case_1_26 "Q.781/1.26"
#define numb_case_1_26 "1.26"
#define name_case_1_26 "Deactivation during aligned state"
#define xtra_case_1_26 NULL
#define desc_case_1_26 "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned state\
"
static int
test_1_26_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_aligned_sut(child))
				goto failure;
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIN:
				print_less(child);
				continue;
			case __TEST_SIOS:
				print_more(child);
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_26_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_26_ptu = { preamble_link_power_on, test_1_26_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_26_iut = { preamble_link_power_on, test_1_26_iut, postamble_link_out_of_service };

#define test_case_1_26 { &test_case_1_26_ptu, &test_case_1_26_iut, NULL }

#define desc_case_1_27 "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\
"
static int
test_1_27_ptu(int child, int proving)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_lpo_sut(child, proving, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_27_iut(int child, int proving)
{
	if (test_alignment_lpo_sut(child, proving, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

#define tgrp_case_1_27_p test_group_1
#define sgrp_case_1_27_p test_subgroup_1
#define sref_case_1_27_p "Q.781/1.27"
#define numb_case_1_27_p "1.27"
#define name_case_1_27_p "Deactivation during aligned not ready"
#define xtra_case_1_27_p "Proving"
#define desc_case_1_27_p "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\n\
With proving\
"
static int
test_1_27_p_ptu(int child)
{
	return test_1_27_ptu(child, 1);
}
static int
test_1_27_p_iut(int child)
{
	return test_1_27_iut(child, 1);
}

static struct test_stream test_case_1_27_p_ptu = { preamble_link_power_on, test_1_27_p_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_27_p_iut = { preamble_link_power_on, test_1_27_p_iut, postamble_link_out_of_service };

#define test_case_1_27_p { &test_case_1_27_p_ptu, &test_case_1_27_p_iut, NULL }

#define tgrp_case_1_27_np test_group_1
#define sgrp_case_1_27_np test_subgroup_1
#define sref_case_1_27_np "Q.781/1.27"
#define numb_case_1_27_np "1.27np"
#define name_case_1_27_np "Deactivation during aligned not ready"
#define xtra_case_1_27_np "No Proving"
#define desc_case_1_27_np "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\n\
Without proving\
"
static int
test_1_27_np_ptu(int child)
{
	return test_1_27_ptu(child, 0);
}
static int
test_1_27_np_iut(int child)
{
	return test_1_27_iut(child, 0);
}

static struct test_stream test_case_1_27_np_ptu = { preamble_link_power_on_np, test_1_27_np_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_27_np_iut = { preamble_link_power_on_np, test_1_27_np_iut, postamble_link_out_of_service };

#define test_case_1_27_np { &test_case_1_27_np_ptu, &test_case_1_27_np_iut, NULL }

#define tgrp_case_1_28 test_group_1
#define sgrp_case_1_28 test_subgroup_1
#define sref_case_1_28 "Q.781/1.28"
#define numb_case_1_28 "1.28"
#define name_case_1_28 "SIO received during link in service"
#define xtra_case_1_28 NULL
#define desc_case_1_28 "\
Link State Control - Expected signal units/orders\n\
SIO received during link in service\
"
static int
test_1_28_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_28_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_28_ptu = { preamble_link_in_service, test_1_28_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_28_iut = { preamble_link_in_service, test_1_28_iut, postamble_link_out_of_service };

#define test_case_1_28 { &test_case_1_28_ptu, &test_case_1_28_iut, NULL }

#define tgrp_case_1_29a test_group_1
#define sgrp_case_1_29a test_subgroup_1
#define sref_case_1_29a "Q.781/1.29"
#define numb_case_1_29a "1.29(a)"
#define name_case_1_29a "SIOS received during link in service"
#define xtra_case_1_29a "Forward"
#define desc_case_1_29a "\
Link State Control - Expected signal units/orders\n\
SIOS received during link in service\n\
Forward direction\
"
static int
test_1_29a_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_STOP))
				goto failure;
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIOS:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_29a_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_29a_ptu = { preamble_link_in_service, test_1_29a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_29a_iut = { preamble_link_in_service, test_1_29a_iut, postamble_link_out_of_service };

#define test_case_1_29a { &test_case_1_29a_ptu, &test_case_1_29a_iut, NULL }

#define tgrp_case_1_29b test_group_1
#define sgrp_case_1_29b test_subgroup_1
#define sref_case_1_29b "Q.781/1.29"
#define numb_case_1_29b "1.29(b)"
#define name_case_1_29b "SIOS received during link in service"
#define xtra_case_1_29b "Reverse"
#define desc_case_1_29b "\
Link State Control - Expected signal units/orders\n\
SIOS received during link in service\n\
Reverse direction\
"
static int
test_1_29b_ptu(int child)
{
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			continue;
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_29b_iut(int child)
{
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_29b_ptu = { preamble_link_in_service, test_1_29b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_29b_iut = { preamble_link_in_service, test_1_29b_iut, postamble_link_out_of_service };

#define test_case_1_29b { &test_case_1_29b_ptu, &test_case_1_29b_iut, NULL }

#define tgrp_case_1_30a test_group_1
#define sgrp_case_1_30a test_subgroup_1
#define sref_case_1_30a "Q.781/1.30"
#define numb_case_1_30a "1.30(a)"
#define name_case_1_30a "Deactivation during LPO"
#define xtra_case_1_30a "Forward"
#define desc_case_1_30a "\
Link State Control - Expected signal units/orders\n\
Deactivation during LPO\n\
Forward direction\
"
static int
test_1_30a_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_30a_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_30a_ptu = { preamble_link_in_service, test_1_30a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_30a_iut = { preamble_link_in_service, test_1_30a_iut, postamble_link_out_of_service };

#define test_case_1_30a { &test_case_1_30a_ptu, &test_case_1_30a_iut, NULL }

#define tgrp_case_1_30b test_group_1
#define sgrp_case_1_30b test_subgroup_1
#define sref_case_1_30b "Q.781/1.30"
#define numb_case_1_30b "1.30(b)"
#define name_case_1_30b "Deactivation during LPO"
#define xtra_case_1_30b "Reverse"
#define desc_case_1_30b "\
Link State Control - Expected signal units/orders\n\
Deactivation during LPO\n\
Reverse direction\
"
static int
test_1_30b_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_LPO))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_STOP))
				goto failure;
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_30b_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_30b_ptu = { preamble_link_in_service, test_1_30b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_30b_iut = { preamble_link_in_service, test_1_30b_iut, postamble_link_out_of_service };

#define test_case_1_30b { &test_case_1_30b_ptu, &test_case_1_30b_iut, NULL }

#define tgrp_case_1_31a test_group_1
#define sgrp_case_1_31a test_subgroup_1
#define sref_case_1_31a "Q.781/1.31"
#define numb_case_1_31a "1.31(a)"
#define name_case_1_31a "Deactivation during RPO"
#define xtra_case_1_31a "Forward"
#define desc_case_1_31a "\
Link State Control - Expected signal units/orders\n\
Deactivation during RPO\n\
Forward direction\
"
static int
test_1_31a_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_LPO))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_31a_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_31a_ptu = { preamble_link_in_service, test_1_31a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_31a_iut = { preamble_link_in_service, test_1_31a_iut, postamble_link_out_of_service };

#define test_case_1_31a { &test_case_1_31a_ptu, &test_case_1_31a_iut, NULL }

#define tgrp_case_1_31b test_group_1
#define sgrp_case_1_31b test_subgroup_1
#define sref_case_1_31b "Q.781/1.31"
#define numb_case_1_31b "1.31(b)"
#define name_case_1_31b "Deactivation during RPO"
#define xtra_case_1_31b "Reverse"
#define desc_case_1_31b "\
Link State Control - Expected signal units/orders\n\
Deactivation during RPO\n\
Reverse direction\
"
static int
test_1_31b_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_STOP))
					goto failure;
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIOS:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_31b_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_31b_ptu = { preamble_link_in_service, test_1_31b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_31b_iut = { preamble_link_in_service, test_1_31b_iut, postamble_link_out_of_service };

#define test_case_1_31b { &test_case_1_31b_ptu, &test_case_1_31b_iut, NULL }

#define tgrp_case_1_32a test_group_1
#define sgrp_case_1_32a test_subgroup_1
#define sref_case_1_32a "Q.781/1.32"
#define numb_case_1_32a "1.32(a)"
#define name_case_1_32a "Deactivation during the proving period"
#define xtra_case_1_32a "Forward"
#define desc_case_1_32a "\
Link State Control - Expected signal units/orders\n\
Deactivation during the proving period\n\
Forward direction\
"
static int
test_1_32a_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_proving_sut(child, 1))
				goto failure;
			if (do_signal(child, last_event))
				goto failure;
			if (start_tt(config->sl.t4n / 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __EVENT_TIMEOUT:
				print_more(child);
				if (do_signal(child, __TEST_STOP))
					goto failure;
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				print_less(child);
				continue;
			case __TEST_SIOS:
				print_more(child);
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_1_32a_iut(int child)
{
	if (test_proving_sut(child, 1))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_32a_ptu = { preamble_link_power_on, test_1_32a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_32a_iut = { preamble_link_power_on, test_1_32a_iut, postamble_link_out_of_service };

#define test_case_1_32a { &test_case_1_32a_ptu, &test_case_1_32a_iut, NULL }

#define tgrp_case_1_32b test_group_1
#define sgrp_case_1_32b test_subgroup_1
#define sref_case_1_32b "Q.781/1.32"
#define numb_case_1_32b "1.32(b)"
#define name_case_1_32b "Deactivation during the proving period"
#define xtra_case_1_32b "Reverse"
#define desc_case_1_32b "\
Link State Control - Expected signal units/orders\n\
Deactivation during the proving period\n\
Reverse direction\
"
static int
test_1_32b_ptu(int child)
{
	if (test_alignment_pt(child, 1, __TEST_SIN, __TEST_SIOS))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_32b_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, config->sl.t4n / 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_32b_ptu = { preamble_link_power_on, test_1_32b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_32b_iut = { preamble_link_power_on, test_1_32b_iut, postamble_link_out_of_service };

#define test_case_1_32b { &test_case_1_32b_ptu, &test_case_1_32b_iut, NULL }

#define tgrp_case_1_33 test_group_1
#define sgrp_case_1_33 test_subgroup_1
#define sref_case_1_33 "Q.781/1.33"
#define numb_case_1_33 "1.33"
#define name_case_1_33 "SIO received instead of FISUs"
#define xtra_case_1_33 NULL
#define desc_case_1_33 "\
Link State Control - Expected signal units/orders\n\
SIO received instead of FISUs\
"
static int
test_1_33_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_sut(child, 1, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_33_iut(int child)
{
	if (test_alignment_sut(child, 1, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_33_ptu = { preamble_link_power_on, test_1_33_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_33_iut = { preamble_link_power_on, test_1_33_iut, postamble_link_out_of_service };

#define test_case_1_33 { &test_case_1_33_ptu, &test_case_1_33_iut, NULL }

#define tgrp_case_1_34 test_group_1
#define sgrp_case_1_34 test_subgroup_1
#define sref_case_1_34 "Q.781/1.34"
#define numb_case_1_34 "1.34"
#define name_case_1_34 "SIOS received instead of FISUs"
#define xtra_case_1_34 NULL
#define desc_case_1_34 "\
Link State Control - Expected signal units/orders\n\
SIOS received instead of FISUs\
"
static int
test_1_34_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_sut(child, 1, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIOS:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_34_iut(int child)
{
	if (test_alignment_sut(child, 1, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_34_ptu = { preamble_link_power_on, test_1_34_ptu, postamble_link_out_of_service };
static struct test_stream test_case_1_34_iut = { preamble_link_power_on, test_1_34_iut, postamble_link_out_of_service };

#define test_case_1_34 { &test_case_1_34_ptu, &test_case_1_34_iut, NULL }

#define tgrp_case_1_35 test_group_1
#define sgrp_case_1_35 test_subgroup_1
#define sref_case_1_35 "Q.781/1.35"
#define numb_case_1_35 "1.35"
#define name_case_1_35 "SIPO received instead of FISUs"
#define xtra_case_1_35 NULL
#define desc_case_1_35 "\
Link State Control - Expected signal units/orders\n\
SIPO received instead of FISUs\
"
static int
test_1_35_ptu(int child)
{
	if (test_alignment_sut(child, 1, __TEST_FISU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIPO))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_1_35_iut(int child)
{
	if (test_alignment_sut(child, 1, __EVENT_IUT_RPO))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_1_35_ptu = { preamble_link_power_on, test_1_35_ptu, postamble_link_in_service };
static struct test_stream test_case_1_35_iut = { preamble_link_power_on, test_1_35_iut, postamble_link_in_service };

#define test_case_1_35 { &test_case_1_35_ptu, &test_case_1_35_iut, NULL }

#define test_group_2 "2. Link State Control"
#define test_subgroup_2 "Unexpected signal units/orders"
#define tgrp_case_2_1 test_group_2
#define sgrp_case_2_1 test_subgroup_2
#define sref_case_2_1 "Q.781/2.1"
#define numb_case_2_1 "2.1"
#define name_case_2_1 "Unexpected signal units/orders in \"Out of service\" state"
#define xtra_case_2_1 NULL
#define desc_case_2_1 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Out of service\" state\
"
static int
test_2_1_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (expect(child, INFINITE_WAIT, __TEST_SIOS))
				goto failure;
			if (do_signal(child, __TEST_POWER_ON))
				goto failure;
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			if (do_signal(child, __TEST_SIO))
				goto failure;
			if (do_signal(child, __TEST_SIN))
				goto failure;
			if (do_signal(child, __TEST_SIE))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_SIB))
				goto failure;
			if (do_signal(child, __TEST_SIX))
				goto failure;
			if (do_signal(child, __TEST_SIX2))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_1_iut(int child)
{
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_1_ptu = { preamble_link_power_on, test_2_1_ptu, postamble_link_in_service };
static struct test_stream test_case_2_1_iut = { preamble_link_power_on, test_2_1_iut, postamble_link_in_service };

#define test_case_2_1 { &test_case_2_1_ptu, &test_case_2_1_iut, NULL }

#define tgrp_case_2_2 test_group_2
#define sgrp_case_2_2 test_subgroup_2
#define sref_case_2_2 "Q.781/2.2"
#define numb_case_2_2 "2.2"
#define name_case_2_2 "Unexpected signal units/orders in \"Not Aligned\" state"
#define xtra_case_2_2 NULL
#define desc_case_2_2 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Not Aligned\" state\
"
static int
test_2_2_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_aligned_sut(child))
				goto failure;
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_SIB))
				goto failure;
			if (do_signal(child, __TEST_SIX))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			if (do_signal(child, __TEST_SIO))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_2_2_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_2_ptu = { preamble_link_power_on, test_2_2_ptu, postamble_link_in_service };
static struct test_stream test_case_2_2_iut = { preamble_link_power_on, test_2_2_iut, postamble_link_in_service };

#define test_case_2_2 { &test_case_2_2_ptu, &test_case_2_2_iut, NULL }

#define tgrp_case_2_3 test_group_2
#define sgrp_case_2_3 test_subgroup_2
#define sref_case_2_3 "Q.781/2.3"
#define numb_case_2_3 "2.3"
#define name_case_2_3 "Unexpected signal units/orders in \"Aligned\" state"
#define xtra_case_2_3 NULL
#define desc_case_2_3 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned\" state\
"
static int
test_2_3_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_proving_sut(child, 1))
				goto failure;
			if (do_signal(child, __TEST_SIO))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_SIB))
				goto failure;
			if (do_signal(child, __TEST_SIX))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			test_msleep(child, LONG_WAIT);
			if (do_signal(child, __TEST_SIN))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}
static int
test_2_3_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_3_ptu = { preamble_link_power_on, test_2_3_ptu, postamble_link_in_service };
static struct test_stream test_case_2_3_iut = { preamble_link_power_on, test_2_3_iut, postamble_link_in_service };

#define test_case_2_3 { &test_case_2_3_ptu, &test_case_2_3_iut, NULL }

#define tgrp_case_2_4 test_group_2
#define sgrp_case_2_4 test_subgroup_2
#define sref_case_2_4 "Q.781/2.4"
#define numb_case_2_4 "2.4"
#define name_case_2_4 "Unexpected signal units/orders in \"Proving\" state"
#define xtra_case_2_4 NULL
#define desc_case_2_4 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Proving\" state\
"
static int
test_2_4_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_proving_sut(child, 1))
				goto failure;
			if (do_signal(child, __TEST_SIN))
				goto failure;
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_SIB))
				goto failure;
			if (do_signal(child, __TEST_SIX))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __MSG_PROVING:
			case __TEST_SIE:
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				print_less(child);
				continue;
			case __TEST_FISU:
				print_more(child);
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_4_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	/* wait for half of proving period */
	if (expect(child, config->sl.t4n / 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_4_ptu = { preamble_link_power_on, test_2_4_ptu, postamble_link_in_service };
static struct test_stream test_case_2_4_iut = { preamble_link_power_on, test_2_4_iut, postamble_link_in_service };

#define test_case_2_4 { &test_case_2_4_ptu, &test_case_2_4_iut, NULL }

#define tgrp_case_2_5 test_group_2
#define sgrp_case_2_5 test_subgroup_2
#define sref_case_2_5 "Q.781/2.5"
#define numb_case_2_5 "2.5"
#define name_case_2_5 "Unexpected signal units/orders in \"Aligned Ready\" state"
#define xtra_case_2_5 NULL
#define desc_case_2_5 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned Ready\" state\
"
static int
test_2_5_ptu(int child)
{
	if (test_alignment_sut(child, 1, __TEST_FISU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIB))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIX))
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_5_iut(int child)
{
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_5_ptu = { preamble_link_power_on, test_2_5_ptu, postamble_link_in_service };
static struct test_stream test_case_2_5_iut = { preamble_link_power_on, test_2_5_iut, postamble_link_in_service };

#define test_case_2_5 { &test_case_2_5_ptu, &test_case_2_5_iut, NULL }

#define tgrp_case_2_6 test_group_2
#define sgrp_case_2_6 test_subgroup_2
#define sref_case_2_6 "Q.781/2.6"
#define numb_case_2_6 "2.6"
#define name_case_2_6 "Unexpected signal units/orders in \"Aligned Not Ready\" state"
#define xtra_case_2_6 NULL
#define desc_case_2_6 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned Not Ready\" state\
"
static int
test_2_6_ptu(int child)
{
	if (test_alignment_lpo_sut(child, 1, __TEST_SIPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIB))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIX))
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_6_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
#if 0
	/* spec says __TEST_LPR, but it doesn't make sense... */
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
#endif
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_6_ptu = { preamble_link_power_on, test_2_6_ptu, postamble_link_in_service };
static struct test_stream test_case_2_6_iut = { preamble_link_power_on, test_2_6_iut, postamble_link_in_service };

#define test_case_2_6 { &test_case_2_6_ptu, &test_case_2_6_iut, NULL }

#define tgrp_case_2_7 test_group_2
#define sgrp_case_2_7 test_subgroup_2
#define sref_case_2_7 "Q.781/2.7"
#define numb_case_2_7 "2.7"
#define name_case_2_7 "Unexpected signal units/orders in \"In Service\" state"
#define xtra_case_2_7 NULL
#define desc_case_2_7 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"In Service\" state\
"
static int
test_2_7_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(LONG_WAIT))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_SIX))
					goto failure;
				if (start_tt(LONG_WAIT))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_7_iut(int child)
{
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_7_ptu = { preamble_link_in_service, test_2_7_ptu, postamble_link_in_service };
static struct test_stream test_case_2_7_iut = { preamble_link_in_service, test_2_7_iut, postamble_link_in_service };

#define test_case_2_7 { &test_case_2_7_ptu, &test_case_2_7_iut, NULL }

#define tgrp_case_2_8 test_group_2
#define sgrp_case_2_8 test_subgroup_2
#define sref_case_2_8 "Q.781/2.8"
#define numb_case_2_8 "2.8"
#define name_case_2_8 "Unexpected signal units/orders in \"Processor Outage\" state"
#define xtra_case_2_8 NULL
#define desc_case_2_8 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Processor Outage\" state\
"
static int
test_2_8_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_SIB))
					goto failure;
				if (do_signal(child, __TEST_SIX))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_2_8_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_EMERG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CEASE))
		goto failure;
	state++;
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_2_8_ptu = { preamble_link_in_service, test_2_8_ptu, postamble_link_in_service };
static struct test_stream test_case_2_8_iut = { preamble_link_in_service, test_2_8_iut, postamble_link_in_service };

#define test_case_2_8 { &test_case_2_8_ptu, &test_case_2_8_iut, NULL }

#define test_group_3 "3. Transmission Failure"
#define test_subgroup_3_1 "Break Tx path"
#define tgrp_case_3_1 test_group_3
#define sgrp_case_3_1 test_subgroup_3_1
#define sref_case_3_1 "Q.781/3.1"
#define numb_case_3_1 "3.1"
#define name_case_3_1 "Link aligned ready"
#define xtra_case_3_1 "Break Tx path"
#define desc_case_3_1 "\
Transmission Failure\n\
Link aligned ready (Break Tx path)\
"
static int
test_3_1_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_sut(child, 1, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_TX_BREAK))
				goto inconclusive;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_TX_MAKE))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TX_MAKE);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
}
static int
test_3_1_iut(int child)
{
	if (test_alignment_sut(child, 1, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_1_ptu = { preamble_link_power_on, test_3_1_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_1_iut = { preamble_link_power_on, test_3_1_iut, postamble_link_out_of_service };

#define test_case_3_1 { &test_case_3_1_ptu, &test_case_3_1_iut, NULL }

#define test_subgroup_3_2 "Corrupt FIBs - Basic"
#define tgrp_case_3_2 test_group_3
#define sgrp_case_3_2 test_subgroup_3_2
#define sref_case_3_2 "Q.781/3.2"
#define numb_case_3_2 "3.2"
#define name_case_3_2 "Link aligned ready"
#define xtra_case_3_2 "Corrupt FIBs - Basic"
#define desc_case_3_2 "\
Transmission Failure\n\
Link aligned ready (Corrupt FIBs - Basic)\
"
static int
test_3_2_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU_BAD_FIB))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, SHORT_WAIT)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU_BAD_FIB))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_3_2_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_2_ptu = { preamble_link_in_service, test_3_2_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_2_iut = { preamble_link_in_service, test_3_2_iut, postamble_link_out_of_service };

#define test_case_3_2 { &test_case_3_2_ptu, &test_case_3_2_iut, NULL }

#define tgrp_case_3_3 test_group_3
#define sgrp_case_3_3 test_subgroup_3_1
#define sref_case_3_3 "Q.781/3.3"
#define numb_case_3_3 "3.3"
#define name_case_3_3 "Link aligned not ready"
#define xtra_case_3_3 "Break Tx path"
#define desc_case_3_3 "\
Transmission Failure\n\
Link aligned not ready (Break Tx path)\
"
static int
test_3_3_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_lpo_sut(child, 1, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_TX_BREAK))
				goto inconclusive;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_TX_MAKE))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TX_MAKE);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
}
static int
test_3_3_iut(int child)
{
	if (test_alignment_lpo_sut(child, 1, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_3_ptu = { preamble_link_power_on, test_3_3_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_3_iut = { preamble_link_power_on, test_3_3_iut, postamble_link_out_of_service };

#define test_case_3_3 { &test_case_3_3_ptu, &test_case_3_3_iut, NULL }

#define tgrp_case_3_4 test_group_3
#define sgrp_case_3_4 test_subgroup_3_2
#define sref_case_3_4 "Q.781/3.4"
#define numb_case_3_4 "3.4"
#define name_case_3_4 "Link aligned not ready"
#define xtra_case_3_4 "Corrupt FIBs - Basic"
#define desc_case_3_4 "\
Transmission Failure\n\
Link aligned not ready (Corrupt FIBs - Basic)\
"
static int
test_3_4_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (test_alignment_lpo_sut(child, 1, __TEST_SIPO))
				goto failure;
			if (do_signal(child, __TEST_BAD_ACK))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, SHORT_WAIT)) {
			case __TEST_SIPO:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_BAD_ACK))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_3_4_iut(int child)
{
	if (test_alignment_lpo_sut(child, 1, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_4_ptu = { preamble_link_power_on, test_3_4_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_4_iut = { preamble_link_power_on, test_3_4_iut, postamble_link_out_of_service };

#define test_case_3_4 { &test_case_3_4_ptu, &test_case_3_4_iut, NULL }

#define tgrp_case_3_5 test_group_3
#define sgrp_case_3_5 test_subgroup_3_1
#define sref_case_3_5 "Q.781/3.5"
#define numb_case_3_5 "3.5"
#define name_case_3_5 "Link in service"
#define xtra_case_3_5 "Break Tx path"
#define desc_case_3_5 "\
Transmission Failure\n\
Link in service (Break Tx path)\
"
static int
test_3_5_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_TX_BREAK))
				goto inconclusive;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_TX_MAKE))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TX_MAKE);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
}
static int
test_3_5_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_5_ptu = { preamble_link_in_service, test_3_5_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_5_iut = { preamble_link_in_service, test_3_5_iut, postamble_link_out_of_service };

#define test_case_3_5 { &test_case_3_5_ptu, &test_case_3_5_iut, NULL }

#define tgrp_case_3_6 test_group_3
#define sgrp_case_3_6 test_subgroup_3_2
#define sref_case_3_6 "Q.781/3.6"
#define numb_case_3_6 "3.6"
#define name_case_3_6 "Link in service"
#define xtra_case_3_6 "Corrupt FIBs - Basic"
#define desc_case_3_6 "\
Transmission Failure\n\
Link in service (Corrupt FIBs - Basic)\
"
static int
test_3_6_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU_BAD_FIB))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, SHORT_WAIT)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU_BAD_FIB))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_3_6_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_6_ptu = { preamble_link_in_service, test_3_6_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_6_iut = { preamble_link_in_service, test_3_6_iut, postamble_link_out_of_service };

#define test_case_3_6 { &test_case_3_6_ptu, &test_case_3_6_iut, NULL }

#define tgrp_case_3_7 test_group_3
#define sgrp_case_3_7 test_subgroup_3_1
#define sref_case_3_7 "Q.781/3.7"
#define numb_case_3_7 "3.7"
#define name_case_3_7 "Link in processor outage"
#define xtra_case_3_7 "Break Tx path"
#define desc_case_3_7 "\
Transmission Failure\n\
Link in processor outage (Break Tx path)\
"
static int
test_3_7_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_TX_BREAK))
					goto inconclusive;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_TX_MAKE))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TX_MAKE);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
}
static int
test_3_7_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_7_ptu = { preamble_link_in_service, test_3_7_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_7_iut = { preamble_link_in_service, test_3_7_iut, postamble_link_out_of_service };

#define test_case_3_7 { &test_case_3_7_ptu, &test_case_3_7_iut, NULL }

#define tgrp_case_3_8 test_group_3
#define sgrp_case_3_8 test_subgroup_3_2
#define sref_case_3_8 "Q.781/3.8"
#define numb_case_3_8 "3.8"
#define name_case_3_8 "Link in processor outage"
#define xtra_case_3_8 "Corrupt FIBs - Basic"
#define desc_case_3_8 "\
Transmission Failure\n\
Link in processor outage (Corrupt FIBs - Basic)\
"
static int
test_3_8_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU_BAD_FIB))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (wait_event(child, SHORT_WAIT)) {
			case __TEST_SIPO:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU_BAD_FIB))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_3_8_iut(int child)
{
#if 0
	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
			goto failure;
		state++;
		return __RESULT_NOTAPPL;
	}
#endif
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_3_8_ptu = { preamble_link_in_service, test_3_8_ptu, postamble_link_out_of_service };
static struct test_stream test_case_3_8_iut = { preamble_link_in_service, test_3_8_iut, postamble_link_out_of_service };

#define test_case_3_8 { &test_case_3_8_ptu, &test_case_3_8_iut, NULL }

#define test_group_4 "4. Processor Outage Control"
#define test_subgroup_4_1 "Local Processor Outage"
#define tgrp_case_4_1a test_group_4
#define sgrp_case_4_1a test_subgroup_4_1
#define sref_case_4_1a "Q.781/4.1"
#define numb_case_4_1a "4.1(a)"
#define name_case_4_1a "Set and clear LPO while link in service"
#define xtra_case_4_1a "Forward"
#define desc_case_4_1a "\
Processor Outage Control\n\
Set and clear LPO while link in service\n\
Forward direction\
"
static int
test_4_1a_ptu(int child)
{
	int dat = 0, ack = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			/* (1) The test begins with the link in the "In Service" state. */
			/* (2) Send one data message from SP B to SP A. */
			if (do_signal(child, __TEST_MSU))
				goto failure;
			/* (3) Send two MSUs from SP A, issue a level 3 "Set Local Processor
			   Outage" command at SP A, and send another MSU from SP A. */
			test_msleep(child, LONG_WAIT);
			state++;
			continue;
		case 1:
			/* (4) Send another data message from SP B to SP A and acknowledge the
			   first data message sent from SP A. */
			/* (5) Check that SP A sends two Data messages and acknowledges one data
			   message before sending "Processor Outage" message. */
			switch (get_event(child)) {
			case __TEST_MSU:
				if (dat == 0) {
					dat++;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					/* first data message, acknowledge it */
					bsn[0] = fsn[1];
					if (do_signal(child, __TEST_FISU))
						goto failure;
					continue;
				} else if (dat == 1) {
					/* second data message, don't ack it */
					dat++;
					if (ack == 0)
						continue;
					state++;
					continue;
				}
				goto failure;
			case __TEST_FISU:
				if (bsn[1] != fsn[0])
					continue;
				if (ack == 0) {
					ack++;
					if (dat != 2)
						continue;
					state++;
					continue;
				}
				goto failure;
			default:
				goto failure;
			}
			break;
		case 2:
			/* (6) Check that the second data message sent after "Set Local Processor
			   Outage" was asserted is not acknowledged or indicated. */
			/* (7) Upon receiving a status "Procesor Outage" message from SP A, send
			   another data message to SP A from SP B. */
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_MSU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			/* (8) Check that this last message is neither acknowledged by nor
			   indicated at SP A. */
			/* (9) Issue a Level 3 "Clear Buffers" and Level 3 "Clear Local Processor
			   Outage" commands at SP A and send another MSU from SP A.  */
			/* (10) Check that SP A sends a status "Processor Outage Ended" message. */
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_FISU:
				/* (11) Send another data message to SP A and send a status "Ready" 
				   message from SP B to SP A with the appropriate sequence numbers. 
				 */
				if (do_signal(child, __TEST_MSU))
					goto failure;
				fsn[0] = bsn[1];
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			/* (12) Check that the message sent before status "Ready" is neither
			   acknowledged by nor indicated at SP A. */
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				/* (13) Send another data message to SP A. */
				if (do_signal(child, __TEST_MSU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 5:
			/* (14) Check that SP A and SP B exchange this last set of data messages
			   and akcnowledgements */
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != fsn[0]) {
					state++;
					continue;
				}
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_4_1a_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	expect(child, LONGEST_WAIT, __EVENT_NO_MSG);
	state++;
	if (do_signal(child, __TEST_CLEARB))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_4_1a_ptu = { preamble_link_in_service, test_4_1a_ptu, postamble_link_in_service };
static struct test_stream test_case_4_1a_iut = { preamble_link_in_service, test_4_1a_iut, postamble_link_in_service };

#define test_case_4_1a { &test_case_4_1a_ptu, &test_case_4_1a_iut, NULL }

#define tgrp_case_4_1b test_group_4
#define sgrp_case_4_1b test_subgroup_4_1
#define sref_case_4_1b "Q.781/4.1"
#define numb_case_4_1b "4.1(b)"
#define name_case_4_1b "Set and clear LPO while link in service"
#define xtra_case_4_1b "Reverse"
#define desc_case_4_1b "\
Processor Outage Control\n\
Set and clear LPO while link in service\n\
Reverse direction\
"
static int
test_4_1b_ptu(int child)
{
	int dat = 0, ack = 0;

	/* ( 1) The test begins with the link in the "In Service" state. */
	/* ( 2) Send two data messages from SP A to SP B and one data message from SP B to SP A. */
	if (do_signal(child, __TEST_MSU))
		goto failure;
	state++;
	/* ( 3) Check that SP A acknowledges the data message sent from SP B to SP A. */
	/* ( 4) Send a "Data Ack" message from SP B to SP A acknowledging the first data messsage
	   and send a status "Processor Outage" message. */
	for (;;) {
		switch (get_event(child)) {
		case __TEST_MSU:
			if (dat == 0) {
				dat++;
				state++;
				bsn[0] = fsn[1];
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			} else if (dat == 1) {
				dat++;
				state++;
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				state++;
				if (ack == 0)
					continue;
				break;
			}
			goto failure;
		case __TEST_FISU:
			state++;
			if (bsn[1] != fsn[0])
				continue;
			if (ack == 0) {
				ack++;
				if (dat != 2)
					continue;
				break;
			}
			goto failure;
		default:
			goto failure;
		}
		break;
	}
	/* ( 5) Send another data message from SP B to SP A and send another MSU from SP A to SP B
	   (during the processor outage period). */
	if (do_signal(child, __TEST_MSU))
		goto failure;
	state++;
	/* ( 6) Check that SP A acknowledges the data message sent to it during the processor
	   outage period. */
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			state++;
			if (bsn[1] != fsn[0])
				continue;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	/* ( 7) Check that SP A does not issue a data message from the MSU requested at SP A after
	   receipt of status "Processor Outage". */
	if (start_tt(config->sl.t7 * 2))
		goto failure;
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_SIPO))
				goto failure;
			state++;
			continue;
		case __EVENT_TIMEOUT:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	/* ( 8) Check that SP A indicates both MSUs and "Remote Processor Outage" to Level 3. */
	/* ( 9) Wait for T7 to ensure that SP A does not require acknowledgement to the data
	   message sent before processor outage was invoked. */
	/* (10) Send a status "Processor Recovered" message to SP A. */
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	/* (11) Check that SP A responds with a status "Ready" message and indicates "Remote
	   Processor Recovered" message to SP A. */
	switch (wait_event(child, 0)) {
	case __TEST_FISU:
	case __EVENT_NO_MSG:
		if (do_signal(child, __TEST_FISU))
			goto failure;
		break;
	default:
		goto failure;
	}
	state++;
	/* (12) Check that SP A sends a data message for the MSU that was requested during
	   processor outage. */
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __TEST_MSU:
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	/* (13) Send a "Data Ack" message to SP A to acknowledge the data message. */
	bsn[0] = fsn[1];
	if (do_signal(child, __TEST_FISU))
		goto failure;
	state++;
	/* (14) Send a data mesasge to SP A. */
	if (do_signal(child, __TEST_MSU))
		goto failure;
	state++;
	/* (15) Check that SP A acknowledges the data message with a "Data Ack" message with the
	   appropriate sequence numbers. */
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			state++;
			if (bsn[1] != fsn[0])
				continue;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_4_1b_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPR))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_4_1b_ptu = { preamble_link_in_service, test_4_1b_ptu, postamble_link_in_service };
static struct test_stream test_case_4_1b_iut = { preamble_link_in_service, test_4_1b_iut, postamble_link_in_service };

#define test_case_4_1b { &test_case_4_1b_ptu, &test_case_4_1b_iut, NULL }

#define test_subgroup_4_2 "Remote Processor Outage"
#define tgrp_case_4_2 test_group_4
#define sgrp_case_4_2 test_subgroup_4_2
#define sref_case_4_2 "Q.781/4.2"
#define numb_case_4_2 "4.2"
#define name_case_4_2 "RPO during LPO"
#define xtra_case_4_2 NULL
#define desc_case_4_2 "\
Processor Outage Control\n\
RPO during LPO\
"
static int
test_4_2_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_FISU:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_4_2_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPR))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_4_2_ptu = { preamble_link_in_service, test_4_2_ptu, postamble_link_in_service };
static struct test_stream test_case_4_2_iut = { preamble_link_in_service, test_4_2_iut, postamble_link_in_service };

#define test_case_4_2 { &test_case_4_2_ptu, &test_case_4_2_iut, NULL }

#define tgrp_case_4_3 test_group_4
#define sgrp_case_4_3 test_subgroup_4_1
#define sref_case_4_3 "Q.781/4.3"
#define numb_case_4_3 "4.3"
#define name_case_4_3 "Clear LPO when \"Both processor outage\""
#define xtra_case_4_3 NULL
#define desc_case_4_3 "\
Processor Outage Control\n\
Clear LPO when \"Both processor outage\"\
"
static int
test_4_3_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIPO:
				if (do_signal(child, __TEST_LPO))
					goto failure;
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIPO:
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				continue;
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (do_signal(child, __TEST_LPR))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_4_3_iut(int child)
{
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPR))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_4_3_ptu = { preamble_link_in_service, test_4_3_ptu, postamble_link_in_service };
static struct test_stream test_case_4_3_iut = { preamble_link_in_service, test_4_3_iut, postamble_link_in_service };

#define test_case_4_3 { &test_case_4_3_ptu, &test_case_4_3_iut, NULL }

#define test_group_5 "5. SU delimitation, alignment, error detection and correction"
#define test_subgroup_5 "Various framing errors"
#define tgrp_case_5_1 test_group_5
#define sgrp_case_5_1 test_subgroup_5
#define sref_case_5_1 "Q.781/5.1"
#define numb_case_5_1 "5.1"
#define name_case_5_1 "More than 7 ones between MSU opening and closing flags"
#define xtra_case_5_1 NULL
#define desc_case_5_1 "\
SU delimitation, alignment, error detection and correction\n\
More than 7 ones between MSU opening and closing flags\
"
static int
test_5_1_ptu(int child)
{
#if 1
	unsigned char old_bsn = bsn[1];
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_MSU_SEVEN_ONES))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != old_bsn)
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_1_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7 * 3, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_1_ptu = { preamble_link_in_service, test_5_1_ptu, postamble_link_in_service };
static struct test_stream test_case_5_1_iut = { preamble_link_in_service, test_5_1_iut, postamble_link_in_service };

#define test_case_5_1 { &test_case_5_1_ptu, &test_case_5_1_iut, NULL }

#define tgrp_case_5_2 test_group_5
#define sgrp_case_5_2 test_subgroup_5
#define sref_case_5_2 "Q.781/5.2"
#define numb_case_5_2 "5.2"
#define name_case_5_2 "Greater than maximum signal unit length"
#define xtra_case_5_2 NULL
#define desc_case_5_2 "\
SU delimitation, alignment, error detection and correction\n\
Greater than maximum signal unit length\
"
static int
test_5_2_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_MSU_TOO_LONG))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != 0x7f) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				if (check_snibs(child, 0x7f, 0xff))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_2_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7 * 3, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_2_ptu = { preamble_link_in_service, test_5_2_ptu, postamble_link_in_service };
static struct test_stream test_case_5_2_iut = { preamble_link_in_service, test_5_2_iut, postamble_link_in_service };

#define test_case_5_2 { &test_case_5_2_ptu, &test_case_5_2_iut, NULL }

#define tgrp_case_5_3 test_group_5
#define sgrp_case_5_3 test_subgroup_5
#define sref_case_5_3 "Q.781/5.3"
#define numb_case_5_3 "5.3"
#define name_case_5_3 "Below minimum signal unit length"
#define xtra_case_5_3 NULL
#define desc_case_5_3 "\
SU delimitation, alignment, error detection and correction\n\
Below minimum signal unit length\
"
static int
test_5_3_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_MSU_TOO_SHORT))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != 0x7f) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
#else
	if (do_signal(child, __TEST_MSU_TOO_SHORT))
		goto failure;
#endif
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_5_3_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7 * 3, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_SUCCESS;
#endif
}

static struct test_stream test_case_5_3_ptu = { preamble_link_in_service, test_5_3_ptu, postamble_link_in_service };
static struct test_stream test_case_5_3_iut = { preamble_link_in_service, test_5_3_iut, postamble_link_in_service };

#define test_case_5_3 { &test_case_5_3_ptu, &test_case_5_3_iut, NULL }

#define tgrp_case_5_4a test_group_5
#define sgrp_case_5_4a test_subgroup_5
#define sref_case_5_4a "Q.781/5.4"
#define numb_case_5_4a "5.4(a)"
#define name_case_5_4a "Reception of single and multiple flags between FISUs"
#define xtra_case_5_4a "Forward"
#define desc_case_5_4a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Forward direction\
"
static int
test_5_4a_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU_FISU_1FLAG))
				goto failure;
			if (start_tt(config->sl.t7))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_4a_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7 * 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_4a_ptu = { preamble_link_in_service, test_5_4a_ptu, postamble_link_in_service };
static struct test_stream test_case_5_4a_iut = { preamble_link_in_service, test_5_4a_iut, postamble_link_in_service };

#define test_case_5_4a { &test_case_5_4a_ptu, &test_case_5_4a_iut, NULL }

#define tgrp_case_5_4b test_group_5
#define sgrp_case_5_4b test_subgroup_5
#define sref_case_5_4b "Q.781/5.4"
#define numb_case_5_4b "5.4(b)"
#define name_case_5_4b "Reception of single and multiple flags between FISUs"
#define xtra_case_5_4b "Reverse"
#define desc_case_5_4b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Reverse direction\
"
static int
test_5_4b_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU_FISU_2FLAG))
				goto failure;
			if (start_tt(config->sl.t7))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_4b_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7 * 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_4b_ptu = { preamble_link_in_service, test_5_4b_ptu, postamble_link_in_service };
static struct test_stream test_case_5_4b_iut = { preamble_link_in_service, test_5_4b_iut, postamble_link_in_service };

#define test_case_5_4b { &test_case_5_4b_ptu, &test_case_5_4b_iut, NULL }

#define tgrp_case_5_5a test_group_5
#define sgrp_case_5_5a test_subgroup_5
#define sref_case_5_5a "Q.781/5.5"
#define numb_case_5_5a "5.5(a)"
#define name_case_5_5a "Reception of single and multiple flags between MSUs"
#define xtra_case_5_5a "Forward"
#define desc_case_5_5a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Forward direction\
"
static int
test_5_5a_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_MSU_MSU_1FLAG))
				goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				stop_tt();
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_5a_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, config->sl.t7, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_5a_ptu = { preamble_link_in_service, test_5_5a_ptu, postamble_link_in_service };
static struct test_stream test_case_5_5a_iut = { preamble_link_in_service, test_5_5a_iut, postamble_link_in_service };

#define test_case_5_5a { &test_case_5_5a_ptu, &test_case_5_5a_iut, NULL }

#define tgrp_case_5_5b test_group_5
#define sgrp_case_5_5b test_subgroup_5
#define sref_case_5_5b "Q.781/5.5"
#define numb_case_5_5b "5.5(b)"
#define name_case_5_5b "Reception of single and multiple flags between MSUs"
#define xtra_case_5_5b "Reverse"
#define desc_case_5_5b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Reverse direction\
"
static int
test_5_5b_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_MSU_MSU_2FLAG))
				goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				stop_tt();
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_5_5b_iut(int child)
{
#if 1
	if (expect(child, config->sl.t7, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, config->sl.t7, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_5_5b_ptu = { preamble_link_in_service, test_5_5b_ptu, postamble_link_in_service };
static struct test_stream test_case_5_5b_iut = { preamble_link_in_service, test_5_5b_iut, postamble_link_in_service };

#define test_case_5_5b { &test_case_5_5b_ptu, &test_case_5_5b_iut, NULL }

#define test_group_6 "6. SUERM check"
#define test_subgroup_6 "Various error conditions"
#define tgrp_case_6_1 test_group_6
#define sgrp_case_6_1 test_subgroup_6
#define sref_case_6_1 "Q.781/6.1"
#define numb_case_6_1 "6.1"
#define name_case_6_1 "Error rate of 1 in 256 - Link remains in service"
#define xtra_case_6_1 NULL
#define desc_case_6_1 "\
SUERM check\n\
Error rate of 1 in 256 - Link remains in service\
"
static int
test_6_1_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU_CORRUPT))
				goto failure;
			if (stop_tt())
				goto failure;
			count = 255;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (tries < 8192) {
					int i;

					if (tries) {
						if (do_signal(child, __TEST_FISU_CORRUPT_S))
							goto failure;
						for (i = 0; i < count; i++)
							if (do_signal(child, __TEST_FISU_S))
								goto failure;
					} else {
						if (do_signal(child, __TEST_FISU_CORRUPT))
							goto failure;
						if (do_signal(child, __TEST_FISU))
							goto failure;
						for (i = 0; i < count; i++)
							if (do_signal(child, __TEST_FISU_S))
								goto failure;
						if (do_signal(child, __TEST_COUNT))
							goto failure;
						if (do_signal(child, __TEST_ETC))
							goto failure;
					}
					tries++;
					continue;
				}
				if (do_signal(child, __TEST_TRIES))
					goto failure;
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TRIES);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_6_1_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_6_1_ptu = { preamble_link_in_service, test_6_1_ptu, postamble_link_in_service };
static struct test_stream test_case_6_1_iut = { preamble_link_in_service, test_6_1_iut, postamble_link_in_service };

#define test_case_6_1 { &test_case_6_1_ptu, &test_case_6_1_iut, NULL }

#define tgrp_case_6_2 test_group_6
#define sgrp_case_6_2 test_subgroup_6
#define sref_case_6_2 "Q.781/6.2"
#define numb_case_6_2 "6.2"
#define name_case_6_2 "Error rate of 1 in 254 - Link out of service"
#define xtra_case_6_2 NULL
#define desc_case_6_2 "\
SUERM check\n\
Error rate of 1 in 254 - Link out of service\
"
static int
test_6_2_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			stop_tt();
			count = 253;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (tries < 8192) {
					int i;

					if (tries) {
						if (do_signal(child, __TEST_FISU_CORRUPT_S))
							goto failure;
						for (i = 0; i < count; i++)
							if (do_signal(child, __TEST_FISU_S))
								goto failure;
					} else {
						if (do_signal(child, __TEST_FISU_CORRUPT))
							goto failure;
						if (do_signal(child, __TEST_FISU))
							goto failure;
						for (i = 0; i < count; i++)
							if (do_signal(child, __TEST_FISU_S))
								goto failure;
						if (do_signal(child, __TEST_COUNT))
							goto failure;
						if (do_signal(child, __TEST_ETC))
							goto failure;
					}
					tries++;
					continue;
				}
				if (do_signal(child, __TEST_TRIES))
					goto failure;
				goto failure;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_TRIES))
					goto failure;
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				do_signal(child, __TEST_TRIES);
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_6_2_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_6_2_ptu = { preamble_link_in_service, test_6_2_ptu, postamble_link_in_service };
static struct test_stream test_case_6_2_iut = { preamble_link_in_service, test_6_2_iut, postamble_link_in_service };

#define test_case_6_2 { &test_case_6_2_ptu, &test_case_6_2_iut, NULL }

#define tgrp_case_6_3 test_group_6
#define sgrp_case_6_3 test_subgroup_6
#define sref_case_6_3 "Q.781/6.3"
#define numb_case_6_3 "6.3"
#define name_case_6_3 "Consequtive corrupt SUs"
#define xtra_case_6_3 NULL
#define desc_case_6_3 "\
SUERM check\n\
Consequtive corrupt SUs\
"
static int
test_6_3_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			count = 1;
			if (do_signal(child, __TEST_FISU_CORRUPT))
				goto failure;
			for (; count < config->sdt.T + 1; count++)
				if (do_signal(child, __TEST_FISU_CORRUPT_S))
					goto failure;
			if (start_tt(config->sl.t7 * 2))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				count++;
				if (do_signal(child, __TEST_FISU_CORRUPT_S))
					goto failure;
				if (count > 128) {
					do_signal(child, __TEST_COUNT);
					goto failure;
				}
				continue;
			case __TEST_SIOS:
				if (count > 1)
					do_signal(child, __TEST_COUNT);
				if (count > 70)
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_6_3_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_6_3_ptu = { preamble_link_in_service, test_6_3_ptu, postamble_link_in_service };
static struct test_stream test_case_6_3_iut = { preamble_link_in_service, test_6_3_iut, postamble_link_in_service };

#define test_case_6_3 { &test_case_6_3_ptu, &test_case_6_3_iut, NULL }

#define tgrp_case_6_4 test_group_6
#define sgrp_case_6_4 test_subgroup_6
#define sref_case_6_4 "Q.781/6.4"
#define numb_case_6_4 "6.4"
#define name_case_6_4 "Time controlled break of the link"
#define xtra_case_6_4 NULL
#define desc_case_6_4 "\
SUERM check\n\
Time controlled break of the link\
"
static int
test_6_4_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_TX_BREAK))
				goto failure;
			if (start_tt(80)) {
				do_signal(child, __TEST_TX_MAKE);
				goto failure;
			}
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_TX_MAKE))
					goto failure;
				if (start_tt(4000))
					goto failure;
				state++;
				continue;
			default:
				continue;

			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_6_4_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_6_4_ptu = { preamble_link_in_service, test_6_4_ptu, postamble_link_in_service };
static struct test_stream test_case_6_4_iut = { preamble_link_in_service, test_6_4_iut, postamble_link_in_service };

#define test_case_6_4 { &test_case_6_4_ptu, &test_case_6_4_iut, NULL }

#define test_group_7 "7. AERM check"
#define test_subgroup_7 "Various error rates"
#define tgrp_case_7_1 test_group_7
#define sgrp_case_7_1 test_subgroup_7
#define sref_case_7_1 "Q.781/7.1"
#define numb_case_7_1 "7.1"
#define name_case_7_1 "Error rate below the normal threshold"
#define xtra_case_7_1 NULL
#define desc_case_7_1 "\
AERM check\n\
Error rate below the normal threshold\
"
static int
test_7_1_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				state++;
				continue;
			default:
				goto failure;

			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIN:
				if (do_signal(child, last_event))
					goto failure;
				beg_time = milliseconds(child, t4n);
				if (start_tt(config->sl.t4n * 10 / 2))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (wait_event(child, 0)) {
			case __EVENT_NO_MSG:
			case __TEST_SIN:
				if (count < config->sdt.Tin - 1) {
					if (do_signal(child, __TEST_LSSU_CORRUPT))
						goto failure;
					count++;
					continue;
				} else {
					if (do_signal(child, __TEST_COUNT))
						goto failure;
					if (do_signal(child, __TEST_SIN))
						goto failure;
					if (start_tt(config->sl.t4n * 10))
						goto failure;
					state++;
					continue;
				}
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_7_1_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_7_1_ptu = { preamble_link_power_on, test_7_1_ptu, postamble_link_power_off };
static struct test_stream test_case_7_1_iut = { preamble_link_power_on, test_7_1_iut, postamble_link_power_off };

#define test_case_7_1 { &test_case_7_1_ptu, &test_case_7_1_iut, NULL }

#define tgrp_case_7_2 test_group_7
#define sgrp_case_7_2 test_subgroup_7
#define sref_case_7_2 "Q.781/7.2"
#define numb_case_7_2 "7.2"
#define name_case_7_2 "Error rate at the normal threshold"
#define xtra_case_7_2 NULL
#define desc_case_7_2 "\
AERM check\n\
Error rate at the normal threshold\
"
static int
test_7_2_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				if (start_tt(config->sl.t4n * 20))
					goto failure;
				state++;
				continue;
			default:
				continue;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				if (start_tt(config->sl.t4n * 10 / 2))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (wait_event(child, 0)) {
			case __EVENT_NO_MSG:
			case __TEST_SIN:
				if (count < config->sdt.Tin) {
					if (do_signal(child, __TEST_LSSU_CORRUPT))
						goto failure;
					count++;
					continue;
				} else {
					if (do_signal(child, __TEST_COUNT))
						goto failure;
					if (do_signal(child, __TEST_SIN))
						goto failure;
					beg_time = milliseconds(child, t4n);
					if (start_tt(config->sl.t4n * 20))
						goto failure;
					state++;
					continue;
				}
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __TEST_FISU:
				state++;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_7_2_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_7_2_ptu = { preamble_link_power_on, test_7_2_ptu, postamble_link_power_off };
static struct test_stream test_case_7_2_iut = { preamble_link_power_on, test_7_2_iut, postamble_link_power_off };

#define test_case_7_2 { &test_case_7_2_ptu, &test_case_7_2_iut, NULL }

#define tgrp_case_7_3 test_group_7
#define sgrp_case_7_3 test_subgroup_7
#define sref_case_7_3 "Q.781/7.3"
#define numb_case_7_3 "7.3"
#define name_case_7_3 "Error rate above the normal threshold"
#define xtra_case_7_3 NULL
#define desc_case_7_3 "\
AERM check\n\
Error rate above the normal threshold\
"
static int
test_7_3_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				if (start_tt(config->sl.t4n * 20))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				if (start_tt(config->sl.t4n * 10 / 2))
					goto failure;
				tries = 1;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch ((event = wait_event(child, 0))) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_COUNT))
					goto failure;
				if (tries == config->sl.M)
					break;
				goto failure;
			case __EVENT_NO_MSG:
			case __TEST_SIN:
				if (count <= config->sdt.Tin) {
					if (do_signal(child, __TEST_LSSU_CORRUPT))
						goto failure;
					count++;
				} else {
					if (do_signal(child, __TEST_COUNT))
						goto failure;
					count = 0;
					if (do_signal(child, __TEST_SIN))
						goto failure;
					if (tries < config->sl.M) {
						if (start_tt(config->sl.t4n * 10 / 2))
							goto failure;
						state--;
						tries++;
					} else {
						if (start_tt(config->sl.t4n * 20))
							goto failure;
						state++;
					}
				}
				continue;
			default:
				goto failure;
			}
			break;
		case 5:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIN))
					goto failure;
				continue;
			case __TEST_SIOS:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_7_3_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_7_3_ptu = { preamble_link_power_on, test_7_3_ptu, postamble_link_power_off };
static struct test_stream test_case_7_3_iut = { preamble_link_power_on, test_7_3_iut, postamble_link_power_off };

#define test_case_7_3 { &test_case_7_3_ptu, &test_case_7_3_iut, NULL }

#define tgrp_case_7_4 test_group_7
#define sgrp_case_7_4 test_subgroup_7
#define sref_case_7_4 "Q.781/7.4"
#define numb_case_7_4 "7.4"
#define name_case_7_4 "Error rate at the emergency threshold"
#define xtra_case_7_4 NULL
#define desc_case_7_4 "\
AERM check\n\
Error rate at the emergency threshold\
"
static int
test_7_4_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				continue;
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_SIO:
				if (do_signal(child, __TEST_SIO))
					goto failure;
				continue;
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIE))
					goto failure;
				if (start_tt(config->sl.t4e * 10 / 2))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIE))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_LSSU_CORRUPT))
					goto failure;
				for (count = 1; count < config->sdt.Tie; count++)
					if (do_signal(child, __TEST_LSSU_CORRUPT_S))
						goto failure;
				if (do_signal(child, __TEST_COUNT))
					goto failure;
				if (do_signal(child, __TEST_SIE))
					goto failure;
				beg_time = milliseconds(child, t4e);
				if (start_tt(config->sl.t4e * 20))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_SIN:
				if (do_signal(child, __TEST_SIE))
					goto failure;
				continue;
			case __TEST_FISU:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_7_4_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_START))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_IN_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_7_4_ptu = { preamble_link_power_on, test_7_4_ptu, postamble_link_power_off };
static struct test_stream test_case_7_4_iut = { preamble_link_power_on, test_7_4_iut, postamble_link_power_off };

#define test_case_7_4 { &test_case_7_4_ptu, &test_case_7_4_iut, NULL }

#define test_group_8 "8. Transmission and reception control (Basic)"
#define test_subgroup_8 "Basic transmission method"
#define tgrp_case_8_1 test_group_8
#define sgrp_case_8_1 test_subgroup_8
#define sref_case_8_1 "Q.781/8.1"
#define numb_case_8_1 "8.1"
#define name_case_8_1 "MSU transmission and reception"
#define xtra_case_8_1 NULL
#define desc_case_8_1 "\
Transmission and reception control (Basic)\n\
MSU transmission and reception\
"
static int
test_8_1_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_MSU))
					goto failure;
				if (start_tt(5000))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x80, 0xff))
						goto failure;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (check_snibs(child, 0x80, 0xff))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (check_snibs(child, 0x80, 0x80))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (check_snibs(child, 0x80, 0x80))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_1_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_1_ptu = { preamble_link_in_service, test_8_1_ptu, postamble_link_in_service };
static struct test_stream test_case_8_1_iut = { preamble_link_in_service, test_8_1_iut, postamble_link_in_service };

#define test_case_8_1 { &test_case_8_1_ptu, &test_case_8_1_iut, NULL }

#define tgrp_case_8_2 test_group_8
#define sgrp_case_8_2 test_subgroup_8
#define sref_case_8_2 "Q.781/8.2"
#define numb_case_8_2 "8.2"
#define name_case_8_2 "Negative acknowledgement of an MSU"
#define xtra_case_8_2 NULL
#define desc_case_8_2 "\
Transmission and reception control (Basic)\n\
Negative acknowledgement of an MSU\
"
static int
test_8_2_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 1:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (check_snibs(child, 0xff, 0x80))
					goto failure;
				state++;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = bib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (check_snibs(child, 0xff, 0x81))
					goto failure;
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = bib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (check_snibs(child, 0xff, 0x00))
					goto failure;
				state++;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (check_snibs(child, 0xff, 0x01))
					goto failure;
				break;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_8_2_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_8_2_ptu = { preamble_link_in_service, test_8_2_ptu, postamble_link_in_service };
static struct test_stream test_case_8_2_iut = { preamble_link_in_service, test_8_2_iut, postamble_link_in_service };

#define test_case_8_2 { &test_case_8_2_ptu, &test_case_8_2_iut, NULL }

#define tgrp_case_8_3 test_group_8
#define sgrp_case_8_3 test_subgroup_8
#define sref_case_8_3 "Q.781/8.3"
#define numb_case_8_3 "8.3"
#define name_case_8_3 "Check RTB full"
#define xtra_case_8_3 NULL
#define desc_case_8_3 "\
Transmission and reception control (Basic)\n\
Check RTB full\
"
static int
test_8_3_ptu(int child)
{
	int origin = state;

	if (msu_len > 12)
		msu_len = 12;
	for (;;) {
		int n = config->sl.N1;

		stop_tt();
		switch (state - origin) {
		case 0:
			show_fisus = 0;
			show_timeout = 1;
			if (start_tt(config->sl.t7 * 10 / 2 - 200))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] != n - 1) {
					count++;
					do_signal(child, __TEST_ETC);
					do_signal(child, __TEST_COUNT);
					bsn[0] = 0x7f;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					continue;
				}
				count++;
			case __EVENT_TIMEOUT:
				print_more(child);
				do_signal(child, __TEST_ETC);
				do_signal(child, __TEST_COUNT);
				count = 0;
				bsn[0] = fsn[0] = 0x7f;
				bib[0] = 0x00;
				fib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(config->sl.t7 * 10 / 2 + 200))
					goto failure;
				show_timeout = 1;
				show_fisus = 1;
				oldisb = 0;
				state++;
				continue;
			default:
				print_more(child);
				do_signal(child, __TEST_ETC);
				do_signal(child, __TEST_COUNT);
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] != n - 1) {
					count++;
					do_signal(child, __TEST_ETC);
					do_signal(child, __TEST_COUNT);
					bsn[0] = 0x7f;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					continue;
				}
				count++;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			default:
				print_more(child);
				do_signal(child, __TEST_ETC);
				do_signal(child, __TEST_COUNT);
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	print_more(child);
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_3_iut(int child)
{
	int i;
	int n = config->sl.N1;

	print_less(child);
	for (i = 0; i < n; i++) {
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
		state++;
	}
	for (i = 0; i < n; i++) {
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
		state++;
	}
	show_timeout = 1;
	start_tt(config->sl.t7 * 2);
	if (expect(child, INFINITE_WAIT, __EVENT_TIMEOUT))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_3_ptu = { preamble_link_in_service, test_8_3_ptu, postamble_link_in_service };
static struct test_stream test_case_8_3_iut = { preamble_link_in_service, test_8_3_iut, postamble_link_in_service };

#define test_case_8_3 { &test_case_8_3_ptu, &test_case_8_3_iut, NULL }

#define tgrp_case_8_4 test_group_8
#define sgrp_case_8_4 test_subgroup_8
#define sref_case_8_4 "Q.781/8.4"
#define numb_case_8_4 "8.4"
#define name_case_8_4 "Single MSU with erroneous FIB"
#define xtra_case_8_4 NULL
#define desc_case_8_4 "\
Transmission and reception control (Basic)\n\
Single MSU with erroneous FIB\
"
static int
test_8_4_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			wait_event(child, 0);
			fib[0] = 0x00;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			fib[0] = 0x80;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			if (start_tt(config->sl.t7 * 20))
				goto failure;
			if (start_tt(10000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
					fib[0] = 0x00;
					fsn[0] = 0x7f;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					if (start_tt(config->sl.t7 * 20))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x7f || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x00, 0xff))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_4_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_4_ptu = { preamble_link_in_service, test_8_4_ptu, postamble_link_in_service };
static struct test_stream test_case_8_4_iut = { preamble_link_in_service, test_8_4_iut, postamble_link_in_service };

#define test_case_8_4 { &test_case_8_4_ptu, &test_case_8_4_iut, NULL }

#define tgrp_case_8_5 test_group_8
#define sgrp_case_8_5 test_subgroup_8
#define sref_case_8_5 "Q.781/8.5"
#define numb_case_8_5 "8.5"
#define name_case_8_5 "Duplicated FSN"
#define xtra_case_8_5 NULL
#define desc_case_8_5 "\
Transmission and reception control (Basic)\n\
Duplicated FSN\
"
static int
test_8_5_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			wait_event(child, 0);
			if (do_signal(child, __TEST_MSU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x80, 0xff))
						goto failure;
					fsn[0] = 0x7f;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					fsn[0] = 0x01;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x80 || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x00, 0xff))
						goto failure;
					fib[0] = 0x00;
					fsn[0] = 0x00;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x00 || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x01, 0xff))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_5_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_5_ptu = { preamble_link_in_service, test_8_5_ptu, postamble_link_in_service };
static struct test_stream test_case_8_5_iut = { preamble_link_in_service, test_8_5_iut, postamble_link_in_service };

#define test_case_8_5 { &test_case_8_5_ptu, &test_case_8_5_iut, NULL }

#define tgrp_case_8_6 test_group_8
#define sgrp_case_8_6 test_subgroup_8
#define sref_case_8_6 "Q.781/8.6"
#define numb_case_8_6 "8.6"
#define name_case_8_6 "Erroneous retransmission - Single MSU"
#define xtra_case_8_6 NULL
#define desc_case_8_6 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Single MSU\
"
static int
test_8_6_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			wait_event(child, 0);
			fib[0] = 0x00;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
					fib[0] = 0x00;
					fsn[0] = 0x7f;
					oldmsg = 0;
					cntmsg = 0;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x7f || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x00, 0xff))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_8_6_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_8_6_ptu = { preamble_link_in_service, test_8_6_ptu, postamble_link_in_service };
static struct test_stream test_case_8_6_iut = { preamble_link_in_service, test_8_6_iut, postamble_link_in_service };

#define test_case_8_6 { &test_case_8_6_ptu, &test_case_8_6_iut, NULL }

#define tgrp_case_8_7 test_group_8
#define sgrp_case_8_7 test_subgroup_8
#define sref_case_8_7 "Q.781/8.7"
#define numb_case_8_7 "8.7"
#define name_case_8_7 "Erroneous retransmission - Multiple FISUs"
#define xtra_case_8_7 NULL
#define desc_case_8_7 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Multiple FISUs\
"
static int
test_8_7_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(1000))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_8_7_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_8_7_ptu = { preamble_link_in_service, test_8_7_ptu, postamble_link_in_service };
static struct test_stream test_case_8_7_iut = { preamble_link_in_service, test_8_7_iut, postamble_link_in_service };

#define test_case_8_7 { &test_case_8_7_ptu, &test_case_8_7_iut, NULL }


#define tgrp_case_8_8 test_group_8
#define sgrp_case_8_8 test_subgroup_8
#define sref_case_8_8 "Q.781/8.8"
#define numb_case_8_8 "8.8"
#define name_case_8_8 "Single FISU with corrupt FIB"
#define xtra_case_8_8 NULL
#define desc_case_8_8 "\
Transmission and reception control (Basic)\n\
Single FISU with corrupt FIB\
"
static int
test_8_8_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(LONG_WAIT))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_8_8_iut(int child)
{
#if 1
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_8_8_ptu = { preamble_link_in_service, test_8_8_ptu, postamble_link_in_service };
static struct test_stream test_case_8_8_iut = { preamble_link_in_service, test_8_8_iut, postamble_link_in_service };

#define test_case_8_8 { &test_case_8_8_ptu, &test_case_8_8_iut, NULL }

#define tgrp_case_8_9a test_group_8
#define sgrp_case_8_9a test_subgroup_8
#define sref_case_8_9a "Q.781/8.9"
#define numb_case_8_9a "8.9(a)"
#define name_case_8_9a "Single FISU prior to RPO being set"
#define xtra_case_8_9a "Forward"
#define desc_case_8_9a "\
Transmission and reception control (Basic)\n\
Single FISU prior to RPO being set\n\
Forward direction\
"
static int
test_8_9a_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			fib[0] = 0x00;
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(1000))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
					fib[0] = 0x00;
					fsn[0] = 0x7f;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					state++;
				} else {
					if (do_signal(child, __TEST_FISU))
						goto failure;
				}
				continue;
			default:
				goto failure;
			}
			break;
		case 5:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x7f || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x00, 0xff))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_9a_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_9a_ptu = { preamble_link_in_service, test_8_9a_ptu, postamble_link_in_service };
static struct test_stream test_case_8_9a_iut = { preamble_link_in_service, test_8_9a_iut, postamble_link_in_service };

#define test_case_8_9a { &test_case_8_9a_ptu, &test_case_8_9a_iut, NULL }

#define tgrp_case_8_9b test_group_8
#define sgrp_case_8_9b test_subgroup_8
#define sref_case_8_9b "Q.781/8.9"
#define numb_case_8_9b "8.9(b)"
#define name_case_8_9b "Single FISU prior to RPO being set"
#define xtra_case_8_9b "Reverse"
#define desc_case_8_9b "\
Transmission and reception control (Basic)\n\
Single FISU prior to RPO being set\n\
Reverse direction\
"
static int
test_8_9b_ptu(int child)
{
	if (expect(child, INFINITE_WAIT, __TEST_DATA))
		goto failure;
	state++;
	bsn[0] = fsn[1];
	if (expect(child, INFINITE_WAIT, __TEST_SIPO))
		goto failure;
	state++;
	bsn[0] = fsn[1];
	if (do_signal(child, __TEST_ACK))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __STATUS_PROCESSOR_ENDED))
		goto failure;
	state++;
	if (do_signal(child, __STATUS_SEQUENCE_SYNC))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_DATA))
		goto failure;
	state++;
	bsn[0] = fsn[1];
	if (do_signal(child, __TEST_ACK))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_8_9b_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPO))
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_LPR))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_9b_ptu = { preamble_link_in_service, test_8_9b_ptu, postamble_link_in_service };
static struct test_stream test_case_8_9b_iut = { preamble_link_in_service, test_8_9b_iut, postamble_link_in_service };

#define test_case_8_9b { &test_case_8_9b_ptu, &test_case_8_9b_iut, NULL }

#define tgrp_case_8_10 test_group_8
#define sgrp_case_8_10 test_subgroup_8
#define sref_case_8_10 "Q.781/8.10"
#define numb_case_8_10 "8.10"
#define name_case_8_10 "Abnormal BSN - single MSU"
#define xtra_case_8_10 NULL
#define desc_case_8_10 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - single MSU\
"
static int
test_8_10_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			wait_event(child, 0);
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				bsn[0] = 0x3f;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x7f, 0xff))
						goto failure;
					fib[0] = 0x00;
					fsn[0] = 0x7f;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					state++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 4:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x7f || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(child, 0x00, 0xff))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_10_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_10_ptu = { preamble_link_in_service, test_8_10_ptu, postamble_link_in_service };
static struct test_stream test_case_8_10_iut = { preamble_link_in_service, test_8_10_iut, postamble_link_in_service };

#define test_case_8_10 { &test_case_8_10_ptu, &test_case_8_10_iut, NULL }

#define tgrp_case_8_11 test_group_8
#define sgrp_case_8_11 test_subgroup_8
#define sref_case_8_11 "Q.781/8.11"
#define numb_case_8_11 "8.11"
#define name_case_8_11 "Abnormal BSN - two consecutive FISUs"
#define xtra_case_8_11 NULL
#define desc_case_8_11 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - two consecutive FISUs\
"
static int
test_8_11_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			wait_event(child, 0);
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case 1:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				bsn[0] = 0x3f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				oldmsg = 0;
				cntmsg = 0;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(1000))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_11_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_11_ptu = { preamble_link_in_service, test_8_11_ptu, postamble_link_out_of_service };
static struct test_stream test_case_8_11_iut = { preamble_link_in_service, test_8_11_iut, postamble_link_out_of_service };

#define test_case_8_11 { &test_case_8_11_ptu, &test_case_8_11_iut, NULL }

#define tgrp_case_8_12a test_group_8
#define sgrp_case_8_12a test_subgroup_8
#define sref_case_8_12a "Q.781/8.12"
#define numb_case_8_12a "8.12(a)"
#define name_case_8_12a "Excessive delay of acknowledgement"
#define xtra_case_8_12a "Single data"
#define desc_case_8_12a "\
Transmission and reception control (Basic)\n\
Excessive delay of acknowledgement\n\
Single data\
"
static int
test_8_12a_ptu(int child)
{
	long beg_time = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (wait_event(child, 0)) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (start_tt(config->sl.t7 * 20))
					goto failure;
				beg_time = milliseconds(child, t7);
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (check_time(child, "T7  ", beg_time, timer[t7].lo, timer[t7].hi))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_8_12a_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_12a_ptu = { preamble_link_in_service, test_8_12a_ptu, postamble_link_out_of_service };
static struct test_stream test_case_8_12a_iut = { preamble_link_in_service, test_8_12a_iut, postamble_link_out_of_service };

#define test_case_8_12a { &test_case_8_12a_ptu, &test_case_8_12a_iut, NULL }

#define tgrp_case_8_12b test_group_8
#define sgrp_case_8_12b test_subgroup_8
#define sref_case_8_12b "Q.781/8.12"
#define numb_case_8_12b "8.12(b)"
#define name_case_8_12b "Excessive delay of acknowledgement"
#define xtra_case_8_12b "Multiple data"
#define desc_case_8_12b "\
Transmission and reception control (Basic)\n\
Excessive delay of acknowledgement\n\
Multiple data\
"
static int
test_8_12b_ptu(int child)
{
	long beg_time = 0;

	if (expect(child, INFINITE_WAIT, __TEST_DATA))
		goto failure;
	state++;
	beg_time = milliseconds(child, t7);
	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_DATA:
			state++;
			continue;
		case __TEST_SIOS:
			state++;
			break;
		default:
			goto failure;
		}
		break;
	}
	if (check_time(child, "T7  ", beg_time, timer[t7].lo, timer[t7].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_8_12b_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, config->sl.t7 / 2, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_12b_ptu = { preamble_link_in_service, test_8_12b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_8_12b_iut = { preamble_link_in_service, test_8_12b_iut, postamble_link_out_of_service };

#define test_case_8_12b { &test_case_8_12b_ptu, &test_case_8_12b_iut, NULL }

#define tgrp_case_8_13 test_group_8
#define sgrp_case_8_13 test_subgroup_8
#define sref_case_8_13 "Q.781/8.13"
#define numb_case_8_13 "8.13"
#define name_case_8_13 "Level 3 Stop command"
#define xtra_case_8_13 NULL
#define desc_case_8_13 "\
Transmission and reception control (Basic)\n\
Level 3 Stop command\
"
static int
test_8_13_ptu(int child)
{
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			continue;
		case __TEST_SIOS:
			if (do_signal(child, __TEST_SIOS))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_8_13_iut(int child)
{
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_13_ptu = { preamble_link_in_service, test_8_13_ptu, postamble_link_out_of_service };
static struct test_stream test_case_8_13_iut = { preamble_link_in_service, test_8_13_iut, postamble_link_out_of_service };

#define test_case_8_13 { &test_case_8_13_ptu, &test_case_8_13_iut, NULL }

#define tgrp_case_8_14 test_group_8
#define sgrp_case_8_14 test_subgroup_8
#define sref_case_8_14 "Q.781/8.14"
#define numb_case_8_14 "8.14"
#define name_case_8_14 "Abnormal FIBR"
#define xtra_case_8_14 NULL
#define desc_case_8_14 "\
Transmission and reception control (Basic)\n\
Abnormal FIBR\
"
static int
test_8_14_ptu(int child)
{
	bsn[0] = 0x7f;
	if (do_signal(child, __TEST_DATA))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_ACK))
		goto failure;
	state++;
	if (fsn[1] != 0x7f)
		goto failure;
	if (bsn[1] != 0x7f)
		if (bsn[1] != 0)
			goto failure;
	fsn[0] = 1;
	if (do_signal(child, __TEST_DATA))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_8_14_iut(int child)
{
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_8_14_ptu = { preamble_link_in_service, test_8_14_ptu, postamble_link_in_service };
static struct test_stream test_case_8_14_iut = { preamble_link_in_service, test_8_14_iut, postamble_link_in_service };

#define test_case_8_14 { &test_case_8_14_ptu, &test_case_8_14_iut, NULL }

#define test_group_9 "9. Transmission and reception control (PCR)"
#define test_subgroup_9 "Preventative Cyclic Retransmission method"
#define tgrp_case_9_1 test_group_9
#define sgrp_case_9_1 test_subgroup_9
#define sref_case_9_1 "Q.781/9.1"
#define numb_case_9_1 "9.1"
#define name_case_9_1 "MSU transmission and reception"
#define xtra_case_9_1 NULL
#define desc_case_9_1 "\
Transmission and reception control (PCR)\n\
MSU transmission and reception\
"
static int
test_9_1_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (count < 4) {
					cntret = -1;
					bsn[0] = 0x7f;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					count++;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_FISU:
				if (check_snibs(child, 0xff, 0x80))
					goto failure;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				if ((bsn[1] | bib[1]) != 0xff || (fsn[1] | fib[1]) != 0x80) {
					if (check_snibs(child, 0x80, 0x80))
						goto failure;
					break;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_1_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_1_ptu = { preamble_link_in_service_pcr, test_9_1_ptu, postamble_link_in_service };
static struct test_stream test_case_9_1_iut = { preamble_link_in_service_pcr, test_9_1_iut, postamble_link_in_service };

#define test_case_9_1 { &test_case_9_1_ptu, &test_case_9_1_iut, NULL }

#define tgrp_case_9_2 test_group_9
#define sgrp_case_9_2 test_subgroup_9
#define sref_case_9_2 "Q.781/9.2"
#define numb_case_9_2 "9.2"
#define name_case_9_2 "Priority control"
#define xtra_case_9_2 NULL
#define desc_case_9_2 "\
Transmission and reception control (PCR)\n\
Priority control\
"
static int
test_9_2_ptu(int child)
{
#if 1
	int fsn_lo = 0, fsn_hi = 0, fsn_ex = 0;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(20000))
				goto failure;
			fsn_lo = 0;
			if (do_signal(CHILD_IUT, __TEST_SEND_MSU))
				goto failure;
			if (do_signal(CHILD_IUT, __TEST_SEND_MSU))
				goto failure;
			fsn_hi = 1;
			fsn_ex = fsn_lo;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				if (fsn[1] < fsn_hi) {
					if (fsn[1] != fsn_ex)
						goto failure;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					bsn[0] = 0x7f;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					continue;
				}
				if (fsn[1] != fsn_hi)
					goto failure;
				if (do_signal(CHILD_IUT, __TEST_ETC))
					goto failure;
				if (do_signal(CHILD_IUT, __TEST_TRIES))
					goto failure;
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (do_signal(CHILD_IUT, __TEST_SEND_MSU))
					goto failure;
				fsn_hi++;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] < fsn_hi) {
					if (fsn[1] != fsn_ex)
						goto failure;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					bsn[0] = 0x7f;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					continue;
				}
				if (fsn[1] != fsn_hi)
					goto failure;
				if (do_signal(CHILD_IUT, __TEST_ETC))
					goto failure;
				if (do_signal(CHILD_IUT, __TEST_TRIES))
					goto failure;
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (fsn[1] == 2)
					break;
				goto failure;
			case __TEST_MSU:
				bsn[0] = fsn_lo;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] != fsn_ex) {
					if (fsn[1] == fsn_ex + 1) {
						if (fsn_lo < fsn_hi) {
							fsn_lo++;
							fsn_ex++;
							if (do_signal(CHILD_IUT, __TEST_ETC))
								goto failure;
							if (do_signal(CHILD_IUT, __TEST_TRIES))
								goto failure;
							tries = 0;
						}
					} else
						fsn_ex--;
				} else if (++tries > 100)
					goto failure;
				if (++fsn_ex > fsn_hi)
					fsn_ex = fsn_lo;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_2_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_2_ptu = { preamble_link_in_service_pcr, test_9_2_ptu, postamble_link_in_service };
static struct test_stream test_case_9_2_iut = { preamble_link_in_service_pcr, test_9_2_iut, postamble_link_in_service };

#define test_case_9_2 { &test_case_9_2_ptu, &test_case_9_2_iut, NULL }

#define tgrp_case_9_3 test_group_9
#define sgrp_case_9_3 test_subgroup_9
#define sref_case_9_3 "Q.781/9.3"
#define numb_case_9_3 "9.3"
#define name_case_9_3 "Forced retransmission with the value N1"
#define xtra_case_9_3 NULL
#define desc_case_9_3 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N1\
"
static int
test_9_3_ptu(int child)
{
#if 1
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;
	int origin = state;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3)
		goto inconclusive;
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n,
	       msu_len);
	fflush(stdout);

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(config->sl.t7 * 10 + 1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				count = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n)
					goto failure;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 0 && count) {
					bsn[0] = 0x0;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					count = 1;
					state++;
					continue;
				}
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n) {
					if (do_signal(child, __TEST_FISU))
						goto failure;
					break;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = 0;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_3_iut(int child)
{
#if 1
	int i;
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3)
		goto inconclusive;
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n,
	       msu_len);
	fflush(stdout);

	for (i = 0; i < n + 1; i++)
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_3_ptu = { preamble_link_in_service_pcr, test_9_3_ptu, postamble_link_in_service };
static struct test_stream test_case_9_3_iut = { preamble_link_in_service_pcr, test_9_3_iut, postamble_link_in_service };

#define test_case_9_3 { &test_case_9_3_ptu, &test_case_9_3_iut, NULL }

#define tgrp_case_9_4 test_group_9
#define sgrp_case_9_4 test_subgroup_9
#define sref_case_9_4 "Q.781/9.4"
#define numb_case_9_4 "9.4"
#define name_case_9_4 "Forced retransmission with the value N2"
#define xtra_case_9_4 NULL
#define desc_case_9_4 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N2\
"
static int
test_9_4_ptu(int child)
{
#if 1
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1 - 1;
	int origin = state;

	msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
	n = config->sl.N2 / (msu_len + h) + 1;
	if (msu_len > config->sdt.m)
		goto inconclusive;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(config->sl.t7 * 10 + 1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				count = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n)
					goto failure;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 0 && count) {
					bsn[0] = 0x0;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					count = 1;
					state++;
					continue;
				}
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n) {
					if (do_signal(child, __TEST_FISU))
						goto failure;
					break;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = 0;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_4_iut(int child)
{
#if 1
	int i;
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1 - 1;

	msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
	n = config->sl.N2 / (msu_len + h) + 1;
	if (msu_len > config->sdt.m)
		goto inconclusive;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (i = 0; i < n + 1; i++)
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
	state++;
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_4_ptu = { preamble_link_in_service_pcr, test_9_4_ptu, postamble_link_in_service };
static struct test_stream test_case_9_4_iut = { preamble_link_in_service_pcr, test_9_4_iut, postamble_link_in_service };

#define test_case_9_4 { &test_case_9_4_ptu, &test_case_9_4_iut, NULL }

#define tgrp_case_9_5 test_group_9
#define sgrp_case_9_5 test_subgroup_9
#define sref_case_9_5 "Q.781/9.5"
#define numb_case_9_5 "9.5"
#define name_case_9_5 "Forced retransmission cancel"
#define xtra_case_9_5 NULL
#define desc_case_9_5 "\
Transmission and reception control (PCR)\n\
Forced retransmission cancel\
"
static int
test_9_5_ptu(int child)
{
#if 1
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;
	int origin = state;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3) {
		n = config->sl.N1 - 1;
		msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
		n = config->sl.N2 / (msu_len + h) + 1;
		if (msu_len > config->sdt.m)
			goto inconclusive;
	}
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(config->sl.t7 * 10 + 1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				count = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n)
					goto failure;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 3 && count) {
					bsn[0] = n - 1;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					count = 1;
					state++;
					continue;
				}
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n - 3)
					goto failure;
				if (fsn[1] == n) {
					if (do_signal(child, __TEST_FISU))
						goto failure;
					break;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = n - 1;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_5_iut(int child)
{
#if 1
	int i;
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3) {
		n = config->sl.N1 - 1;
		msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
		n = config->sl.N2 / (msu_len + h) + 1;
		if (msu_len > config->sdt.m)
			goto inconclusive;
	}
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (i = 0; i < n + 1; i++)
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_5_ptu = { preamble_link_in_service_pcr, test_9_5_ptu, postamble_link_in_service };
static struct test_stream test_case_9_5_iut = { preamble_link_in_service_pcr, test_9_5_iut, postamble_link_in_service };

#define test_case_9_5 { &test_case_9_5_ptu, &test_case_9_5_iut, NULL }

#define tgrp_case_9_6 test_group_9
#define sgrp_case_9_6 test_subgroup_9
#define sref_case_9_6 "Q.781/9.6"
#define numb_case_9_6 "9.6"
#define name_case_9_6 "Reception of forced retransmission"
#define xtra_case_9_6 NULL
#define desc_case_9_6 "\
Transmission and reception control (PCR)\n\
Reception of forced retransmission\
"
static int
test_9_6_ptu(int child)
{
#if 1
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;
	int origin = state;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3) {
		n = config->sl.N1 - 1;
		msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
		n = config->sl.N2 / (msu_len + h) + 1;
		if (msu_len > config->sdt.m)
			goto inconclusive;
	}
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(config->sl.t7 * 10 + 1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				count = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				if (fsn[1] == n)
					goto failure;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 3 && count == 2) {
					bsn[0] = n - 1;
					if (do_signal(child, __TEST_FISU))
						goto failure;
					break;
				}
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (fsn[1] == 2)
					if (do_signal(child, __TEST_ETC))
						goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_6_iut(int child)
{
#if 1
	int i;
	int h = (config->opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = config->sl.N1;

	msu_len = config->sl.N2 / config->sl.N1 - h - 1;
	if (msu_len < 3) {
		n = config->sl.N1 - 1;
		msu_len = config->sl.N2 / (config->sl.N1 - 1) - h + 1;
		n = config->sl.N2 / (msu_len + h) + 1;
		if (msu_len > config->sdt.m)
			goto inconclusive;
	}
	if (msu_len > 12)
		msu_len = 12;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", (long) config->sl.N1, (long) config->sl.N2, n, msu_len);
	fflush(stdout);

	for (i = 0; i < n + 1; i++)
		if (do_signal(child, __TEST_SEND_MSU))
			goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      inconclusive:
	return __RESULT_INCONCLUSIVE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_6_ptu = { preamble_link_in_service_pcr, test_9_6_ptu, postamble_link_in_service };
static struct test_stream test_case_9_6_iut = { preamble_link_in_service_pcr, test_9_6_iut, postamble_link_in_service };

#define test_case_9_6 { &test_case_9_6_ptu, &test_case_9_6_iut, NULL }

#define tgrp_case_9_7 test_group_9
#define sgrp_case_9_7 test_subgroup_9
#define sref_case_9_7 "Q.781/9.7"
#define numb_case_9_7 "9.7"
#define name_case_9_7 "MSU transmission while RPO set"
#define xtra_case_9_7 NULL
#define desc_case_9_7 "\
Transmission and reception control (PCR)\n\
MSU transmission while RPO set\
"
static int
test_9_7_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(5000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_LPO))
					goto failure;
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				if (start_tt(config->sl.t7 * 20))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				continue;
			case __TEST_FISU:
				if (!count++)
					if (check_snibs(child, 0xff, 0x80))
						goto failure;
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_SIPO))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_LPR))
					goto failure;
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				if (start_tt(500))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != 0) {
					bsn[0] = 0x7f;
					fsn[0] = 0x7f;
					if (do_signal(child, __TEST_MSU))
						goto failure;
					continue;
				}
				if (do_signal(child, __TEST_FISU))
					goto failure;
				break;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_7_iut(int child)
{
#if 0
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPO))
		goto failure;
	state++;
	if (do_signal(child, __TEST_CLEARB))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_RPR))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_7_ptu = { preamble_link_in_service_pcr, test_9_7_ptu, postamble_link_in_service };
static struct test_stream test_case_9_7_iut = { preamble_link_in_service_pcr, test_9_7_iut, postamble_link_in_service };

#define test_case_9_7 { &test_case_9_7_ptu, &test_case_9_7_iut, NULL }

#define tgrp_case_9_8 test_group_9
#define sgrp_case_9_8 test_subgroup_9
#define sref_case_9_8 "Q.781/9.8"
#define numb_case_9_8 "9.8"
#define name_case_9_8 "Abnormal BSN - Single MSU"
#define xtra_case_9_8 NULL
#define desc_case_9_8 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Single MSU\
"
static int
test_9_8_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			bsn[0] = 0x00;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			bsn[0] = 0x7f;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			bsn[0] = 0x7f;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			count = 2;
			if (do_signal(child, __TEST_COUNT))
				goto failure;
			if (start_tt(10000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] == 0)
					break;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_8_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_8_ptu = { preamble_link_in_service_pcr, test_9_8_ptu, postamble_link_in_service };
static struct test_stream test_case_9_8_iut = { preamble_link_in_service_pcr, test_9_8_iut, postamble_link_in_service };

#define test_case_9_8 { &test_case_9_8_ptu, &test_case_9_8_iut, NULL }

#define tgrp_case_9_9 test_group_9
#define sgrp_case_9_9 test_subgroup_9
#define sref_case_9_9 "Q.781/9.9"
#define numb_case_9_9 "9.9"
#define name_case_9_9 "Abnormal BSN - Two MSUs"
#define xtra_case_9_9 NULL
#define desc_case_9_9 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Two MSUs\
"
static int
test_9_9_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			bsn[0] = 0x7e;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			bsn[0] = 0x7f;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			bsn[0] = 0x7e;
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			if (start_tt(1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				fsn[0] = 0x7f;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_9_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_9_ptu = { preamble_link_in_service_pcr, test_9_9_ptu, postamble_link_in_service };
static struct test_stream test_case_9_9_iut = { preamble_link_in_service_pcr, test_9_9_iut, postamble_link_in_service };

#define test_case_9_9 { &test_case_9_9_ptu, &test_case_9_9_iut, NULL }

#define tgrp_case_9_10 test_group_9
#define sgrp_case_9_10 test_subgroup_9
#define sref_case_9_10 "Q.781/9.10"
#define numb_case_9_10 "9.10"
#define name_case_9_10 "Unexpected FSN"
#define xtra_case_9_10 NULL
#define desc_case_9_10 "\
Transmission and reception control (PCR)\n\
Unexpected FSN\
"
static int
test_9_10_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			fsn[0] = 0x7f;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			fsn[0] = 0x01;
			if (do_signal(child, __TEST_MSU))
				goto failure;
			if (start_tt(1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (bsn[1] != 0x7f) {
					if (check_snibs(child, 0x80, 0xff))
						goto failure;
					break;
				}
				fsn[0] = 0x7f;
				if (do_signal(child, __TEST_MSU))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_10_iut(int child)
{
#if 1
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_DATA))
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_10_ptu = { preamble_link_in_service_pcr, test_9_10_ptu, postamble_link_in_service };
static struct test_stream test_case_9_10_iut = { preamble_link_in_service_pcr, test_9_10_iut, postamble_link_in_service };

#define test_case_9_10 { &test_case_9_10_ptu, &test_case_9_10_iut, NULL }

#define tgrp_case_9_11 test_group_9
#define sgrp_case_9_11 test_subgroup_9
#define sref_case_9_11 "Q.781/9.11"
#define numb_case_9_11 "9.11"
#define name_case_9_11 "Excessive delay of acknowledgement"
#define xtra_case_9_11 NULL
#define desc_case_9_11 "\
Transmission and reception control (PCR)\n\
Excessive delay of acknowledgement\
"
static int
test_9_11_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				beg_time = milliseconds(child, t7);
				if (start_tt(config->sl.t7 * 20))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_MSU:
				bsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (check_time(child, "T7  ", beg_time, timer[t7].lo, timer[t7].hi))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_11_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_11_ptu = { preamble_link_in_service_pcr, test_9_11_ptu, postamble_link_in_service };
static struct test_stream test_case_9_11_iut = { preamble_link_in_service_pcr, test_9_11_iut, postamble_link_in_service };

#define test_case_9_11 { &test_case_9_11_ptu, &test_case_9_11_iut, NULL }

#define tgrp_case_9_12 test_group_9
#define sgrp_case_9_12 test_subgroup_9
#define sref_case_9_12 "Q.781/9.12"
#define numb_case_9_12 "9.12"
#define name_case_9_12 "FISU with FSN expected for MSU"
#define xtra_case_9_12 NULL
#define desc_case_9_12 "\
Transmission and reception control (PCR)\n\
FISU with FSN expected for MSU\
"
static int
test_9_12_ptu(int child)
{
#if 1
	fsn[0] = 0x00;
	if (do_signal(child, __TEST_FISU))
		goto failure;
	fsn[0] = 0x7f;
	state++;
	if (start_tt(1000))
		goto failure;

	state++;
	for (;;) {
		switch (get_event(child)) {
		case __TEST_FISU:
			if (do_signal(child, __TEST_FISU))
				goto failure;
			state++;
			continue;
		case __EVENT_TIMEOUT:
			if (check_snibs(child, 0xff, 0xff))
				goto failure;
			break;
		default:
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_12_iut(int child)
{
#if 1
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_12_ptu = { preamble_link_in_service_pcr, test_9_12_ptu, postamble_link_in_service };
static struct test_stream test_case_9_12_iut = { preamble_link_in_service_pcr, test_9_12_iut, postamble_link_in_service };

#define test_case_9_12 { &test_case_9_12_ptu, &test_case_9_12_iut, NULL }

#define tgrp_case_9_13 test_group_9
#define sgrp_case_9_13 test_subgroup_9
#define sref_case_9_13 "Q.781/9.13"
#define numb_case_9_13 "9.13"
#define name_case_9_13 "Level 3 Stop command"
#define xtra_case_9_13 NULL
#define desc_case_9_13 "\
Transmission and reception control (PCR)\n\
Level 3 Stop command\
"
static int
test_9_13_ptu(int child)
{
#if 1
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			if (start_tt(1000))
				goto failure;
			state++;
			continue;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (do_signal(child, __TEST_SIOS))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}
static int
test_9_13_iut(int child)
{
#if 1
	if (do_signal(child, __TEST_STOP))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
#else
	return __RESULT_NOTAPPL;	/* can't do this */
#endif
}

static struct test_stream test_case_9_13_ptu = { preamble_link_in_service_pcr, test_9_13_ptu, postamble_link_in_service };
static struct test_stream test_case_9_13_iut = { preamble_link_in_service_pcr, test_9_13_iut, postamble_link_in_service };

#define test_case_9_13 { &test_case_9_13_ptu, &test_case_9_13_iut, NULL }

#define test_group_10 "10. Congestion Control"
#define test_subgroup_10 "Congestion onset and abatement"
#define tgrp_case_10_1 test_group_10
#define sgrp_case_10_1 test_subgroup_10
#define sref_case_10_1 "Q.781/10.1"
#define numb_case_10_1 "10.1"
#define name_case_10_1 "Congestion abatement"
#define xtra_case_10_1 NULL
#define desc_case_10_1 "\
Congestion Control\n\
Congestion abatement\
"
static int
test_10_1_ptu(int child)
{
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIB:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				beg_time = milliseconds(child, t5);
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIB:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (check_time(child, "T5  ", beg_time, timer[t5].lo, timer[t5].hi))
					goto failure;
				if (start_tt(config->sl.t5 * 20))
					goto failure;
				count = 0;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_SIB:
				if (count++ > 0)
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_10_1_iut(int child)
{
	if (do_signal(child, __TEST_CONG_D))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __TEST_NO_CONG))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_10_1_ptu = { preamble_link_in_service, test_10_1_ptu, postamble_link_in_service };
static struct test_stream test_case_10_1_iut = { preamble_link_in_service, test_10_1_iut, postamble_link_in_service };

#define test_case_10_1 { &test_case_10_1_ptu, &test_case_10_1_iut, NULL }

#define tgrp_case_10_2a test_group_10
#define sgrp_case_10_2a test_subgroup_10
#define sref_case_10_2a "Q.781/10.2"
#define numb_case_10_2a "10.2(a)"
#define name_case_10_2a "Timer T7 during receive congestion"
#define xtra_case_10_2a NULL
#define desc_case_10_2a "\
Congestion Control\n\
Timer T7\n\
During receive congestion\
"
static int
test_10_2a_ptu(int child)
{
	int n = (config->sl.t6 - config->sl.t7) / config->sl.t5;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = fsn[0] = 0x7f;
				if (start_tt(config->sl.t5 * 10))
					goto failure;
				if (do_signal(child, __TEST_SIB))
					goto failure;
				if (do_signal(child, __TEST_FISU_S))
					goto failure;
				beg_time = milliseconds(child, t6);
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_SIB_S))
					goto failure;
				count++;
				if (count == n) {
					if (do_signal(child, __TEST_COUNT))
						goto failure;
					if (start_tt(config->sl.t7 * 10 - 200))
						goto failure;
					state++;
					continue;
				}
				if (start_tt(config->sl.t5 * 10))
					goto failure;
			case __TEST_FISU:
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU_S))
					goto failure;
				continue;
			default:
				goto failure;
			}
			break;
		case 2:
			switch (get_event(child)) {
			case __TEST_FISU:
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				bsn[0] = 0x00;
				if (do_signal(child, __TEST_FISU))
					goto failure;
				if (start_tt(1000))
					goto failure;
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 3:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __EVENT_TIMEOUT:
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_10_2a_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_10_2a_ptu = { preamble_link_in_service, test_10_2a_ptu, postamble_link_in_service };
static struct test_stream test_case_10_2a_iut = { preamble_link_in_service, test_10_2a_iut, postamble_link_in_service };

#define test_case_10_2a { &test_case_10_2a_ptu, &test_case_10_2a_iut, NULL }

#define tgrp_case_10_2b test_group_10
#define sgrp_case_10_2b test_subgroup_10
#define sref_case_10_2b "Q.781/10.2"
#define numb_case_10_2b "10.2(b)"
#define name_case_10_2b "Timer T7 after receive congestion"
#define xtra_case_10_2b NULL
#define desc_case_10_2b "\
Congestion Control\n\
Timer T7\n\
After receive congestion\
"
static int
test_10_2b_ptu(int child)
{
	long beg_time = 0;

	if (expect(child, INFINITE_WAIT, __TEST_DATA))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIB))
		goto failure;
	state++;
	beg_time = milliseconds(child, t6);
	state++;
	if (expect(child, config->sl.t6 - 300, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __STATUS_BUSY_ENDED))
		goto failure;
	state++;
	beg_time = milliseconds(child, t7);
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_SIOS))
		goto failure;
	state++;
	if (check_time(child, "T7  ", beg_time, timer[t7].lo, timer[t7].hi))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_10_2b_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_10_2b_ptu = { preamble_link_in_service, test_10_2b_ptu, postamble_link_out_of_service };
static struct test_stream test_case_10_2b_iut = { preamble_link_in_service, test_10_2b_iut, postamble_link_out_of_service };

#define test_case_10_2b { &test_case_10_2b_ptu, &test_case_10_2b_iut, NULL }

#define tgrp_case_10_2c test_group_10
#define sgrp_case_10_2c test_subgroup_10
#define sref_case_10_2c "Q.781/10.2"
#define numb_case_10_2c "10.2(c)"
#define name_case_10_2c "Timer T7 after receive congestion"
#define xtra_case_10_2c NULL
#define desc_case_10_2c "\
Congestion Control\n\
Timer T7\n\
After receive congestion\
"
static int
test_10_2c_ptu(int child)
{
	long beg_time = 0;

	if (expect(child, INFINITE_WAIT, __TEST_DATA))
		goto failure;
	state++;
	if (do_signal(child, __TEST_SIB))
		goto failure;
	state++;
	beg_time = milliseconds(child, t6);
	state++;
	if (expect(child, config->sl.t6 - 300, __EVENT_NO_MSG))
		goto failure;
	state++;
	if (do_signal(child, __STATUS_BUSY_ENDED))
		goto failure;
	state++;
	beg_time = milliseconds(child, t7);
	state++;
	if (expect(child, config->sl.t7 - 300, __EVENT_NO_MSG))
		goto failure;
	state++;
	bsn[0] = fsn[1];
	if (do_signal(child, __TEST_ACK))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
test_10_2c_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_10_2c_ptu = { preamble_link_in_service, test_10_2c_ptu, postamble_link_in_service };
static struct test_stream test_case_10_2c_iut = { preamble_link_in_service, test_10_2c_iut, postamble_link_in_service };

#define test_case_10_2c { &test_case_10_2c_ptu, &test_case_10_2c_iut, NULL }

#define tgrp_case_10_3 test_group_10
#define sgrp_case_10_3 test_subgroup_10
#define sref_case_10_3 "Q.781/10.3"
#define numb_case_10_3 "10.3"
#define name_case_10_3 "Timer T6"
#define xtra_case_10_3 NULL
#define desc_case_10_3 "\
Congestion Control\n\
Timer T6\
"
static int
test_10_3_ptu(int child)
{
	int n = (config->sl.t6 + 20) / config->sl.t5 + 20;
	int origin = state;

	for (;;) {
		switch (state - origin) {
		case 0:
			switch (get_event(child)) {
			case __TEST_FISU:
				if (do_signal(child, __TEST_FISU))
					goto failure;
				continue;
			case __TEST_MSU:
				bsn[0] = fsn[0] = 0x7f;
				if (start_tt(config->sl.t5 * 10))
					goto failure;
				if (do_signal(child, __TEST_SIB))
					goto failure;
				beg_time = milliseconds(child, t6);
				state++;
				continue;
			default:
				goto failure;
			}
			break;
		case 1:
			switch (get_event(child)) {
			case __EVENT_TIMEOUT:
				if (do_signal(child, __TEST_SIB_S))
					goto failure;
				count++;
				if (count == n) {
					if (count > 1)
						if (do_signal(child, __TEST_COUNT))
							goto failure;
					goto failure;
				}
				if (start_tt(config->sl.t5 * 10))
					goto failure;
			case __TEST_FISU:
				bsn[0] = fsn[0] = 0x7f;
				if (do_signal(child, __TEST_FISU_S))
					goto failure;
				continue;
			case __TEST_SIOS:
				if (count > 1)
					if (do_signal(child, __TEST_COUNT))
						goto failure;
				if (check_time(child, "T6  ", beg_time, timer[t6].lo, timer[t6].hi))
					goto failure;
				break;
			default:
				goto failure;
			}
			break;
		default:
			goto scripterror;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
      scripterror:
	return __RESULT_SCRIPT_ERROR;
}
static int
test_10_3_iut(int child)
{
	if (do_signal(child, __TEST_SEND_MSU))
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __EVENT_IUT_OUT_OF_SERVICE))
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static struct test_stream test_case_10_3_ptu = { preamble_link_in_service, test_10_3_ptu, postamble_link_out_of_service };
static struct test_stream test_case_10_3_iut = { preamble_link_in_service, test_10_3_iut, postamble_link_out_of_service };

#define test_case_10_3 { &test_case_10_3_ptu, &test_case_10_3_iut, NULL }

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

	oldact = 0;		/* previous action for this child */
	cntact = 0;		/* count of this action for this child */
	oldevt = 0;		/* previous return for this child */
	cntevt = 0;		/* count of this return for this child */

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

	if (start_tt(test_duration) != __RESULT_SUCCESS)
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
	int run;			/* whether to run this test */
	int result;			/* results of test */
	int expect;			/* expected result */
} tests[] = {
	{
	numb_case_0_1, tgrp_case_0_1, sgrp_case_0_1, name_case_0_1, xtra_case_0_1, desc_case_0_1, sref_case_0_1, test_case_0_1, &begin_sanity, &end_sanity, 0, 0, __RESULT_INCONCLUSIVE,}, {
	numb_case_0_2_1, tgrp_case_0_2_1, sgrp_case_0_2_1, name_case_0_2_1, xtra_case_0_2_1, desc_case_0_2_1, sref_case_0_2_1, test_case_0_2_1, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_2, tgrp_case_0_2_2, sgrp_case_0_2_2, name_case_0_2_2, xtra_case_0_2_2, desc_case_0_2_2, sref_case_0_2_2, test_case_0_2_2, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_3, tgrp_case_0_2_3, sgrp_case_0_2_3, name_case_0_2_3, xtra_case_0_2_3, desc_case_0_2_3, sref_case_0_2_3, test_case_0_2_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_4, tgrp_case_0_2_4, sgrp_case_0_2_4, name_case_0_2_4, xtra_case_0_2_4, desc_case_0_2_4, sref_case_0_2_4, test_case_0_2_4, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_5, tgrp_case_0_2_5, sgrp_case_0_2_5, name_case_0_2_5, xtra_case_0_2_5, desc_case_0_2_5, sref_case_0_2_5, test_case_0_2_5, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_6, tgrp_case_0_2_6, sgrp_case_0_2_6, name_case_0_2_6, xtra_case_0_2_6, desc_case_0_2_6, sref_case_0_2_6, test_case_0_2_6, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_7, tgrp_case_0_2_7, sgrp_case_0_2_7, name_case_0_2_7, xtra_case_0_2_7, desc_case_0_2_7, sref_case_0_2_7, test_case_0_2_7, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_1a, tgrp_case_1_1a, sgrp_case_1_1a, name_case_1_1a, xtra_case_1_1a, desc_case_1_1a, sref_case_1_1a, test_case_1_1a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_1b, tgrp_case_1_1b, sgrp_case_1_1b, name_case_1_1b, xtra_case_1_1b, desc_case_1_1b, sref_case_1_1b, test_case_1_1b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_2, tgrp_case_1_2, sgrp_case_1_2, name_case_1_2, xtra_case_1_2, desc_case_1_2, sref_case_1_2, test_case_1_2, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_3, tgrp_case_1_3, sgrp_case_1_3, name_case_1_3, xtra_case_1_3, desc_case_1_3, sref_case_1_3, test_case_1_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_4, tgrp_case_1_4, sgrp_case_1_4, name_case_1_4, xtra_case_1_4, desc_case_1_4, sref_case_1_4, test_case_1_4, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_5a_p, tgrp_case_1_5a_p, sgrp_case_1_5a_p, name_case_1_5a_p, xtra_case_1_5a_p, desc_case_1_5a_p, sref_case_1_5a_p, test_case_1_5a_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_5b_p, tgrp_case_1_5b_p, sgrp_case_1_5b_p, name_case_1_5b_p, xtra_case_1_5b_p, desc_case_1_5b_p, sref_case_1_5b_p, test_case_1_5b_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_5a_np, tgrp_case_1_5a_np, sgrp_case_1_5a_np, name_case_1_5a_np, xtra_case_1_5a_np, desc_case_1_5a_np, sref_case_1_5a_np, test_case_1_5a_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_5b_np, tgrp_case_1_5b_np, sgrp_case_1_5b_np, name_case_1_5b_np, xtra_case_1_5b_np, desc_case_1_5b_np, sref_case_1_5b_np, test_case_1_5b_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_6_p, tgrp_case_1_6_p, sgrp_case_1_6_p, name_case_1_6_p, xtra_case_1_6_p, desc_case_1_6_p, sref_case_1_6_p, test_case_1_6_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_6_np, tgrp_case_1_6_np, sgrp_case_1_6_np, name_case_1_6_np, xtra_case_1_6_np, desc_case_1_6_np, sref_case_1_6_np, test_case_1_6_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_7, tgrp_case_1_7, sgrp_case_1_7, name_case_1_7, xtra_case_1_7, desc_case_1_7, sref_case_1_7, test_case_1_7, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_8a_p, tgrp_case_1_8a_p, sgrp_case_1_8a_p, name_case_1_8a_p, xtra_case_1_8a_p, desc_case_1_8a_p, sref_case_1_8a_p, test_case_1_8a_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_8b_p, tgrp_case_1_8b_p, sgrp_case_1_8b_p, name_case_1_8b_p, xtra_case_1_8b_p, desc_case_1_8b_p, sref_case_1_8b_p, test_case_1_8b_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_8a_np, tgrp_case_1_8a_np, sgrp_case_1_8a_np, name_case_1_8a_np, xtra_case_1_8a_np, desc_case_1_8a_np, sref_case_1_8a_np, test_case_1_8a_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_8b_np, tgrp_case_1_8b_np, sgrp_case_1_8b_np, name_case_1_8b_np, xtra_case_1_8b_np, desc_case_1_8b_np, sref_case_1_8b_np, test_case_1_8b_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_9a_p, tgrp_case_1_9a_p, sgrp_case_1_9a_p, name_case_1_9a_p, xtra_case_1_9a_p, desc_case_1_9a_p, sref_case_1_9a_p, test_case_1_9a_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_9b_p, tgrp_case_1_9b_p, sgrp_case_1_9b_p, name_case_1_9b_p, xtra_case_1_9b_p, desc_case_1_9b_p, sref_case_1_9b_p, test_case_1_9b_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_9a_np, tgrp_case_1_9a_np, sgrp_case_1_9a_np, name_case_1_9a_np, xtra_case_1_9a_np, desc_case_1_9a_np, sref_case_1_9a_np, test_case_1_9a_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_9b_np, tgrp_case_1_9b_np, sgrp_case_1_9b_np, name_case_1_9b_np, xtra_case_1_9b_np, desc_case_1_9b_np, sref_case_1_9b_np, test_case_1_9b_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_10_p, tgrp_case_1_10_p, sgrp_case_1_10_p, name_case_1_10_p, xtra_case_1_10_p, desc_case_1_10_p, sref_case_1_10_p, test_case_1_10_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_10_np, tgrp_case_1_10_np, sgrp_case_1_10_np, name_case_1_10_np, xtra_case_1_10_np, desc_case_1_10_np, sref_case_1_10_np, test_case_1_10_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_11_p, tgrp_case_1_11_p, sgrp_case_1_11_p, name_case_1_11_p, xtra_case_1_11_p, desc_case_1_11_p, sref_case_1_11_p, test_case_1_11_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_11_np, tgrp_case_1_11_np, sgrp_case_1_11_np, name_case_1_11_np, xtra_case_1_11_np, desc_case_1_11_np, sref_case_1_11_np, test_case_1_11_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_12a_p, tgrp_case_1_12a_p, sgrp_case_1_12a_p, name_case_1_12a_p, xtra_case_1_12a_p, desc_case_1_12a_p, sref_case_1_12a_p, test_case_1_12a_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_12b_p, tgrp_case_1_12b_p, sgrp_case_1_12b_p, name_case_1_12b_p, xtra_case_1_12b_p, desc_case_1_12b_p, sref_case_1_12b_p, test_case_1_12b_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_12a_np, tgrp_case_1_12a_np, sgrp_case_1_12a_np, name_case_1_12a_np, xtra_case_1_12a_np, desc_case_1_12a_np, sref_case_1_12a_np, test_case_1_12a_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_12b_np, tgrp_case_1_12b_np, sgrp_case_1_12b_np, name_case_1_12b_np, xtra_case_1_12b_np, desc_case_1_12b_np, sref_case_1_12b_np, test_case_1_12b_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_13_p, tgrp_case_1_13_p, sgrp_case_1_13_p, name_case_1_13_p, xtra_case_1_13_p, desc_case_1_13_p, sref_case_1_13_p, test_case_1_13_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_13_np, tgrp_case_1_13_np, sgrp_case_1_13_np, name_case_1_13_np, xtra_case_1_13_np, desc_case_1_13_np, sref_case_1_13_np, test_case_1_13_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_14, tgrp_case_1_14, sgrp_case_1_14, name_case_1_14, xtra_case_1_14, desc_case_1_14, sref_case_1_14, test_case_1_14, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_15_p, tgrp_case_1_15_p, sgrp_case_1_15_p, name_case_1_15_p, xtra_case_1_15_p, desc_case_1_15_p, sref_case_1_15_p, test_case_1_15_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_15_np, tgrp_case_1_15_np, sgrp_case_1_15_np, name_case_1_15_np, xtra_case_1_15_np, desc_case_1_15_np, sref_case_1_15_np, test_case_1_15_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_16_p, tgrp_case_1_16_p, sgrp_case_1_16_p, name_case_1_16_p, xtra_case_1_16_p, desc_case_1_16_p, sref_case_1_16_p, test_case_1_16_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_16_np, tgrp_case_1_16_np, sgrp_case_1_16_np, name_case_1_16_np, xtra_case_1_16_np, desc_case_1_16_np, sref_case_1_16_np, test_case_1_16_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_17, tgrp_case_1_17, sgrp_case_1_17, name_case_1_17, xtra_case_1_17, desc_case_1_17, sref_case_1_17, test_case_1_17, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_18, tgrp_case_1_18, sgrp_case_1_18, name_case_1_18, xtra_case_1_18, desc_case_1_18, sref_case_1_18, test_case_1_18, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_19, tgrp_case_1_19, sgrp_case_1_19, name_case_1_19, xtra_case_1_19, desc_case_1_19, sref_case_1_19, test_case_1_19, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_20, tgrp_case_1_20, sgrp_case_1_20, name_case_1_20, xtra_case_1_20, desc_case_1_20, sref_case_1_20, test_case_1_20, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_21, tgrp_case_1_21, sgrp_case_1_21, name_case_1_21, xtra_case_1_21, desc_case_1_21, sref_case_1_21, test_case_1_21, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_22, tgrp_case_1_22, sgrp_case_1_22, name_case_1_22, xtra_case_1_22, desc_case_1_22, sref_case_1_22, test_case_1_22, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_23, tgrp_case_1_23, sgrp_case_1_23, name_case_1_23, xtra_case_1_23, desc_case_1_23, sref_case_1_23, test_case_1_23, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_24, tgrp_case_1_24, sgrp_case_1_24, name_case_1_24, xtra_case_1_24, desc_case_1_24, sref_case_1_24, test_case_1_24, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_25, tgrp_case_1_25, sgrp_case_1_25, name_case_1_25, xtra_case_1_25, desc_case_1_25, sref_case_1_25, test_case_1_25, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_26, tgrp_case_1_26, sgrp_case_1_26, name_case_1_26, xtra_case_1_26, desc_case_1_26, sref_case_1_26, test_case_1_26, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_27_p, tgrp_case_1_27_p, sgrp_case_1_27_p, name_case_1_27_p, xtra_case_1_27_p, desc_case_1_27_p, sref_case_1_27_p, test_case_1_27_p, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_27_np, tgrp_case_1_27_np, sgrp_case_1_27_np, name_case_1_27_np, xtra_case_1_27_np, desc_case_1_27_np, sref_case_1_27_np, test_case_1_27_np, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_28, tgrp_case_1_28, sgrp_case_1_28, name_case_1_28, xtra_case_1_28, desc_case_1_28, sref_case_1_28, test_case_1_28, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_29a, tgrp_case_1_29a, sgrp_case_1_29a, name_case_1_29a, xtra_case_1_29a, desc_case_1_29a, sref_case_1_29a, test_case_1_29a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_29b, tgrp_case_1_29b, sgrp_case_1_29b, name_case_1_29b, xtra_case_1_29b, desc_case_1_29b, sref_case_1_29b, test_case_1_29b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_30a, tgrp_case_1_30a, sgrp_case_1_30a, name_case_1_30a, xtra_case_1_30a, desc_case_1_30a, sref_case_1_30a, test_case_1_30a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_30b, tgrp_case_1_30b, sgrp_case_1_30b, name_case_1_30b, xtra_case_1_30b, desc_case_1_30b, sref_case_1_30b, test_case_1_30b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_31a, tgrp_case_1_31a, sgrp_case_1_31a, name_case_1_31a, xtra_case_1_31a, desc_case_1_31a, sref_case_1_31a, test_case_1_31a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_31b, tgrp_case_1_31b, sgrp_case_1_31b, name_case_1_31b, xtra_case_1_31b, desc_case_1_31b, sref_case_1_31b, test_case_1_31b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_32a, tgrp_case_1_32a, sgrp_case_1_32a, name_case_1_32a, xtra_case_1_32a, desc_case_1_32a, sref_case_1_32a, test_case_1_32a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_32b, tgrp_case_1_32b, sgrp_case_1_32b, name_case_1_32b, xtra_case_1_32b, desc_case_1_32b, sref_case_1_32b, test_case_1_32b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_33, tgrp_case_1_33, sgrp_case_1_33, name_case_1_33, xtra_case_1_33, desc_case_1_33, sref_case_1_33, test_case_1_33, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_34, tgrp_case_1_34, sgrp_case_1_34, name_case_1_34, xtra_case_1_34, desc_case_1_34, sref_case_1_34, test_case_1_34, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_35, tgrp_case_1_35, sgrp_case_1_35, name_case_1_35, xtra_case_1_35, desc_case_1_35, sref_case_1_35, test_case_1_35, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_1, tgrp_case_2_1, sgrp_case_2_1, name_case_2_1, xtra_case_2_1, desc_case_2_1, sref_case_2_1, test_case_2_1, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_2, tgrp_case_2_2, sgrp_case_2_2, name_case_2_2, xtra_case_2_2, desc_case_2_2, sref_case_2_2, test_case_2_2, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_3, tgrp_case_2_3, sgrp_case_2_3, name_case_2_3, xtra_case_2_3, desc_case_2_3, sref_case_2_3, test_case_2_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_4, tgrp_case_2_4, sgrp_case_2_4, name_case_2_4, xtra_case_2_4, desc_case_2_4, sref_case_2_4, test_case_2_4, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_5, tgrp_case_2_5, sgrp_case_2_5, name_case_2_5, xtra_case_2_5, desc_case_2_5, sref_case_2_5, test_case_2_5, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_6, tgrp_case_2_6, sgrp_case_2_6, name_case_2_6, xtra_case_2_6, desc_case_2_6, sref_case_2_6, test_case_2_6, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_7, tgrp_case_2_7, sgrp_case_2_7, name_case_2_7, xtra_case_2_7, desc_case_2_7, sref_case_2_7, test_case_2_7, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_8, tgrp_case_2_8, sgrp_case_2_8, name_case_2_8, xtra_case_2_8, desc_case_2_8, sref_case_2_8, test_case_2_8, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_1, tgrp_case_3_1, sgrp_case_3_1, name_case_3_1, xtra_case_3_1, desc_case_3_1, sref_case_3_1, test_case_3_1, &begin_tests, &end_tests, 0, __RESULT_INCONCLUSIVE, __RESULT_INCONCLUSIVE,}, {
	numb_case_3_2, tgrp_case_3_2, sgrp_case_3_2, name_case_3_2, xtra_case_3_2, desc_case_3_2, sref_case_3_2, test_case_3_2, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_3, tgrp_case_3_3, sgrp_case_3_3, name_case_3_3, xtra_case_3_3, desc_case_3_3, sref_case_3_3, test_case_3_3, &begin_tests, &end_tests, 0, __RESULT_INCONCLUSIVE, __RESULT_INCONCLUSIVE,}, {
	numb_case_3_4, tgrp_case_3_4, sgrp_case_3_4, name_case_3_4, xtra_case_3_4, desc_case_3_4, sref_case_3_4, test_case_3_4, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_5, tgrp_case_3_5, sgrp_case_3_5, name_case_3_5, xtra_case_3_5, desc_case_3_5, sref_case_3_5, test_case_3_5, &begin_tests, &end_tests, 0, __RESULT_INCONCLUSIVE, __RESULT_INCONCLUSIVE,}, {
	numb_case_3_6, tgrp_case_3_6, sgrp_case_3_6, name_case_3_6, xtra_case_3_6, desc_case_3_6, sref_case_3_6, test_case_3_6, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_7, tgrp_case_3_7, sgrp_case_3_7, name_case_3_7, xtra_case_3_7, desc_case_3_7, sref_case_3_7, test_case_3_7, &begin_tests, &end_tests, 0, __RESULT_INCONCLUSIVE, __RESULT_INCONCLUSIVE,}, {
	numb_case_3_8, tgrp_case_3_8, sgrp_case_3_8, name_case_3_8, xtra_case_3_8, desc_case_3_8, sref_case_3_8, test_case_3_8, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_1a, tgrp_case_4_1a, sgrp_case_4_1a, name_case_4_1a, xtra_case_4_1a, desc_case_4_1a, sref_case_4_1a, test_case_4_1a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_1b, tgrp_case_4_1b, sgrp_case_4_1b, name_case_4_1b, xtra_case_4_1b, desc_case_4_1b, sref_case_4_1b, test_case_4_1b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_2, tgrp_case_4_2, sgrp_case_4_2, name_case_4_2, xtra_case_4_2, desc_case_4_2, sref_case_4_2, test_case_4_2, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_3, tgrp_case_4_3, sgrp_case_4_3, name_case_4_3, xtra_case_4_3, desc_case_4_3, sref_case_4_3, test_case_4_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_5_1, tgrp_case_5_1, sgrp_case_5_1, name_case_5_1, xtra_case_5_1, desc_case_5_1, sref_case_5_1, test_case_5_1, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_5_2, tgrp_case_5_2, sgrp_case_5_2, name_case_5_2, xtra_case_5_2, desc_case_5_2, sref_case_5_2, test_case_5_2, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_5_3, tgrp_case_5_3, sgrp_case_5_3, name_case_5_3, xtra_case_5_3, desc_case_5_3, sref_case_5_3, test_case_5_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_5_4a, tgrp_case_5_4a, sgrp_case_5_4a, name_case_5_4a, xtra_case_5_4a, desc_case_5_4a, sref_case_5_4a, test_case_5_4a, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_5_4b, tgrp_case_5_4b, sgrp_case_5_4b, name_case_5_4b, xtra_case_5_4b, desc_case_5_4b, sref_case_5_4b, test_case_5_4b, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_5_5a, tgrp_case_5_5a, sgrp_case_5_5a, name_case_5_5a, xtra_case_5_5a, desc_case_5_5a, sref_case_5_5a, test_case_5_5a, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_5_5b, tgrp_case_5_5b, sgrp_case_5_5b, name_case_5_5b, xtra_case_5_5b, desc_case_5_5b, sref_case_5_5b, test_case_5_5b, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_6_1, tgrp_case_6_1, sgrp_case_6_1, name_case_6_1, xtra_case_6_1, desc_case_6_1, sref_case_6_1, test_case_6_1, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_6_2, tgrp_case_6_2, sgrp_case_6_2, name_case_6_2, xtra_case_6_2, desc_case_6_2, sref_case_6_2, test_case_6_2, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_6_3, tgrp_case_6_3, sgrp_case_6_3, name_case_6_3, xtra_case_6_3, desc_case_6_3, sref_case_6_3, test_case_6_3, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_6_4, tgrp_case_6_4, sgrp_case_6_4, name_case_6_4, xtra_case_6_4, desc_case_6_4, sref_case_6_4, test_case_6_4, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_7_1, tgrp_case_7_1, sgrp_case_7_1, name_case_7_1, xtra_case_7_1, desc_case_7_1, sref_case_7_1, test_case_7_1, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_7_2, tgrp_case_7_2, sgrp_case_7_2, name_case_7_2, xtra_case_7_2, desc_case_7_2, sref_case_7_2, test_case_7_2, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_7_3, tgrp_case_7_3, sgrp_case_7_3, name_case_7_3, xtra_case_7_3, desc_case_7_3, sref_case_7_3, test_case_7_3, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_7_4, tgrp_case_7_4, sgrp_case_7_4, name_case_7_4, xtra_case_7_4, desc_case_7_4, sref_case_7_4, test_case_7_4, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_8_1, tgrp_case_8_1, sgrp_case_8_1, name_case_8_1, xtra_case_8_1, desc_case_8_1, sref_case_8_1, test_case_8_1, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_2, tgrp_case_8_2, sgrp_case_8_2, name_case_8_2, xtra_case_8_2, desc_case_8_2, sref_case_8_2, test_case_8_2, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_8_3, tgrp_case_8_3, sgrp_case_8_3, name_case_8_3, xtra_case_8_3, desc_case_8_3, sref_case_8_3, test_case_8_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_4, tgrp_case_8_4, sgrp_case_8_4, name_case_8_4, xtra_case_8_4, desc_case_8_4, sref_case_8_4, test_case_8_4, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_5, tgrp_case_8_5, sgrp_case_8_5, name_case_8_5, xtra_case_8_5, desc_case_8_5, sref_case_8_5, test_case_8_5, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_6, tgrp_case_8_6, sgrp_case_8_6, name_case_8_6, xtra_case_8_6, desc_case_8_6, sref_case_8_6, test_case_8_6, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_8_7, tgrp_case_8_7, sgrp_case_8_7, name_case_8_7, xtra_case_8_7, desc_case_8_7, sref_case_8_7, test_case_8_7, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_8_8, tgrp_case_8_8, sgrp_case_8_8, name_case_8_8, xtra_case_8_8, desc_case_8_8, sref_case_8_8, test_case_8_8, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_8_9a, tgrp_case_8_9a, sgrp_case_8_9a, name_case_8_9a, xtra_case_8_9a, desc_case_8_9a, sref_case_8_9a, test_case_8_9a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_9b, tgrp_case_8_9b, sgrp_case_8_9b, name_case_8_9b, xtra_case_8_9b, desc_case_8_9b, sref_case_8_9b, test_case_8_9b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_10, tgrp_case_8_10, sgrp_case_8_10, name_case_8_10, xtra_case_8_10, desc_case_8_10, sref_case_8_10, test_case_8_10, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_11, tgrp_case_8_11, sgrp_case_8_11, name_case_8_11, xtra_case_8_11, desc_case_8_11, sref_case_8_11, test_case_8_11, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_12a, tgrp_case_8_12a, sgrp_case_8_12a, name_case_8_12a, xtra_case_8_12a, desc_case_8_12a, sref_case_8_12a, test_case_8_12a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_12b, tgrp_case_8_12b, sgrp_case_8_12b, name_case_8_12b, xtra_case_8_12b, desc_case_8_12b, sref_case_8_12b, test_case_8_12b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_13, tgrp_case_8_13, sgrp_case_8_13, name_case_8_13, xtra_case_8_13, desc_case_8_13, sref_case_8_13, test_case_8_13, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_8_14, tgrp_case_8_14, sgrp_case_8_14, name_case_8_14, xtra_case_8_14, desc_case_8_14, sref_case_8_14, test_case_8_14, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_9_1, tgrp_case_9_1, sgrp_case_9_1, name_case_9_1, xtra_case_9_1, desc_case_9_1, sref_case_9_1, test_case_9_1, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_2, tgrp_case_9_2, sgrp_case_9_2, name_case_9_2, xtra_case_9_2, desc_case_9_2, sref_case_9_2, test_case_9_2, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_3, tgrp_case_9_3, sgrp_case_9_3, name_case_9_3, xtra_case_9_3, desc_case_9_3, sref_case_9_3, test_case_9_3, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_4, tgrp_case_9_4, sgrp_case_9_4, name_case_9_4, xtra_case_9_4, desc_case_9_4, sref_case_9_4, test_case_9_4, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_5, tgrp_case_9_5, sgrp_case_9_5, name_case_9_5, xtra_case_9_5, desc_case_9_5, sref_case_9_5, test_case_9_5, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_6, tgrp_case_9_6, sgrp_case_9_6, name_case_9_6, xtra_case_9_6, desc_case_9_6, sref_case_9_6, test_case_9_6, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_7, tgrp_case_9_7, sgrp_case_9_7, name_case_9_7, xtra_case_9_7, desc_case_9_7, sref_case_9_7, test_case_9_7, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_8, tgrp_case_9_8, sgrp_case_9_8, name_case_9_8, xtra_case_9_8, desc_case_9_8, sref_case_9_8, test_case_9_8, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_9, tgrp_case_9_9, sgrp_case_9_9, name_case_9_9, xtra_case_9_9, desc_case_9_9, sref_case_9_9, test_case_9_9, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_10, tgrp_case_9_10, sgrp_case_9_10, name_case_9_10, xtra_case_9_10, desc_case_9_10, sref_case_9_10, test_case_9_10, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_11, tgrp_case_9_11, sgrp_case_9_11, name_case_9_11, xtra_case_9_11, desc_case_9_11, sref_case_9_11, test_case_9_11, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_12, tgrp_case_9_12, sgrp_case_9_12, name_case_9_12, xtra_case_9_12, desc_case_9_12, sref_case_9_12, test_case_9_12, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_9_13, tgrp_case_9_13, sgrp_case_9_13, name_case_9_13, xtra_case_9_13, desc_case_9_13, sref_case_9_13, test_case_9_13, &begin_tests, &end_tests, 0, __RESULT_NOTAPPL, __RESULT_NOTAPPL,}, {
	numb_case_10_1, tgrp_case_10_1, sgrp_case_10_1, name_case_10_1, xtra_case_10_1, desc_case_10_1, sref_case_10_1, test_case_10_1, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_10_2a, tgrp_case_10_2a, sgrp_case_10_2a, name_case_10_2a, xtra_case_10_2a, desc_case_10_2a, sref_case_10_2a, test_case_10_2a, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_10_2b, tgrp_case_10_2b, sgrp_case_10_2b, name_case_10_2b, xtra_case_10_2b, desc_case_10_2b, sref_case_10_2b, test_case_10_2b, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_10_2c, tgrp_case_10_2c, sgrp_case_10_2c, name_case_10_2c, xtra_case_10_2c, desc_case_10_2c, sref_case_10_2c, test_case_10_2c, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_10_3, tgrp_case_10_3, sgrp_case_10_3, name_case_10_3, xtra_case_10_3, desc_case_10_3, sref_case_10_3, test_case_10_3, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS,}, {
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
				if ((result = (*tests[i].start) (i)) != __RESULT_SUCCESS)
					goto inconclusive;
				result = test_run(tests[i].stream);
				(*tests[i].stop) (i);
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
    -u, --iut\n\
        IUT connects instead of PT.\n\
    -D, --decl-std [STANDARD]\n\
        specify the SS7 standard version to test.\n\
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
Symbols:\n\
    [STANDARD]\n\
        SS7_PVAR_ITUT_83  SS7_PVAR_ETSI_88  SS7_PVAR_ANSI_92  SS7_PVAR_SING_88\n\
        SS7_PVAR_ITUT_93  SS7_PVAR_ETSI_93  SS7_PVAR_JTTC_94  SS7_PVAR_SPAN_88\n\
        SS7_PVAR_ITUT_96  SS7_PVAR_ETSI_96  SS7_PVAR_ANSI_96\n\
        SS7_PVAR_ITUT_00  SS7_PVAR_ETSI_00  SS7_PVAR_ANSI_00  SS7_PVAR_CHIN_00\n\
\n\
", argv[0], devname);
}

#define HOST_BUF_LEN 128

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
			{"iut",		no_argument,		NULL, 'u'},
			{"decl-std",	required_argument,	NULL, 'D'},
			{"client",	no_argument,		NULL, 'c'},
			{"server",	no_argument,		NULL, 'S'},
			{"again",	no_argument,		NULL, 'a'},
			{"wait",	no_argument,		NULL, 'w'},
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
		c = getopt_long(argc, argv, "uD:cSawrRd:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "uD:cSawrRd:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'u':	/* --iut */
			iut_connects = 1;
			break;
		case 'D':	/* --decl-std */
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
#if 0
	if (client_ppa_specified) {
	}
	if (server_ppa_specified) {
	}
#endif
	exit(do_tests(tests_to_run));
}
