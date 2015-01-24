/*****************************************************************************

 @(#) File: src/test/test-sl-mux.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Affero General Public License (AGPL) Version 3, so long as the software is
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

 *****************************************************************************/

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/*
 * This is a test case executable program for testing the signalling link multiplexing driver.  The
 * signalling link multiplexing driver is responsible for coordinating all of the signalling links
 * in the entire system.  This includes signalling links that are provided by real TDM links in the
 * system as well as M2PA and M2UA SIGTRAN links and virtual signalling links provided using the
 * sl-pmod module.  The signalling link multiplexer is tested primarily using the sl-pmod signalling
 * link simulator.  With the sl-pmod signalling link simulator, a STREAMS-based pipe is opened and
 * the sl-pmod module is pushed over one end of the pipe.  Both ends of the pipe then represent
 * either end of a signalling link and provide the SLI (Signalling Link Interface).  In addition,
 * the SL-PMOD input-output controls are implemented to be able to simulate signalling link events
 * such as excessive bit errors, etc.  A modified Q.781 test suite is executed to ensure that the
 * state machine seen through the SL-MUX is the same as that seen for the SL-PMOD module itself.
 *
 * To perform the tests, the SL-MUX is configured at the beginning of the test sequence by adding
 * the ends of a SL-PMOD pipe to the multiplexer.  Then two Streams are opened on the SL-MUX and
 * attached to the available test signalling link ends.  The test case is performed on the
 * signalling link and then the Streams are closed.  Once the test cases have completed, the SL-PMOD
 * test signalling link is removed from the multiplexing driver.
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

#include <sys/testmod.h>

#include <linux/limits.h>

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
 * The following test configuration is used to provide a loopback signalling link:
 *           _____                       _____          
 *           \   /    |                  \   /    |     
 *            \ /    / \                  \ /    / \    
 *          ___|____/___\__________________|____/___\__
 *         |                                           |
 *         |                                           |
 *         |                 SL-MUX                    |
 *         |                                           |
 *         |                                           |
 *         |___________________________________________|
 *           \   /    |                  \   /    |     
 *            \ /    / \                  \ /     |     
 *          ___|___ /___\_                 |      |     
 *         |              |                |      |     
 *         |    SL-PMOD   |                |      |     
 *         |______________|                |      |     
 *           \   /    |                    |      |     
 *            \ /    / \                   |     / \    
 *          ___|____/___\__________________|____/___\__
 *         |                                           |
 *         |                                           |
 *         |                   PIPE                    |
 *         |                                           |
 *         |___________________________________________|
 *
 * The signalling link multiplexer allows signalling links to be linked beneath the multiplexing
 * driver and accessed using either a global PPA or a CLEI.  We use a particlar global PPA and CLEI
 * for testing purposes.  Note that this test can be performed on the multiplexing driver while it
 * is being used for other purposes.
 *
 * Another feature provided by the signalling link multiplexing driver is the ability to monitor one
 * or more signalling links.  The monitoring Stream provided by the driver can be used to generate
 * PCAP formatted capture files that can be examined using wireshark or ethereal.  The test case
 * executables can be requested to generate these PCAP files, one for each test case, as auxillary
 * output.  A test suite script executable can then use twireshark or tethereal to generate a text
 * output of the test suite executable at the end of test case execution.
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "SS7 MTP Level 2 Multiplexer";

/* static const char *spkgname = "strss7"; */
static const char *lstdname = "Q.703/ANSI T1.111.3/JQ.703";
static const char *sstdname = "Q.781";
static const char *shortname = "MTP2";

static char devname[256] = "/dev/streams/clone/sl-mux";

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

static int verbose = 1;

static int client_exec = 0;		/* execute client side */
static int server_exec = 0;		/* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_fisus = 1;

static int show_msus = 1;

static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;

int test_fd[3] = { 0, 0, 0 };

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

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, -1, cbuf };
struct strbuf data = { BUFSIZE, -1, dbuf };

static int test_pflags = MSG_BAND;	/* MSG_BAND | MSG_HIPRI */
static int test_pband = 0;
static int test_gflags = 0;		/* MSG_BAND | MSG_HIPRI */
static int test_gband = 0;
static int test_timout = 200;

struct strfdinsert fdi = {
	{BUFSIZE, 0, cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};
int flags = 0;

int dummy = 0;

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
} iutstat = {};



