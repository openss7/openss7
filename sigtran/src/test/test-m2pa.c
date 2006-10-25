/*****************************************************************************

 @(#) $RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:52 $

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

 Last Modified $Date: 2006/10/17 11:55:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-m2pa.c,v $
 Revision 0.9.2.1  2006/10/17 11:55:52  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.15  2006/03/03 12:06:12  brian
 - 32/64-bit compatibility

 Revision 0.9.2.14  2005/12/29 21:36:09  brian
 - a few idiosynchrasies for PPC, old 2.95 compiler, and 2.6.14 builds

 Revision 0.9.2.13  2005/12/28 09:58:32  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.12  2005/06/23 22:09:55  brian
 - changes to pass _FORTIFY_SOURCE=2 on gcc 4 testing on FC4

 Revision 0.9.2.11  2005/06/22 07:58:43  brian
 - unsigned/signed pointer corrections for gcc4 on FC4

 Revision 0.9.2.10  2005/05/14 08:31:31  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:52 $"

static char const ident[] = "$RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:52 $";

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

#include <features.h>
#include <limits.h>
#include <stdint.h>
#include <endian.h>
#include <netinet/in.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

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
static const char *lstdname = "draft-bidulock-sigtran-m2pa-test";
static const char *sstdname = "Q.781";
static const char *shortname = "M2PA";
static char devname[256] = "/dev/sctp_n";

static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

static int client_port_specified = 0;
static int server_port_specified = 0;
static int client_host_specified = 0;
static int server_host_specified = 0;

static int verbose = 1;

static int client_exec = 0; /* execute client side */
static int server_exec = 0; /* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
//static int show_data = 1;

static int last_event = 0;
static int last_retval = 0;
static int PRIM_type = 0;
static int last_errno = 0;
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
uint32_t bsn[3] = { 0, 0, 0 };
uint32_t fsn[3] = { 0, 0, 0 };

#define BUFSIZE 32*4096

#define FFLUSH(stream)
// #define FFLUSH(stream) fflush((_stream))

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	20000
#define INFINITE_WAIT	-1

static int test_duration = TEST_DURATION; /* wait on other side */

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
	__TEST_OUT_OF_SERVICE = 200, __TEST_ALIGNMENT, __TEST_PROVING_NORMAL, __TEST_PROVING_EMER,
	__TEST_IN_SERVICE, __TEST_PROCESSOR_OUTAGE, __TEST_PROCESSOR_ENDED, __TEST_BUSY,
	__TEST_BUSY_ENDED, __TEST_INVALID_STATUS, __TEST_SEQUENCE_SYNC, __TEST_DATA, __TEST_PROVING,
	__TEST_ACK, __TEST_TX_BREAK, __TEST_TX_MAKE, __TEST_BAD_ACK, __TEST_MSU_TOO_SHORT,
	__TEST_FISU, __TEST_FISU_S, __TEST_FISU_CORRUPT, __TEST_FISU_CORRUPT_S,
	__TEST_MSU_SEVEN_ONES, __TEST_MSU_TOO_LONG, __TEST_FISU_FISU_1FLAG, __TEST_FISU_FISU_2FLAG,
	__TEST_MSU_MSU_1FLAG, __TEST_MSU_MSU_2FLAG, __TEST_COUNT, __TEST_TRIES,
	,__TEST_ETC, __TEST_SIB_S, __TEST_IUT_IN_SERVICE, __TEST_IUT_OUT_OF_SERVICE, __TEST_IUT_RPO,
	__TEST_IUT_RPR, __TEST_IUT_DATA, __TEST_UNKNOWN,
};

enum {
	__TEST_POWER_ON = 300, __TEST_START, __TEST_STOP, __TEST_LPO, __TEST_LPR, __TEST_EMERG,
	__TEST_CEASE, __TEST_SEND_MSU, __TEST_SEND_MSU_S, __TEST_CONG_A, __TEST_CONG_D,
	__TEST_NO_CONG, __TEST_CLEARB, __TEST_SYNC,
};

enum {
	__TEST_ENABLE_REQ = 400, __TEST_ENABLE_CON, __TEST_DISABLE_REQ, __TEST_DISABLE_CON,
	__TEST_ERROR_IND,
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

// #define M2PA_VERSION_DRAFT3
// #define M2PA_VERSION_DRAFT3_1
// #define M2PA_VERSION_DRAFT4
// #define M2PA_VERSION_DRAFT4_1
// #define M2PA_VERSION_DRAFT4_9
// #define M2PA_VERSION_DRAFT5
// #define M2PA_VERSION_DRAFT5_1
// #define M2PA_VERSION_DRAFT6
// #define M2PA_VERSION_DRAFT6_1
// #define M2PA_VERSION_DRAFT6_9
// #define M2PA_VERSION_DRAFT7
// #define M2PA_VERSION_DRAFT10
#define M2PA_VERSION_DRAFT11

typedef struct addr {
	uint16_t port __attribute__ ((packed));
	uint32_t addr[3] __attribute__ ((packed));
} addr_t;

static struct {
	addr_t loc;
	addr_t rem;
	N_qos_sel_info_sctp_t qos;
	N_qos_sel_data_sctp_t qos_data;
} ptconf = {
	{
		__constant_htons(10001), {
			__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003)
		}
	},			/* loc */
	{
		__constant_htons(10000), {
			__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003)
		}
	},			/* rem */
	{
		N_QOS_SEL_INFO_SCTP,	/* n_qos_type */
		    2,		/* i_streams */
		    2,		/* o_streams */
		    5,		/* ppi */
		    0,		/* sid */
		    -1L,	/* max_in */
		    -1L,	/* max_retran */
		    -1L,	/* ck_life */
		    -1L,	/* ck_inc */
		    -1L,	/* hmac */
		    -1L,	/* thrott */
		    -1L,	/* max_sack */
		    -1L,	/* rto_ini */
		    -1L,	/* rto_min */
		    -1L,	/* rto_max */
		    -1L,	/* rtx_path */
		    -1L,	/* hb_itvl */
		    0		/* options */
	}, {
		N_QOS_SEL_DATA_SCTP,	/* n_qos_type */
		    5,		/* ppi */
		    1,		/* sid */
		    0,		/* ssn */
		    0,		/* tsn */
		    0		/* more */
	}
};

#ifndef HZ
#define HZ 100
#endif

static struct {
	addr_t loc;
	addr_t rem;
	N_qos_sel_info_sctp_t qos;
	lmi_option_t opt;
	sdl_config_t sdl;
	sdt_config_t sdt;
	sl_config_t sl;
} iutconf = {
	{
		__constant_htons(10000), {
		__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003),}
	},			/* loc */
	{
		__constant_htons(10001), {
		__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003),}
	},			/* rem */
	{
		N_QOS_SEL_INFO_SCTP,	/* n_qos_type */
		    2,		/* i_streams */
		    2,		/* o_streams */
		    5,		/* ppi */
		    0,		/* sid */
		    -1L,	/* max_in */
		    -1L,	/* max_retran */
		    -1L,	/* ck_life */
		    -1L,	/* ck_inc */
		    -1L,	/* hmac */
		    -1L,	/* thrott */
		    -1L,	/* max_sack */
		    -1L,	/* rto_ini */
		    -1L,	/* rto_min */
		    -1L,	/* rto_max */
		    -1L,	/* rtx_path */
		    -1L,	/* hb_itvl */
		    0,		/* options */
	},			/* qos */
	{
		SS7_PVAR_ITUT_96,	/* pvar */
		    0,		/* popt */
	},			/* opt */
	{
      ifname: NULL, ifflags: 0, iftype: SDL_TYPE_PACKET, ifrate: 10000000, ifgtype: SDL_GTYPE_SCTP, ifgrate: 10000000, ifmode: SDL_MODE_PEER, ifgmode: SDL_GMODE_NONE, ifgcrc: SDL_GCRC_NONE, ifclock: SDL_CLOCK_NONE, ifcoding: SDL_CODING_NONE, ifframing: SDL_FRAMING_NONE, ifblksize: 0, ifleads: 0, ifbpv: 0, ifalarms: 0, ifrxlevel: 0, iftxlevel: 0, ifsync:0,},
	    /*
	       sdl 
	     */
	{
	      t8:100 * HZ / 1000,	/* t8 - T8 timeout */
	      Tin: 4,		/* Tin - AERM normal proving threshold */
	      Tie: 1,		/* Tie - AERM emergency proving threshold */
	      T:   64,		/* T - SUERM error threshold */
	      D:   256,	/* D - SUERM error rate parameter */
	      Te:  577169,	/* Te - EIM error threshold */
	      De:  9308000,	/* De - EIM correct decrement */
	      Ue:  144292000,	/* Ue - EIM error increment */
	      N:   16,		/* N */
	      m:   272,	/* m */
	      b:   64,		/* b */
	      f:   1,		/* f */
	},			/* sdt */
	{
	      t1:45 * HZ,	/* t1 - timer t1 duration (ticks) */
	      t2:  5 * HZ,	/* t2 - timer t2 duration (ticks) */
	      t2l: 20 * HZ,	/* t2l - timer t2l duration (ticks) */
	      t2h: 100 * HZ,	/* t2h - timer t2h duration (ticks) */
	      t3:  1 * HZ,	/* t3 - timer t3 duration (ticks) */
	      t4n: 8 * HZ,	/* t4n - timer t4n duration (ticks) */
	      t4e: 500 * HZ / 1000,	/* t4e - timer t4e duration (ticks) */
	      t5:  100 * HZ / 1000,	/* t5 - timer t5 duration (ticks) */
	      t6:  4 * HZ,	/* t6 - timer t6 duration (ticks) */
	      t7:  2 * HZ,	/* t7 - timer t7 duration (ticks) */
	      rb_abate:3,	/* rb_abate - RB cong abatement (#msgs) */
	      rb_accept:6,	/* rb_accept - RB cong onset accept (#msgs) */
	      rb_discard:9,	/* rb_discard - RB cong discard (#msgs) */
	      tb_abate_1:128 * 272, /* tb_abate_1 - lev 1 cong abate (#bytes) */
	      tb_onset_1:256 * 272, /* tb_onset_1 - lev 1 cong onset (#bytes) */
	      tb_discd_1:384 * 272, /* tb_discd_1 - lev 1 cong discard (#bytes) */
	      tb_abate_2:512 * 272, /* tb_abate_2 - lev 1 cong abate (#bytes) */
	      tb_onset_2:640 * 272, /* tb_onset_2 - lev 1 cong onset (#bytes) */
	      tb_discd_2:768 * 272, /* tb_discd_2 - lev 1 cong discard (#bytes) */
	      tb_abate_3:896 * 272, /* tb_abate_3 - lev 1 cong abate (#bytes) */
	      tb_onset_3:1024 * 272, /* tb_onset_3 - lev 1 cong onset (#bytes) */
	      tb_discd_3:1152 * 272, /* tb_discd_3 - lev 1 cong discard (#bytes) */
	      N1:  31,		/* N1 - PCR/RTBmax messages (#msg) */
	      N2:  8192,	/* N2 - PCR/RTBmax octets (#bytes) */
	      M:   5		/* M - IAC normal proving periods */
} /* sl */ };

static struct {
	addr_t loc;
	addr_t rem;
	N_qos_sel_info_sctp_t qos;
} mgmconf = {
	{
		__constant_htons(10001), {
			__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003)
		}
	},			/* loc */
	{
		__constant_htons(10000), {
			__constant_htonl(0x7f000001), __constant_htonl(0x7f000002), __constant_htonl(0x7f000003)
		}
	},			/* rem */
	{
		N_QOS_SEL_INFO_SCTP,	/* n_qos_type */
		    2,		/* i_streams */
		    2,		/* o_streams */
		    5,		/* ppi */
		    0,		/* sid */
		    -1L,	/* max_in */
		    -1L,	/* max_retran */
		    -1L,	/* ck_life */
		    -1L,	/* ck_inc */
		    -1L,	/* hmac */
		    -1L,	/* thrott */
		    -1L,	/* max_sack */
		    -1L,	/* rto_ini */
		    -1L,	/* rto_min */
		    -1L,	/* rto_max */
		    -1L,	/* rtx_path */
		    -1L,	/* hb_itvl */
		    0		/* options */
	}			/* qos */
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

#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

/*
 *  Return the current time in milliseconds.
 */
static long
dual_milliseconds(int t1, int t2)
{
	long ret;
	struct timeval now;
	printf("                               !                                    \n");
	printf("                               !  %-6.6s     %ld.%03ld <= %-2.2s <= %ld.%03ld   \n", timer[t1].name, timer[t1].lo / 1000, timer[t1].lo - ((timer[t1].lo / 1000) * 1000), timer[t1].name, timer[t1].hi / 1000, timer[t1].hi - ((timer[t1].hi / 1000) * 1000));
	printf("                               !   +                                \n");
	printf("                               !  %-6.6s     %ld.%03ld <= %-2.2s <= %ld.%03ld   \n", timer[t2].name, timer[t2].lo / 1000, timer[t2].lo - ((timer[t2].lo / 1000) * 1000), timer[t2].name, timer[t2].hi / 1000, timer[t2].hi - ((timer[t2].hi / 1000) * 1000));
	printf("                               !                                    \n");
	FFLUSH(stdout);
	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500L) / 1000;
	return ret;
}

/*
 *  Return the current time in milliseconds.
 */
static long
milliseconds(int t)
{
	long ret;
	struct timeval now;
	printf("                               !                                    \n");
	printf("                               !  %-6.6s     %ld.%03ld <= %-2.2s <= %ld.%03ld   \n", timer[t].name, timer[t].lo / 1000, timer[t].lo - ((timer[t].lo / 1000) * 1000), timer[t].name, timer[t].hi / 1000, timer[t].hi - ((timer[t].hi / 1000) * 1000));
	printf("                               !                                    \n");
	FFLUSH(stdout);
	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500L) / 1000;
	return ret;
}

/*
 *  Check the current time against the beginning time provided as an argnument
 *  and see if the time inverval falls between the low and high values for the
 *  timer as specified by arguments.  Return SUCCESS if the interval is within
 *  the allowable range and FAILURE otherwise.
 */
static int
check_time(const char *t, long beg, long lo, long hi)
{
	long i;
	struct timeval now;
	if (gettimeofday(&now, NULL)) {
		printf("****ERROR: gettimeofday\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	i = (now.tv_sec - test_start) * 1000;
	i += (now.tv_usec + 500L) / 1000;
	i -= beg;
	printf("                           check timeout     %s [%ld.%03ld <= %ld.%03ld <= %ld.%03ld]\n", t, (lo - 100) / 1000, (lo - 100) - (((lo - 100) / 1000) * 1000), i / 1000, i - ((i / 1000) * 1000), (hi + 100) / 1000, (hi + 100) - (((hi + 100) / 1000) * 1000)
	    );
	FFLUSH(stdout);
	if (lo - 100 <= i && i <= hi + 100)
		return __RESULT_SUCCESS;
	else
		return __RESULT_FAILURE;
}

static int
time_event(int child, int event)
{
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
		fprintf(stdout, "                    | %11.6g                    |  |                    <%d:%03d>\n", t, child, state);
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
//	act.sa_flags = SA_RESTART | SA_ONESHOT;
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

	if (duration == (long)INFINITE_WAIT)
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
print_open(int child, const char* name)
{
	static const char *msgs[] = {
		"  open()      ----->v %-30s    .                   \n",
		"                    | %-30s    v<-----     open()  \n",
		"                    | %-30s v<-+------     open()  \n",
		"                    . %-30s .  .                   \n",
	};

	if (verbose > 3)
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

	if ((verbose && show) || (verbose > 5 && show_msg))
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

	if (show && verbose > 0)
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

	if (show && verbose > 0)
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
		"????%4ld????  ?----?|?- - - - - - -?                |  |                     [%d:%03d]\n",
		"                    |                               |  |?--? ????%4ld????    [%d:%03d]\n",
		"                    |                               |?-+---? ????%4ld????    [%d:%03d]\n",
		"                    | ? - - - - - - %4ld  - - - - ? |  |                     [%d:%03d]\n",
	};

	if (verbose > 0)
		print_long_state(child, msgs, prim);
}

void
print_signal(int child, int signum)
{
	static const char *msgs[] = {
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"  >>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"    >>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, signal_string(signum));
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"  %-14s--->|                                |  |                    [%d:%03d]\n",
		"    %-14s->|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_command(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"  %-14s--->|                                |  |                    [%d:%03d]\n",
		"    %-14s->|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, command);
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
		"%-14s----->|       %16s         |  |                    [%d:%03d]\n",
		"  %-14s--->|       %16s         |  |                    [%d:%03d]\n",
		"    %-14s->|       %16s         |  |                    [%d:%03d]\n",
		"                    | %-14s %16s|  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
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
print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - |- |                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|  |--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - |->|--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
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

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
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

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
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
		"%10d<--------/|%-32s|  |                    [%d:%03d]\n",
		"  %10d<------/|%-32s|  |                    [%d:%03d]\n",
		"    %10d<----/|%-32s|  |                    [%d:%03d]\n",
		"          %10d|%-32s|  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_int_string_state(child, msgs, value, poll_events_string(revents));
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
		"  %1$16s->|- -%2$5d bytes- - - - - - - - ->|->|                    [%3$d:%4$03d]\n",
		"                    |< -%2$5d bytes- - - - - - - - - |- |<-%1$16s  [%3$d:%4$03d]\n",
		"                    |< -%2$5d bytes- - - - - - - - - |<-+--%1$16s  [%3$d:%4$03d]\n",
		"                    |- -%2$5d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
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
				print_errno(child, (last_errno = errno));
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
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
				print_errno(child, (last_errno = errno));
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
				return (__RESULT_FAILURE);
			}
			if ((verbose > 3 && show) || (verbose > 5 && show_msg))
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	}
}

int
test_m2pa_status(int child, uint32_t status) {
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	union primitives *p = (typeof(p)) cbuf;

	switch (m2pa_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		data->len = 3*sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = status;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		data->len = 4*sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint16_t *) dbuf)[4] = htons(bsn[child]);
		((uint16_t *) dbuf)[5] = htons(fsn[child]);
		((uint32_t *) dbuf)[3] = status;
		break;
	case M2PA_VERSION_DRAFT6_9:
	case M2PA_VERSION_DRAFT7:
		data->len = 3*sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = status;
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		data->len = 5*sizeof(uint32_t);
		((uint32_t *) dbuf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) dbuf)[1] = htonl(data->len);
		((uint32_t *) dbuf)[2] = htonl(bsn[child] & 0x00ffffff);
		((uint32_t *) dbuf)[3] = htonl(fsn[child] & 0x00ffffff);
		((uint32_t *) dbuf)[4] = status;
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	ctrl->len = sizeof(p->npi.data_req) + sizeof(ptconf.qos.data);
	p->npi.type = N_DATA_REQ;
	p->npi.data_req.DATA_xfer_flags = 0;
	bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos.data));
	return test_putpmsg(child, ctrl, data, MSG_BAND, 0);
}

int
test_write(int child, const void *buf, size_t len)
{
	print_syscall(child, "write(2)------");
	for (;;) {
		if ((last_retval = write(test_fd[child], buf, len)) == -1) {
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

int
test_writev(int child, const struct iovec *iov, int num)
{
	print_syscall(child, "writev(2)-----");
	for (;;) {
		if ((last_retval = writev(test_fd[child], iov, num)) == -1) {
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

int
test_getmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *flagp)
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

int
test_getpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *bandp, int *flagp)
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

int
test_read(int child, void *buf, size_t count)
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

int
test_readv(int child, const struct iovec *iov, int count)
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

int
test_block(int child)
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

int
test_isastream(int child)
{
	int result;

	print_syscall(child, "isastream(2)--");
	if ((result = last_retval = isastream(test_fd[child])) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_poll(int child, const short events, short *revents, long ms)
{
	struct pollfd pfd = { .fd = test_fd[child], .events = events, .revents = 0 };
	int result;

	print_poll(child, events);
	if ((result = last_retval = poll(&pfd, 1, ms)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_poll_value(child, last_retval, pfd.revents);
	if (last_retval == 1 && revents)
		*revents = pfd.revents;
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
	switch (child) {
	case 1:
	case 2:
	case 0:
		for (i = 0; i < anums[child]; i++) {
#ifndef SCTP_VERSION_2
			addrs[child].port = htons(ports[child] + offset);
			inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
			addrs[child][i].sin_family = AF_INET;
			if ((child == 0 && !client_port_specified) ||
			    ((child == 1 || child == 2) && !server_port_specified))
				addrs[child][i].sin_port = htons(ports[child] + offset);
			else
				addrs[child][i].sin_port = htons(ports[child]);
			if ((child == 0 && !client_host_specified) ||
			    ((child == 1 || child == 2) && !server_host_specified))
				inet_aton(addr_strings[i], &addrs[child][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
		}
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

union primitives {
	ulong prim;
	union LMI_primitives lmi;
	union SDL_primitives sdl;
	union SDT_primitives sdt;
	union SL_primitives sl;
	union N_primitives npi;
};

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
	union primitives *p = (typeof(p)) cbuf;
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
		if (verbose > 3) {
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
		if (verbose > 3) {
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
		p->npi.error_ack.last_errno = last_errno;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->npi.type));
		print_string(child, nerrno_string(p->npi.error_ack.NPI_error, p->npi.error_ack.last_errno));
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

	case __TEST_POWER_ON:
		ctrl->len = sizeof(p->sl.power_on_req);
		p->sl.power_on_req.sl_primitive = SL_POWER_ON_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_START:
		ctrl->len = sizeof(p->sl.start_req);
		p->sl.start_req.sl_primitive = SL_START_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_STOP:
		ctrl->len = sizeof(p->sl.stop_req);
		p->sl.stop_req.sl_primitive = SL_STOP_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_LPO:
		ctrl->len = sizeof(p->sl.local_proc_outage_req);
		p->sl.local_proc_outage_req.sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_LPR:
		ctrl->len = sizeof(p->sl.resume_req);
		p->sl.resume_req.sl_primitive = SL_RESUME_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONG_A:
		ctrl->len = sizeof(p->sl.cong_accept_req);
		p->sl.cong_accept_req.sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONG_D:
		ctrl->len = sizeof(p->sl.cong_discard_req);
		p->sl.cong_discard_req.sl_primitive = SL_CONGESTION_DISCARD_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_NO_CONG:
		ctrl->len = sizeof(p->sl.no_cong_req);
		p->sl.no_cong_req.sl_primitive = SL_NO_CONGESTION_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EMERG:
		ctrl->len = sizeof(p->sl.emergency_req);
		p->sl.emergency_req.sl_primitive = SL_EMERGENCY_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CEASE:
		ctrl->len = sizeof(p->sl.emergency_ceases_req);
		p->sl.emergency_ceases_req.sl_primitive = SL_EMERGENCY_CEASES_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_COUNT:
		/* just a print action */
		return __RESULT_SUCCESS;
	case __TEST_TRIES:
		/* just a print action */
		return __RESULT_SUCCESS;
	case __TEST_ETC:
		/* just a print action */
		return __RESULT_SUCCESS;

	case __TEST_SEND_MSU:
	case __TEST_SEND_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		ctrl->len = sizeof(p->sl.pdu_req);
		data->len = msu_len;
		memset(dbuf, 'B', msu_len);
		test_pflags = MSG_BAND;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);

	case __TEST_ENABLE_REQ:
		ctrl->len = sizeof(p->lmi.enable_req);
		p->lmi.enable_req.lmi_primitive = LMI_ENABLE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISABLE_REQ:
		ctrl->len = sizeof(p->lmi.disable_req);
		p->lmi.disable_req.lmi_primitive = LMI_DISABLE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);

	case __TEST_ALIGNMENT:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_ALIGNMENT);
	case __TEST_PROVING_NORMAL:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_PROVING_NORMAL);
	case __TEST_PROVING_EMERG:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_PROVING_EMERG);
	case __TEST_OUT_OF_SERVICE:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_OUT_OF_SERVICE);
	case __TEST_IN_SERVICE:
		ptconf.qos_data.sid = 0;
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT3:
		case M2PA_VERSION_DRAFT3_1:
			/* called IN-SERVICE */
			break;
		default:
			/* called READY */
			break;
		}
		return test_m2pa_status(child, M2PA_STATUS_IN_SERVICE);
	case __TEST_SEQUENCE_SYNC:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			ptconf.qos_data.sid = 0;
			return test_m2pa_status(child, M2PA_STATUS_IN_SERVICE);
		default:
			return __RESULT_SUCCESS;
		}
	case __TEST_PROCESSOR_OUTAGE:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			ptconf.qos_data.sid = 1;
			break;
		default:
			ptconf.qos_data.sid = 0;
			break;
		}
		return test_m2pa_status(child, M2PA_STATUS_PROCESSOR_OUTAGE);
	case __TEST_PROCESSOR_ENDED:
		switch (m2pa_version) {
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
		case M2PA_VERSION_RFC4165:
			/* called PROCESSOR-RECOVERED */
			ptconf.qos.data.sid = 1;
			break;
		default:
			/* called PROCESSOR-OUTAGE-ENDED */
			ptconf.qos.data.sid = 0;
			break;
		}
		return test_m2pa_status(child, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED);
	case __TEST_BUSY:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_BUSY);
	case __TEST_BUSY_ENDED:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_BUSY_ENDED);
	case __TEST_INVALID_STATUS:
		ptconf.qos_data.sid = 0;
		return test_m2pa_status(child, M2PA_STATUS_INVALID);
	case __TEST_BAD_ACK:
	case __TEST_ACK:
	case __TEST_TX_BREAK:
	case __TEST_TX_MAKE:
	case __TEST_DISCON_REQ:
	case __TEST_BIND_REQ:
	case __TEST_OPTMGMT_REQ:
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

	if (child != 1) {
		if (ctrl->len >= sizeof(p->npi.type)) {
			switch ((PRIM_type = p->npi.type)) {
			case N_CONN_REQ:
				event = __TEST_CONN_REQ;
				print_rx_prim(child, prim_string(p->npi.type));
				print_options(child, cbuf, p->npi.conn_req.QOS_offset,
					      p->npi.conn_req.QOS_length);
				break;
			case N_CONN_RES:
				event = __TEST_CONN_RES;
				print_rx_prim(child, prim_string(p->npi.type));
				print_options(child, cbuf, p->npi.conn_res.QOS_offset,
					      p->npi.conn_res.QOS_length);
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
				print_string(child,
					     addr_string(cbuf + p->npi.unitdata_req.DEST_offset,
							 p->npi.unitdata_req.DEST_length));
#else
				print_addrs(child, cbuf + p->npi.unitdata_req.DEST_offset,
					    p->npi.unitdata_req.DEST_length);
#endif
				break;
			case N_OPTMGMT_REQ:
				event = __TEST_OPTMGMT_REQ;
				print_rx_prim(child, prim_string(p->npi.type));
				print_mgmtflag(child, p->npi.optmgmt_req.OPTMGMT_flags);
				print_options(child, cbuf, p->npi.optmgmt_req.QOS_offset,
					      p->npi.optmgmt_req.QOS_length);
				break;
			case N_CONN_IND:
				event = __TEST_CONN_IND;
				SEQ_number = p->npi.conn_ind.SEQ_number;
				CONN_flags = p->npi.conn_ind.CONN_flags;
				DEST_buffer =
				    (typeof(DEST_buffer)) (ctrl->buf + p->npi.conn_ind.DEST_offset);
				DEST_length = p->npi.conn_ind.DEST_length;
				SRC_buffer =
				    (typeof(SRC_buffer)) (ctrl->buf + p->npi.conn_ind.SRC_offset);
				SRC_length = p->npi.conn_ind.SRC_length;
				QOS_buffer =
				    (typeof(QOS_buffer)) (ctrl->buf + p->npi.conn_ind.QOS_offset);
				QOS_length = p->npi.conn_ind.QOS_length;
				DATA_buffer = data->buf;
				DATA_length = data->len;
				print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
				print_string(child,
					     addr_string(cbuf + p->npi.conn_ind.DEST_offset,
							 p->npi.conn_ind.DEST_length));
#else
				print_addrs(child, cbuf + p->npi.conn_ind.DEST_offset,
					    p->npi.conn_ind.DEST_length);
#endif
#ifndef SCTP_VERSION_2
				print_string(child,
					     addr_string(cbuf + p->npi.conn_ind.SRC_offset,
							 p->npi.conn_ind.SRC_length));
#else
				print_addrs(child, cbuf + p->npi.conn_ind.SRC_offset,
					    p->npi.conn_ind.SRC_length);
#endif
				print_options(child, cbuf, p->conn_ind.QOS_offset,
					      p->npi.conn_ind.QOS_length);
				break;
			case N_CONN_CON:
				event = __TEST_CONN_CON;
				print_rx_prim(child, prim_string(p->npi.type));
				print_addrs(child, cbuf + p->npi.conn_con.RES_offset,
					    p->npi.conn_con.RES_length);
				print_options(child, cbuf, p->npi.conn_con.QOS_offset,
					      p->npi.conn_con.QOS_length);
				break;
			case N_DISCON_IND:
				event = __TEST_DISCON_IND;
				print_rx_prim(child, prim_string(p->npi.type));
				break;
			case N_DATA_IND:
				event = __TEST_DATA_IND;
				DATA_xfer_flags = p->npi.data_ind.DATA_xfer_flags;
				print_rx_prim(child, prim_string(p->npi.type));
				sid[child] =
				    ((N_qos_sel_data_sctp_t *) (cbuf +
								sizeof(p->npi.data_ind)))->sid;
				// print_options(child, cbuf, sizeof(p->npi.data_ind),
				// sizeof(N_qos_sel_data_sctp_t));
				break;
			case N_EXDATA_IND:
				event = __TEST_EXDATA_IND;
				print_rx_prim(child, prim_string(p->npi.type));
				sid[child] =
				    ((N_qos_sel_data_sctp_t *) (cbuf +
								sizeof(p->npi.exdata_ind)))->sid;
				// print_options(child, cbuf, sizeof(p->npi.exdata_ind),
				// sizeof(N_qos_sel_data_sctp_t));
				break;
			case N_DATACK_IND:
				event = __TEST_DATACK_IND;
				print_rx_prim(child, prim_string(p->npi.type));
				sid[child] =
				    ((N_qos_sel_data_sctp_t *) (cbuf +
								sizeof(p->npi.datack_ind)))->sid;
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
				ADDR_buffer =
				    (typeof(ADDR_buffer)) (ctrl->buf + p->npi.bind_ack.ADDR_offset);
				ADDR_length = p->npi.bind_ack.ADDR_length;
				PROTOID_buffer =
				    (typeof(PROTOID_buffer)) (ctrl->buf +
							      p->npi.bind_ack.PROTOID_offset);
				PROTOID_length = p->npi.bind_ack.PROTOID_length;
				print_ack_prim(child, prim_string(p->npi.type));
				print_prots(child, cbuf + p->npi.bind_ack.PROTOID_offset,
					    p->npi.bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
				print_string(child,
					     addr_string(cbuf + p->npi.bind_ack.ADDR_offset,
							 p->npi.bind_ack.ADDR_length));
#else
				print_addrs(child, cbuf + p->npi.bind_ack.ADDR_offset,
					    p->npi.bind_ack.ADDR_length);
#endif
				if (verbose > 3) {
					snprintf(buf, sizeof(buf), "CONIND_number = %d",
						 CONIND_number);
					print_string(child, buf);
					snprintf(buf, sizeof(buf), "TOKEN_value = %x", TOKEN_value);
					print_string(child, buf);
				}
				break;
			case N_ERROR_ACK:
				event = __TEST_ERROR_ACK;
				NPI_error = p->npi.error_ack.NPI_error;
				last_errno = p->npi.error_ack.last_errno;
				print_ack_prim(child, prim_string(p->npi.type));
				print_string(child,
					     nerrno_string(p->npi.error_ack.NPI_error,
							   p->npi.error_ack.last_errno));
				break;
			case N_OK_ACK:
				event = __TEST_OK_ACK;
				print_ack_prim(child, prim_string(p->npi.type));
				break;
			case N_UNITDATA_IND:
				event = __TEST_UNITDATA_IND;
				print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
				print_string(child,
					     addr_string(cbuf + p->npi.unitdata_ind.SRC_offset,
							 p->npi.unitdata_ind.SRC_length));
#else
				print_addrs(child, cbuf + p->npi.unitdata_ind.SRC_offset,
					    p->npi.unitdata_ind.SRC_length);
#endif
				break;
			case N_UDERROR_IND:
				event = __TEST_UDERROR_IND;
				ADDR_buffer =
				    (typeof(ADDR_buffer)) (ctrl->buf +
							   p->npi.uderror_ind.DEST_offset);
				ADDR_length = p->npi.uderror_ind.DEST_length;
				RESERVED_field[0] = p->npi.uderror_ind.RESERVED_field;
				ERROR_type = p->npi.uderror_ind.ERROR_type;
				print_rx_prim(child, prim_string(p->npi.type));
#ifndef SCTP_VERSION_2
				print_string(child,
					     addr_string(cbuf + p->npi.uderror_ind.DEST_offset,
							 p->npi.uderror_ind.DEST_length));
#else
				print_addrs(child, cbuf + p->npi.uderror_ind.DEST_offset,
					    p->npi.uderror_ind.DEST_length);
#endif
				break;
			default:
				event = __EVENT_UNKNOWN;
				print_no_prim(child, p->npi.type);
				break;
			}
			if (data && data->len >= 0)
				if (do_decode_data(child, data) != __TEST_DATA)
					event = __RESULT_FAILURE;
		}
	} else {
		switch (p->prim) {
		}
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
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				break;
			print_errno(child, (last_errno = errno));
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
					if ((verbose > 3 && show) || (verbose > 5 && show_msg))
						print_syscall(child, "getmsg()");
					if ((ret = getmsg(test_fd[child], &ctrl, &data, &flags)) >= 0)
						break;
					if (last_errno == EINTR || last_errno == ERESTART)
						continue;
					print_errno(child, (last_errno = errno));
					if (last_errno == EAGAIN)
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
 *  M2PA Message Definitions
 *
 *  -------------------------------------------------------------------------
 */

#define M2PA_PPI		5

#define M2PA_MESSAGE_CLASS	11

#define M2PA_VERSION		1

#define M2PA_MTYPE_DATA		1
#define M2PA_MTYPE_STATUS	2
#define M2PA_MTYPE_PROVING	3
#define M2PA_MTYPE_ACK		4

#define M2PA_DATA_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_DATA)
#define M2PA_STATUS_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_STATUS)
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
#define M2PA_PROVING_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_PROVING)
#endif
#if defined(M2PA_VERSION_DRAFT3_1)
#define M2PA_ACK_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_ACK)
#endif

#define M2PA_STATUS_STREAM	0
#define M2PA_DATA_STREAM	1

#define M2PA_STATUS_ALIGNMENT			(__constant_htonl(1))
#define M2PA_STATUS_PROVING_NORMAL		(__constant_htonl(2))
#define M2PA_STATUS_PROVING_EMERGENCY		(__constant_htonl(3))
#define M2PA_STATUS_IN_SERVICE			(__constant_htonl(4))
#define M2PA_STATUS_PROCESSOR_OUTAGE		(__constant_htonl(5))
#define M2PA_STATUS_PROCESSOR_OUTAGE_ENDED	(__constant_htonl(6))
#define M2PA_STATUS_BUSY			(__constant_htonl(7))
#define M2PA_STATUS_BUSY_ENDED			(__constant_htonl(8))
#define M2PA_STATUS_OUT_OF_SERVICE		(__constant_htonl(9))
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)
#define M2PA_STATUS_NONE			(__constant_htonl(10))
#endif
#define M2PA_STATUS_INVALID			(__constant_htonl(11))

/*
 *  -------------------------------------------------------------------------
 *
 *  Globals
 *
 *  -------------------------------------------------------------------------
 */

static int state = 0;
static int event = 0;
static int count = 0;
static int tries = 0;
static int expand = 0;
static int debug = 1;
static int verbose = 1;
static long beg_time = 0;

static int summary = 0;
static int timer_scale = 1;
static int show_msg = 0;

static size_t nacks = 1;

static unsigned long iut_options = 0;


ulong tok[3] = { 0, 0, 0, };
ulong seq[3] = { 0, 0, 0, };
unsigned char fib[3] = { 0, 0, 0, };
unsigned char bib[3] = { 0, 0, 0, };

/* starts at zero if earlier than draft 10 */
unsigned int fsn[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff, };
unsigned int bsn[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff, };

/*
   listening file descriptor 
 */
static char mgm_buf[BUFSIZE];

/* 
   protocol tester file descriptor
 */
static char pt_buf[BUFSIZE];
// static unsigned char pt_li = 0;
// static unsigned char pt_sio = 0;

/*
   implementation under test file descriptor 
 */
static char iut_buf[BUFSIZE];

#define MSU_LEN 35
static int msu_len = MSU_LEN;

/*
   some globals for compressing events 
 */
static int oldpsb = 0;			/* last pt sequence number and indicator bits */
static int oldmsg = 0;			/* last pt message */
static int cntmsg = 0;			/* repeats of pt message */
static int oldact = 0;			/* last iut action */
static int cntact = 0;			/* repeats of iut action */
static int oldisb = 0;			/* last iut sequence number and indicator bits */
static int oldret = 0;
static int cntret = 0;
static int oldprm = 0;
static int cntprm = 0;
static int oldmgm = 0;			/* last management control */
static int cntmgm = 0;			/* repeats of management control */

union primitives {
	ulong prim;
	union LMI_primitives lmi;
	union SDL_primitives sdl;
	union SDT_primitives sdt;
	union SL_primitives sl;
	union N_primitives npi;
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Send messages from the Protocol Tester
 *
 *  -------------------------------------------------------------------------
 */
static void
pt_printf_sn(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	printf("%23.23s  --%06X, %06X->  %-23.23s\n", l, fsn[0] & 0xffffff, bsn[0] & 0xffffff, r);
#else
	printf("%23.23s  ----------------->  %-23.23s\n", l, r);
	FFLUSH(stdout);
#endif
}
static void
pt_printf(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	if (fsn[0] != 0xffffff || bsn[0] != 0xffffff)
		printf("%23.23s  --%06X, %06X->  %-23.23s\n", l, fsn[0] & 0xffffff, bsn[0] & 0xffffff, r);
	else
#endif
		printf("%23.23s  ----------------->  %-23.23s\n", l, r);
	FFLUSH(stdout);
}

#define send pt_send
static int
send(int msg)
{
	uint32_t status = 0;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*pt_buf), 0, pt_buf };
	union primitives *p = (union primitives *) cbuf;
	if (msg != oldmsg) {
		oldmsg = msg;
		if (verbose > 1) {
			if (cntmsg) {
				printf("             Ct=%5d                                               \n", cntmsg + 1);
				FFLUSH(stdout);
			}
		}
		cntmsg = 0;
	} else if (!expand)
		cntmsg++;
	switch (msg) {
	case __TEST_ALIGNMENT:
		if (!cntmsg)
			pt_printf("ALIGNMENT", "");
		status = M2PA_STATUS_ALIGNMENT;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_PROVING_NORMAL:
		if (!cntmsg)
			pt_printf("PROVING-NORMAL", "");
		status = M2PA_STATUS_PROVING_NORMAL;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_PROVING_EMERG:
		if (!cntmsg)
			pt_printf("PROVING-EMERGENCY", "");
		status = M2PA_STATUS_PROVING_EMERGENCY;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_OUT_OF_SERVICE:
		if (!cntmsg)
			pt_printf("OUT-OF-SERVICE", "");
		status = M2PA_STATUS_OUT_OF_SERVICE;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_IN_SERVICE:
		if (!cntmsg)
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			pt_printf("IN-SERVICE", "");
#else
			pt_printf_sn("READY", "");
#endif
		status = M2PA_STATUS_IN_SERVICE;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_SEQUENCE_SYNC:
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
		if (!cntmsg)
			pt_printf_sn("READY", "");
		status = M2PA_STATUS_IN_SERVICE;
		ptconf.qos_data.sid = 1;
		goto pt_status_putmsg;
#else
		return __RESULT_SUCCESS;
#endif
	case __TEST_PROCESSOR_OUTAGE:
		if (!cntmsg)
			pt_printf("PROCESSOR-OUTAGE", "");
		status = M2PA_STATUS_PROCESSOR_OUTAGE;
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
		ptconf.qos_data.sid = 1;
#else
		ptconf.qos_data.sid = 0;
#endif
		goto pt_status_putmsg;
	case __TEST_PROCESSOR_ENDED:
		if (!cntmsg)
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
			pt_printf_sn("PROCESSOR-RECOVERED", "");
#else
			pt_printf("PROCESSOR-OUTAGE-ENDED", "");
#endif
		status = M2PA_STATUS_PROCESSOR_OUTAGE_ENDED;
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
		ptconf.qos_data.sid = 1;
#else
		ptconf.qos_data.sid = 0;
#endif
		goto pt_status_putmsg;
	case __TEST_BUSY_ENDED:
		if (!cntmsg)
			pt_printf("BUSY-ENDED", "");
		status = M2PA_STATUS_BUSY_ENDED;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_BUSY:
		if (!cntmsg)
			pt_printf("BUSY", "");
		status = M2PA_STATUS_BUSY;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case __TEST_COUNT:
		printf("           Ct = %5d    ----------------->                         \n", count);
		return __RESULT_SUCCESS;
	case __TEST_TRIES:
		printf("       %5d iterations  ----------------->                         \n", tries);
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_ETC:
		printf("                    .    ----------------->                         \n");
		printf("                    .    ----------------->                         \n");
		printf("                    .    ----------------->                         \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_INVALID_STATUS:
		if (!cntmsg)
			pt_printf("[INVALID-STATUS]", "");
		status = M2PA_STATUS_INVALID;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	      pt_status_putmsg:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		data.len = 3 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(bsn[0]);
		((uint16_t *) pt_buf)[5] = htons(fsn[0]);
		((uint32_t *) pt_buf)[3] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(bsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(fsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[4] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)
		data.len = 3 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
		return __RESULT_FAILURE;
#endif
#endif
#endif
#endif
	case __TEST_MSU_TOO_SHORT:
		msu_len = 1;
		goto send_data;
	case __TEST_DATA:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
	      send_data:
		ptconf.qos_data.sid = 1;
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		if (!cntmsg) {
			printf("   [%3d bytes]     DATA  ----------------->                         \n", msu_len);
			FFLUSH(stdout);
		}
		data.len = 2 * sizeof(uint32_t) + msu_len + 1;
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		memset(&(((uint32_t *) pt_buf)[2]), 'B', msu_len + 1);
		memset(&(((uint32_t *) pt_buf)[2]), 0, 1);
		p->npi.type = N_DATA_REQ;
#if defined(M2PA_VERSION_DRAFT3)
		p->npi.data_req.DATA_xfer_flags = N_RC_FLAG;
#else
		p->npi.data_req.DATA_xfer_flags = 0;
#endif
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		fsn[0]++;
		fsn[0] &= 0xffffff;
		if (!cntmsg) {
			printf("   [%3d bytes]     DATA  --%06X, %06X->                         \n", msu_len, fsn[0] & 0xffffff, bsn[0] & 0xffffff);
			FFLUSH(stdout);
		}
		data.len = 3 * sizeof(uint32_t) + msu_len + 1;
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(bsn[0]);
		((uint16_t *) pt_buf)[5] = htons(fsn[0]);
		memset(&(((uint32_t *) pt_buf)[3]), 'B', msu_len + 1);
		memset(&(((uint32_t *) pt_buf)[3]), 0, 1);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		fsn[0]++;
		fsn[0] &= 0xffffff;
		if (!cntmsg) {
			printf("   [%3d bytes]     DATA  --%06X, %06X->                         \n", msu_len, fsn[0] & 0xffffff, bsn[0] & 0xffffff);
			FFLUSH(stdout);
		}
		data.len = 4 * sizeof(uint32_t) + msu_len + 1;
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(bsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(fsn[0] & 0xffffff);
		memset(&(((uint32_t *) pt_buf)[4]), 'B', msu_len + 1);
		memset(&(((uint32_t *) pt_buf)[4]), 0, 1);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
		return __RESULT_FAILURE;
#endif
#endif
#endif
	case __TEST_BAD_ACK:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		return __RESULT_INCONCLUSIVE;
#else
		fsn[0] = 0xffff;
		goto pt_ack_putmsg;
#endif
	case __TEST_ACK:
#if defined(M2PA_VERSION_DRAFT3_1)
		pt_printf("ACK", "");
		ptconf.qos_data.sid = 1;
		data.len = 3 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_ACK_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(nacks);
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4_1)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(bsn[0]);
		((uint16_t *) pt_buf)[5] = htons(fsn[0]);
		((uint32_t *) pt_buf)[3] = M2PA_STATUS_NONE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT4_9)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htons(bsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[3] = htons(fsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[4] = M2PA_STATUS_NONE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htons(bsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[3] = htons(fsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[4] = M2PA_STATUS_IN_SERVICE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	      pt_ack_putmsg:
		printf("               DATA-ACK  --%06X, %06X->                         \n", fsn[0] & 0xffffff, bsn[0] & 0xffffff);
		FFLUSH(stdout);
		ptconf.qos_data.sid = 1;
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(bsn[0] & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(fsn[0] & 0xffffff);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(test_fd[0], &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
#else
		return __RESULT_INCONCLUSIVE;
#endif
#endif
#endif
#endif
#endif
	case __TEST_STOP:
		printf("                  :stop                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_POWER_ON:
		printf("              :power on                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_LPO:
		printf("               :set lpo                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_LPR:
		printf("             :clear lpo                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_EMERG:
		printf("             :set emerg                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_CEASE:
		printf("           :clear emerg                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_CONG_A:
		printf("        :make congested                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_CONG_D:
		printf("        :make congested                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_NO_CONG:
		printf("       :clear congested                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_START:
		printf("                 :start                                             \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_TX_BREAK:
		printf("              :tx break                                             \n");
		FFLUSH(stdout);
		p->npi.type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = 0;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		ctrl.len = sizeof(N_discon_req_t);
		goto pt_control_putmsg;
	case __TEST_TX_MAKE:
		return __RESULT_SUCCESS;
	case __TEST_ENABLE_REQ:
		if (verbose > 1) {
			printf("                :enable                                             \n");
			FFLUSH(stdout);
		}
		return __RESULT_SUCCESS;
	case __TEST_DISABLE_REQ:
		if (verbose > 1) {
			printf("               :disable                                             \n");
			FFLUSH(stdout);
		}
		return __RESULT_SUCCESS;
	case __TEST_DISCON_REQ:
		printf("            :disconnect                                             \n");
		FFLUSH(stdout);
		p->npi.type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = 0;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		ctrl.len = sizeof(N_discon_req_t);
		goto pt_control_putmsg;
	case __TEST_BIND_REQ:
		if (verbose > 1) {
			printf("                  :bind                                             \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_BIND_REQ;
		p->npi.bind_req.ADDR_length = sizeof(ptconf.loc);
		p->npi.bind_req.ADDR_offset = sizeof(p->npi.bind_req);
		p->npi.bind_req.CONIND_number = 0;
		p->npi.bind_req.BIND_flags = TOKEN_REQUEST;
		p->npi.bind_req.PROTOID_length = 0;
		p->npi.bind_req.PROTOID_offset = 0;
		bcopy(&ptconf.loc, (&p->npi.bind_req + 1), sizeof(ptconf.loc));
		ctrl.len = sizeof(N_bind_req_t) + sizeof(ptconf.loc);
		goto pt_control_putmsg;
	case __TEST_OPTMGMT_REQ:
		if (verbose > 1) {
			printf("           :optmgmt req                                             \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_OPTMGMT_REQ;
		p->npi.optmgmt_req.QOS_length = sizeof(ptconf.qos);
		p->npi.optmgmt_req.QOS_offset = sizeof(p->npi.optmgmt_req);
		p->npi.optmgmt_req.OPTMGMT_flags = 0;
		bcopy(&ptconf.qos, (&p->npi.optmgmt_req + 1), sizeof(ptconf.qos));
		ctrl.len = sizeof(N_optmgmt_req_t) + sizeof(ptconf.qos);
		goto pt_control_putmsg;
	      pt_control_putmsg:
		if (putmsg(test_fd[0], &ctrl, NULL, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	default:
		if (verbose > 1 || !cntmsg) {
			printf("              :????????                                             \n");
			FFLUSH(stdout);
		}
		return __RESULT_FAILURE;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Signal commands to the Implementation Under Test
 *
 *  -------------------------------------------------------------------------
 */
#define signal iut_signal
static int
signal(int action)
{
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union primitives *p = (union primitives *) cbuf;
	if (action != oldact) {
		oldact = action;
		if (verbose > 1) {
			if (cntact) {
				printf("                                                     Ct=%5d       \n", cntact + 1);
				FFLUSH(stdout);
			}
		}
		cntact = 0;
	} else if (!expand)
		cntact++;
	switch (action) {
	case __TEST_POWER_ON:
		printf("                                             :power on              \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_POWER_ON_REQ;
		ctrl.len = SL_POWER_ON_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_START:
		printf("                                             :start                 \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_START_REQ;
		ctrl.len = SL_START_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_STOP:
		printf("                                             :stop                  \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_STOP_REQ;
		ctrl.len = SL_STOP_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_LPO:
		printf("                                             :set lpo               \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		ctrl.len = SL_LOCAL_PROC_OUTAGE_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_LPR:
		printf("                                             :clear lpo             \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_RESUME_REQ;
		ctrl.len = SL_RESUME_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_CONG_A:
		printf("                                             :make cong accept      \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		ctrl.len = SL_CONG_ACCEPT_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_CONG_D:
		printf("                                             :make cong discard     \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CONGESTION_DISCARD_REQ;
		ctrl.len = SL_CONG_DISCARD_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_NO_CONG:
		printf("                                             :clear congestion      \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_NO_CONGESTION_REQ;
		ctrl.len = SL_NO_CONG_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_CLEARB:
		printf("                                             :clear buffers         \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CLEAR_BUFFERS_REQ;
		ctrl.len = SL_CLEAR_BUFFERS_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_EMERG:
		printf("                                             :set emergency         \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_EMERGENCY_REQ;
		ctrl.len = SL_EMERGENCY_REQ_SIZE;
		goto signal_iut_putmsg;
	case __TEST_CEASE:
		printf("                                             :clear emergency       \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_EMERGENCY_CEASES_REQ;
		ctrl.len = SL_EMERGENCY_CEASES_REQ_SIZE;
	      signal_iut_putmsg:
		if (putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	case __TEST_COUNT:
		printf("                                                     Ct=%5d       \n", count);
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_TRIES:
		printf("                                                     %4d iterations\n", tries);
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_ETC:
		printf("                                                     .              \n");
		printf("                                                     .              \n");
		printf("                                                     .              \n");
		FFLUSH(stdout);
		return __RESULT_SUCCESS;
	case __TEST_SEND_MSU:
		if (!cntact) {
			printf("                                             :msu                   \n");
			FFLUSH(stdout);
		}
	case __TEST_SEND_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		p->sl.sl_primitive = SL_PDU_REQ;
		ctrl.len = SL_PDU_REQ_SIZE;
		memset(dbuf, 'B', msu_len);
		data.len = msu_len;
		if (putmsg(test_fd[1], NULL, &data, 0) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	case __TEST_ENABLE_REQ:
		if (verbose > 1) {
			printf("                                             :enable                \n");
			FFLUSH(stdout);
		}
		p->lmi.enable_req.lmi_primitive = LMI_ENABLE_REQ;
		ctrl.len = sizeof(p->lmi.enable_req);
		goto iut_lmi_putmsg;
	case __TEST_DISABLE_REQ:
		if (verbose > 1) {
			printf("                                             :disable               \n");
			FFLUSH(stdout);
		}
		p->lmi.disable_req.lmi_primitive = LMI_DISABLE_REQ;
		ctrl.len = sizeof(p->lmi.disable_req);
		goto iut_lmi_putmsg;
	      iut_lmi_putmsg:
		if (putmsg(test_fd[1], &ctrl, NULL, RS_HIPRI) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	case __TEST_CONN_REQ:
		printf("                                             :connect               \n");
		FFLUSH(stdout);
		p->npi.type = N_CONN_REQ;
		p->npi.conn_req.DEST_length = sizeof(iutconf.rem);
		p->npi.conn_req.DEST_offset = sizeof(p->npi.conn_req);
		p->npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->npi.conn_req.QOS_length = 0;
		p->npi.conn_req.QOS_offset = 0;
		bcopy(&iutconf.rem, (&p->npi.conn_req + 1), sizeof(iutconf.rem));
		ctrl.len = sizeof(N_conn_req_t) + sizeof(iutconf.rem);
		goto iut_control_putmsg;
	case __TEST_BIND_REQ:
		if (verbose > 1) {
			printf("                                             <bind req              \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_BIND_REQ;
		p->npi.bind_req.ADDR_length = sizeof(iutconf.loc);
		p->npi.bind_req.ADDR_offset = sizeof(p->npi.bind_req);
		p->npi.bind_req.CONIND_number = 0;
		p->npi.bind_req.BIND_flags = TOKEN_REQUEST;
		p->npi.bind_req.PROTOID_length = 0;
		p->npi.bind_req.PROTOID_offset = 0;
		bcopy(&iutconf.loc, (&p->npi.bind_req + 1), sizeof(iutconf.loc));
		ctrl.len = sizeof(N_bind_req_t) + sizeof(iutconf.loc);
		goto iut_control_putmsg;
	case __TEST_DISCON_REQ:
		if (verbose > 1) {
			printf("                                             :disconnect            \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = 0;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		ctrl.len = sizeof(N_discon_req_t);
		goto iut_control_putmsg;
	case __TEST_OPTMGMT_REQ:
		if (verbose > 1) {
			printf("                                             <optmgmt req           \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_OPTMGMT_REQ;
		p->npi.optmgmt_req.QOS_length = sizeof(iutconf.qos);
		p->npi.optmgmt_req.QOS_offset = sizeof(p->npi.optmgmt_req);
		p->npi.optmgmt_req.OPTMGMT_flags = 0;
		bcopy(&iutconf.qos, (&p->npi.optmgmt_req + 1), sizeof(iutconf.qos));
		ctrl.len = sizeof(N_optmgmt_req_t) + sizeof(iutconf.qos);
		goto iut_control_putmsg;
	      iut_control_putmsg:
		if (putmsg(test_fd[1], &ctrl, NULL, 0) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	default:
		if (!cntact) {
			printf("                                             <????????              \n");
			FFLUSH(stdout);
		}
		return __RESULT_FAILURE;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Control commands given to management stream.
 *
 *  -------------------------------------------------------------------------
 */
#define control mgm_control
static int
control(int prim)
{
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	union primitives *p = (union primitives *) cbuf;
	if (prim != oldmgm) {
		oldmgm = prim;
		if (cntmgm) {
			printf("             Ct=%5d                                               \n", cntmgm + 1);
			FFLUSH(stdout);
		}
		cntmgm = 0;
	} else if (!expand)
		cntmgm++;
	switch (prim) {
	case __TEST_CONN_RES:
		if (verbose > 1) {
			printf("              >conn res                                             \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_CONN_RES;
		p->npi.conn_res.TOKEN_value = tok[0];
		p->npi.conn_res.RES_length = 0;
		p->npi.conn_res.RES_offset = 0;
		p->npi.conn_res.SEQ_number = seq[2];
		p->npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->npi.conn_res.QOS_length = 0;
		p->npi.conn_res.QOS_offset = 0;
		ctrl.len = sizeof(N_conn_res_t);
		goto mgm_control_putmsg;
	case __TEST_BIND_REQ:
		if (verbose > 1) {
			printf("              >bind req |                        \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_BIND_REQ;
		p->npi.bind_req.ADDR_length = sizeof(mgmconf.loc);
		p->npi.bind_req.ADDR_offset = sizeof(p->npi.bind_req);
		p->npi.bind_req.CONIND_number = 2;
		p->npi.bind_req.BIND_flags = TOKEN_REQUEST;
		p->npi.bind_req.PROTOID_length = 0;
		p->npi.bind_req.PROTOID_offset = 0;
		bcopy(&mgmconf.loc, (&p->npi.bind_req + 1), sizeof(mgmconf.loc));
		ctrl.len = sizeof(N_bind_req_t) + sizeof(mgmconf.loc);
		goto mgm_control_putmsg;
	case __TEST_OPTMGMT_REQ:
		if (verbose > 1) {
			printf("           >optmgmt req |                        \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_OPTMGMT_REQ;
		p->npi.optmgmt_req.QOS_length = sizeof(mgmconf.qos);
		p->npi.optmgmt_req.QOS_offset = sizeof(p->npi.optmgmt_req);
		p->npi.optmgmt_req.OPTMGMT_flags = 0;
		bcopy(&mgmconf.qos, (&p->npi.optmgmt_req + 1), sizeof(mgmconf.qos));
		ctrl.len = sizeof(N_optmgmt_req_t) + sizeof(mgmconf.qos);
		goto mgm_control_putmsg;
	      mgm_control_putmsg:
		if (putmsg(test_fd[2], &ctrl, NULL, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return __RESULT_FAILURE;
		}
		return __RESULT_SUCCESS;
	default:
		if (!cntmgm) {
			printf("              >???????? |                        \n");
			FFLUSH(stdout);
		}
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

/*
 *  =========================================================================
 *
 *  DECODE DATA and CTRL
 *
 *  =========================================================================
 */
static int show_msus = 1;
static int show_fisus = 1;

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode data at Protocol Tester
 *
 *  -------------------------------------------------------------------------
 */
static void
iut_printf_sn(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	printf("%23.23s  <-%06X, %06X--  %-23.23s\n", l, bsn[1] & 0xffffff, fsn[1] & 0xffffff, r);
#else
	printf("%23.23s  <-----------------  %-23.23s\n", l, r);
#endif
	FFLUSH(stdout);
}
static void
iut_printf(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	if (fsn[1] != 0xffffff || bsn[1] != 0xffffff)
		printf("%23.23s  <-%06X, %06X--  %-23.23s\n", l, bsn[1] & 0xffffff, fsn[1] & 0xffffff, r);
	else
#endif
		printf("%23.23s  <-----------------  %-23.23s\n", l, r);
	FFLUSH(stdout);
}

static int
pt_decode_data(void)
{
	int ret;
	if (debug && verbose > 1) {
		printf("pt decode data...       .    .        .    .                        \n");
		FFLUSH(stdout);
	}
	switch (((uint32_t *) pt_buf)[0]) {
	case M2PA_STATUS_MESSAGE:
	{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)
		uint mystatus = ((uint32_t *) pt_buf)[2];
#else
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		uint mystatus = ((uint32_t *) pt_buf)[3];
#else
#if defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		uint mystatus = ((uint32_t *) pt_buf)[4];
#else
#error "Draft poorly defined."
#endif
#endif
#endif
		switch (mystatus) {
		case M2PA_STATUS_OUT_OF_SERVICE:
			ret = __TEST_OUT_OF_SERVICE;
			break;
		case M2PA_STATUS_IN_SERVICE:
			ret = __TEST_IN_SERVICE;
			break;
		case M2PA_STATUS_PROVING_NORMAL:
			ret = __TEST_PROVING_NORMAL;
			break;
		case M2PA_STATUS_PROVING_EMERGENCY:
			ret = __TEST_PROVING_EMERG;
			break;
		case M2PA_STATUS_ALIGNMENT:
			ret = __TEST_ALIGNMENT;
			break;
		case M2PA_STATUS_PROCESSOR_OUTAGE:
			ret = __TEST_PROCESSOR_OUTAGE;
			break;
		case M2PA_STATUS_BUSY:
			ret = __TEST_BUSY;
			break;
		case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
			ret = __TEST_PROCESSOR_ENDED;
			break;
		case M2PA_STATUS_BUSY_ENDED:
			ret = __TEST_BUSY_ENDED;
			break;
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)
		case M2PA_STATUS_NONE:
			ret = __TEST_ACK;
			break;
#endif
		default:
		{
			int i;
			printf("ERROR: invalid status = %d\n", mystatus);
			printf("pt_buf =");
			for (i = 0; i < 20; i++) {
				printf(" %02x", pt_buf[i]);
			}
			printf("\n");
			ret = __TEST_INVALID_STATUS;
			break;
		}
		}
		break;
	}
#if defined(M2PA_VERSION_DRAFT3_1)
	case M2PA_ACK_MESSAGE:
		ret = __TEST_ACK;
		break;
#endif
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	case M2PA_PROVING_MESSAGE:
		ret = __TEST_PROVING;
		break;
#endif
	case M2PA_DATA_MESSAGE:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		if (ntohl(((uint32_t *) pt_buf)[1]) == 4 * sizeof(uint32_t)) {
			ret = __TEST_ACK;
			break;
		}
#endif
		ret = __TEST_DATA;
		break;
	default:
		ret = __RESULT_FAILURE;
		break;
	}

	if (ret != oldret) {
		if ((ret == __TEST_DATA || ret == __TEST_ACK) && !expand)
			cntmsg++;
		else
			cntret = 0;
		oldret = ret;
		if (verbose > 1) {
			if (cntret) {
				printf("    Ct=%5d                                                        \n", cntret + 1);
				FFLUSH(stdout);
			}
		}
		cntret = 0;
	} else if (!expand)
		cntret++;
#if 0
	if (show_fisus || ret != __TEST_FISU || verbose > 1) {
		if (ret != oldret || oldisb != (((bib[1] | bsn[1]) << 8) | (fib[1] | fsn[1]))) {
			// if ( oldisb == (((bib[1]|bsn[1])<<8)|(fib[1]|fsn[1])) &&
			// ( ( ret == __TEST_FISU && oldret == __TEST_DATA ) || ( ret == __TEST_DATA && oldret == __TEST_FISU ) 
			// ) )
			// {
			// if ( ret == __TEST_DATA && !expand )
			// cntmsg++;
			// }
			// else
			cntret = 0;
			oldret = ret;
			oldisb = ((bib[1] | bsn[1]) << 8) | (fib[1] | fsn[1]);
			if (verbose > 1) {
				if (cntret) {
					printf("    Ct=%5d                                                        \n", cntret + 1);
					FFLUSH(stdout);
				}
			}
			cntret = 0;
		} else if (!expand)
			cntret++;
	}
#endif
	if (!cntret) {
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		bsn[1] = ntohl(((uint32_t *) pt_buf)[2]) >> 16;
		fsn[1] = ntohl(((uint32_t *) pt_buf)[2]) & 0xffff;
#else
		bsn[1] = ntohl(((uint32_t *) pt_buf)[2]) & 0xffffff;
		fsn[1] = ntohl(((uint32_t *) pt_buf)[3]) & 0xffffff;
#endif
		switch (ret) {
		case __TEST_IN_SERVICE:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			iut_printf("", "IN-SERVICE");
#else
			iut_printf_sn("", "READY");
#endif
			return ret;
		case __TEST_ALIGNMENT:
			iut_printf("", "ALIGNMENT");
			return ret;
		case __TEST_PROVING_NORMAL:
			iut_printf("", "PROVING-NORMAL");
			return ret;
		case __TEST_PROVING_EMERG:
			iut_printf("", "PROVING-EMERGENCY");
			return ret;
		case __TEST_OUT_OF_SERVICE:
			iut_printf("", "OUT-OF-SERVICE");
			return ret;
		case __TEST_PROCESSOR_OUTAGE:
			iut_printf("", "PROCESSOR-OUTAGE");
			return ret;
		case __TEST_BUSY:
			iut_printf("", "BUSY");
			return ret;
		case __TEST_INVALID_STATUS:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)
			printf("                         <-----------------  [INVALID STATUS %5u] \n", ntohl(((uint32_t *) pt_buf)[2]));
#else
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
			printf("                         <-%06X, %06X--  [INVALID STATUS %5u] \n", bsn[1] & 0xffffff, fsn[1] & 0xffffff, ntohl(((uint32_t *) pt_buf)[3]));
#else
#if defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
			printf("                         <-%06X, %06X--  [INVALID STATUS %5u] \n", bsn[1] & 0xffffff, fsn[1] & 0xffffff, ntohl(((uint32_t *) pt_buf)[4]));
#else
#error "Poorly defined version."
#endif
#endif
#endif
			FFLUSH(stdout);
			return ret;
		case __TEST_PROCESSOR_ENDED:
#if defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
			iut_printf_sn("", "PROCESSOR-RECOVERED");
#else
			iut_printf("", "PROCESSOR-OUTAGE-ENDED");
#endif
			return ret;
		case __TEST_BUSY_ENDED:
			iut_printf("", "BUSY-ENDED");
			return ret;
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)
		case __TEST_ACK:
			iut_printf_sn("", "IN-SERVICE");
			return ret;
#else
#if defined(M2PA_VERSION_DRAFT3_1)
		case __TEST_ACK:
			printf("                         <-----------------  ACK [%5u msgs]       \n", ntohl(((uint32_t *) pt_buf)[2]));
			FFLUSH(stdout);
			return ret;
#else
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		case __TEST_ACK:
			printf("                         <-%06X, %06X--  DATA-ACK               \n", bsn[1] & 0xffffff, fsn[1] & 0xffffff);
			FFLUSH(stdout);
			return ret;
#endif
#endif
#endif
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		case __TEST_PROVING:
			printf("                         <-----------------  PROVING [%5u bytes]  \n", ntohl(((uint32_t *) pt_buf)[1]) - 2 * sizeof(uint32_t));
			FFLUSH(stdout);
			return ret;
#endif
		case __TEST_DATA:
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)
			bsn[0] = fsn[1];
#endif
			if (show_msus || verbose > 1) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
				printf("                         <-----------------  DATA [%5lu bytes]     \n", bsn[1], fsn[1], (ulong) ntohl(((uint32_t *) pt_buf)[1]) - 2 * sizeof(uint32_t));
#else
				printf("                         <-%06X, %06X--  DATA [%5lu bytes]     \n", bsn[1] & 0xffffff, fsn[1] & 0xffffff, (ulong) ntohl(((uint32_t *) pt_buf)[1]) - 4 * sizeof(uint32_t));
#endif
				FFLUSH(stdout);
				return ret;
			}
			return ret;
		default:
			printf("                         <-----------------  ????????               \n");
			FFLUSH(stdout);
			return ret;
		}
	}
	return ret;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode ctrl at Protocol Tester
 *
 *  -------------------------------------------------------------------------
 */
static int
pt_decode_msg(char *buf)
{
	union primitives *p = (union primitives *) buf;
	if (debug && verbose > 1)
		printf("pt decode msg...         <-----------------                         \n");
	switch (p->npi.type) {
	case N_DATA_IND:
		if (verbose > 1) {
			printf("              !data ind                                             \n");
			FFLUSH(stdout);
		}
		return pt_decode_data();
	case N_EXDATA_IND:
		if (verbose > 1) {
			printf("            !exdata ind                                             \n");
			FFLUSH(stdout);
		}
		return pt_decode_data();
	case N_CONN_IND:
		if (verbose > 1) {
			printf("              !conn ind                                             \n");
			FFLUSH(stdout);
		}
		seq[0] = p->npi.conn_ind.SEQ_number;
		return __TEST_UNKNOWN;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("              !conn con                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("            !discon ind                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_DISCON_IND;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("              !info ack                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_BIND_ACK:
		if (verbose > 1) {
			printf("              !bind ack                                             \n");
			printf("    cons = %lu\n", (ulong)p->npi.bind_ack.CONIND_number);
			printf("    tok  = %lx\n", (ulong)p->npi.bind_ack.TOKEN_value);
			printf("    alen = %lu\n", (ulong)p->npi.bind_ack.ADDR_length);
			if (p->npi.bind_ack.ADDR_length == 14) {
				addr_t *a = (addr_t *) (buf + p->npi.bind_ack.ADDR_offset);
				printf("    port = %u\n", ntohs(a->port));
				printf("    add1 = %d.%d.%d.%d\n", (a->addr[0] >> 0) & 0xff, (a->addr[0] >> 8) & 0xff, (a->addr[0] >> 16) & 0xff, (a->addr[0] >> 24) & 0xff);
				printf("    add2 = %d.%d.%d.%d\n", (a->addr[1] >> 0) & 0xff, (a->addr[1] >> 8) & 0xff, (a->addr[1] >> 16) & 0xff, (a->addr[1] >> 24) & 0xff);
				printf("    add3 = %d.%d.%d.%d\n", (a->addr[2] >> 0) & 0xff, (a->addr[2] >> 8) & 0xff, (a->addr[2] >> 16) & 0xff, (a->addr[2] >> 24) & 0xff);
			}
			FFLUSH(stdout);
		}
		tok[0] = p->npi.bind_ack.TOKEN_value;
		return __TEST_BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("             !error ack                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("                !ok ack                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("          !unitdata ind                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("           !uderror ind                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("            !datack ind                                             \n");
			FFLUSH(stdout);
		}
#if defined(M2PA_VERSION_DRAFT3_1)
		return __TEST_ACK;
#else
		return __TEST_UNKNOWN;
#endif
	case N_RESET_IND:
		if (verbose > 1) {
			printf("             !reset ind                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("             !reset con                                             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	default:
		printf("         !(unknown %3ld)                                             \n", (long)p->npi.type);
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode data at Implementation Under Test
 *
 *  -------------------------------------------------------------------------
 */
static int
iut_decode_data(void)
{
	printf("                                             !msu                   \n");
	FFLUSH(stdout);
	return __TEST_IUT_DATA;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode ctrl at Implementation Under Test
 *
 *  -------------------------------------------------------------------------
 */
static int
iut_decode_msg(char *buf)
{
	char *reason;
	union primitives *p = (union primitives *) buf;
	if (p->sl.sl_primitive != oldprm) {
		oldprm = p->sl.sl_primitive;
		cntprm = 0;
	} else if (!expand)
		cntprm++;
	switch (p->prim) {
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		if (!cntprm) {
			printf("                                             !rpo                   \n");
			FFLUSH(stdout);
		}
		return __TEST_IUT_RPO;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		if (!cntprm) {
			printf("                                             !rpr                   \n");
			FFLUSH(stdout);
		}
		return __TEST_IUT_RPR;
	case SL_IN_SERVICE_IND:
		printf("                                             !in service            \n");
		FFLUSH(stdout);
		return __TEST_IUT_IN_SERVICE;
	case SL_OUT_OF_SERVICE_IND:
		switch (p->sl.out_of_service_ind.sl_reason) {
		case SL_FAIL_UNSPECIFIED:
			reason = "unspec";
			break;
		case SL_FAIL_CONG_TIMEOUT:
			reason = "T6";
			break;
		case SL_FAIL_ACK_TIMEOUT:
			reason = "T7";
			break;
		case SL_FAIL_ABNORMAL_BSNR:
			reason = "BSNR";
			break;
		case SL_FAIL_ABNORMAL_FIBR:
			reason = "FIBR";
			break;
		case SL_FAIL_SUERM_EIM:
			reason = "SUERM";
			break;
		case SL_FAIL_ALIGNMENT_NOT_POSSIBLE:
			reason = "AERM";
			break;
		case SL_FAIL_RECEIVED_SIO:
			reason = "SIO";
			break;
		case SL_FAIL_RECEIVED_SIN:
			reason = "SIN";
			break;
		case SL_FAIL_RECEIVED_SIE:
			reason = "SIE";
			break;
		case SL_FAIL_RECEIVED_SIOS:
			reason = "SIOS";
			break;
		case SL_FAIL_T1_TIMEOUT:
			reason = "T1";
			break;
		default:
			reason = "???";
			break;
		}
		printf("                                             !out of service(%s)\n", reason);
		FFLUSH(stdout);
		return __TEST_IUT_OUT_OF_SERVICE;
	case SL_PDU_IND:
		printf("                                             !msu                   \n");
		FFLUSH(stdout);
		return __TEST_IUT_DATA;
	case SL_LINK_CONGESTED_IND:
		printf("                                             !congested             \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_LINK_CONGESTION_CEASED_IND:
		printf("                                             !congestion ceased     \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_RETRIEVED_MESSAGE_IND:
		printf("                                             !retrieved message     \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_RETRIEVAL_COMPLETE_IND:
		printf("                                             !retrieval compelte    \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_RB_CLEARED_IND:
		printf("                                             !rb cleared            \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_BSNT_IND:
		printf("                                             !bsnt                  \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case SL_RTB_CLEARED_IND:
		printf("                                             !rtb cleared           \n");
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	case LMI_INFO_ACK:
		if (verbose > 1) {
			printf("                                             !info ack              \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case LMI_OK_ACK:
		if (verbose > 1) {
			printf("                                             !ok ack                \n");
			FFLUSH(stdout);
		}
		return __TEST_OK_ACK;
	case LMI_ERROR_ACK:
		if (verbose > 1) {
			printf("                                             !error ack             \n");
			FFLUSH(stdout);
		}
		return __TEST_ERROR_ACK;
	case LMI_ENABLE_CON:
		if (verbose > 1) {
			printf("                                             !enable con            \n");
			FFLUSH(stdout);
		}
		return __TEST_ENABLE_CON;
	case LMI_DISABLE_CON:
		if (verbose > 1) {
			printf("                                             !disable con           \n");
			FFLUSH(stdout);
		}
		return __TEST_DISABLE_CON;
	case LMI_ERROR_IND:
		if (verbose > 1) {
			printf("                                             !error ind             \n");
			FFLUSH(stdout);
		}
		return __TEST_ERROR_IND;
	case LMI_STATS_IND:
		if (verbose > 1) {
			printf("                                             !stats ind             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case LMI_EVENT_IND:
		if (verbose > 1) {
			printf("                                             !event ind             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_CONN_IND:
		if (verbose > 1) {
			printf("                                             !conn ind              \n");
			FFLUSH(stdout);
		}
		seq[1] = p->npi.conn_ind.SEQ_number;
		return __TEST_UNKNOWN;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("                                             !conn con              \n");
			FFLUSH(stdout);
		}
		return __TEST_CONN_CON;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("                                             !discon ind            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("                                             !info ack              \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_BIND_ACK:
		if (verbose > 1) {
			printf("                                             !bind ack              \n");
			printf("                                                   cons = %lu\n", (ulong)p->npi.bind_ack.CONIND_number);
			printf("                                                   tok  = %lx\n", (ulong)p->npi.bind_ack.TOKEN_value);
			printf("                                                   alen = %lu\n", (ulong)p->npi.bind_ack.ADDR_length);
			if (p->npi.bind_ack.ADDR_length == 14) {
				addr_t *a = (addr_t *) (buf + p->npi.bind_ack.ADDR_offset);
				printf("                                                   port = %u\n", ntohs(a->port));
				printf("                                                   add1 = %d.%d.%d.%d\n", (a->addr[0] >> 0) & 0xff, (a->addr[0] >> 8) & 0xff, (a->addr[0] >> 16) & 0xff, (a->addr[0] >> 24) & 0xff);
				printf("                                                   add2 = %d.%d.%d.%d\n", (a->addr[1] >> 0) & 0xff, (a->addr[1] >> 8) & 0xff, (a->addr[1] >> 16) & 0xff, (a->addr[1] >> 24) & 0xff);
				printf("                                                   add3 = %d.%d.%d.%d\n", (a->addr[2] >> 0) & 0xff, (a->addr[2] >> 8) & 0xff, (a->addr[2] >> 16) & 0xff, (a->addr[2] >> 24) & 0xff);
			}
			FFLUSH(stdout);
		}
		tok[1] = p->npi.bind_ack.TOKEN_value;
		return __TEST_BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("                                             !error ack             \n");
			FFLUSH(stdout);
		}
		return __TEST_ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("                                             !ok ack                \n");
			FFLUSH(stdout);
		}
		return __TEST_OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("                                             !unitdata ind          \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("                                             !uderror ind           \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("                                             !datack ind            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_RESET_IND:
		if (verbose > 1) {
			printf("                                             !reset ind             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("                                             !reset con             \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	default:
		printf("                                             !(unknown %5ld)       \n", p->sl.sl_primitive);
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode data at Management Stream
 *
 *  -------------------------------------------------------------------------
 */
static int
mgm_decode_data(void)
{
	printf("                 !data |                                            \n");
	FFLUSH(stdout);
	return __TEST_UNKNOWN;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode ctrl at Management Stream
 *
 *  -------------------------------------------------------------------------
 */
static int
mgm_decode_msg(char *buf)
{
	union primitives *p = (union primitives *) buf;
	if (p->npi.type != oldmgm) {
		oldmgm = p->npi.type;
		cntmgm = 0;
	} else if (!expand)
		cntmgm++;
	switch (p->prim) {
	case N_CONN_IND:
		if (verbose > 1) {
			printf("             !conn ind |                                            \n");
			FFLUSH(stdout);
		}
		seq[2] = p->npi.conn_ind.SEQ_number;
		return __TEST_CONN_IND;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("             !conn con |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("           !discon ind |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_DISCON_IND;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("             !info ack |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_BIND_ACK:
		if (verbose > 1) {
			printf("             !bind ack |                                            \n");
			printf("    cons = %lu\n", (ulong)p->npi.bind_ack.CONIND_number);
			printf("    tok  = %lx\n", (ulong)p->npi.bind_ack.TOKEN_value);
			printf("    alen = %lu\n", (ulong)p->npi.bind_ack.ADDR_length);
			if (p->npi.bind_ack.ADDR_length == 14) {
				addr_t *a = (addr_t *) (buf + p->npi.bind_ack.ADDR_offset);
				printf("    port = %u\n", ntohs(a->port));
				printf("    add1 = %d.%d.%d.%d\n", (a->addr[0] >> 0) & 0xff, (a->addr[0] >> 8) & 0xff, (a->addr[0] >> 16) & 0xff, (a->addr[0] >> 24) & 0xff);
				printf("    add2 = %d.%d.%d.%d\n", (a->addr[1] >> 0) & 0xff, (a->addr[1] >> 8) & 0xff, (a->addr[1] >> 16) & 0xff, (a->addr[1] >> 24) & 0xff);
				printf("    add3 = %d.%d.%d.%d\n", (a->addr[2] >> 0) & 0xff, (a->addr[2] >> 8) & 0xff, (a->addr[2] >> 16) & 0xff, (a->addr[2] >> 24) & 0xff);
			}
			FFLUSH(stdout);
		}
		tok[2] = p->npi.bind_ack.TOKEN_value;
		return __TEST_BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("            !error ack |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("               !ok ack |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("         !unitdata ind |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("          !uderror ind |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("           !datack ind |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_RESET_IND:
		if (verbose > 1) {
			printf("            !reset ind |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("            !reset con |                                            \n");
			FFLUSH(stdout);
		}
		return __TEST_UNKNOWN;
	default:
		printf("      !(uninown %5ld) |                                            \n", (long)p->npi.type);
		FFLUSH(stdout);
		return __TEST_UNKNOWN;
	}
}

/*
 *  =========================================================================
 *
 *  EVENT HANDLING
 *
 *  =========================================================================
 */
static int show_timeout = 0;
/*
 *  -------------------------------------------------------------------------
 *
 *  Wait for message or timeout.
 *
 *  -------------------------------------------------------------------------
 */
static int
wait_event(int wait)
{
	while (1) {
		struct pollfd pfd[] = {
			{test_fd[0], POLLIN | POLLPRI, 0},
			{test_fd[1], POLLIN | POLLPRI, 0},
			{test_fd[2], POLLIN | POLLPRI, 0}
		};
		if (debug && verbose > 1) {
			float t, m;
			struct timeval now;
			static long reference = 0;
			gettimeofday(&now, NULL);
			if (!reference)
				reference = now.tv_sec;
			t = (now.tv_sec - reference);
			m = now.tv_usec;
			m = m / 1000000;
			t += m;
			printf("%11.6g\n", t);
			FFLUSH(stdout);
		}
		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				printf("                         -----timeout------                         \n");
				FFLUSH(stdout);
				if (show_timeout)
					show_timeout--;
			}
			return __EVENT_TIMEOUT;
		}
		if (debug && verbose > 1) {
			printf("                         ......polling.....                         \n");
			FFLUSH(stdout);
		}
		switch (poll(pfd, 3, wait)) {
		case -1:
			if (debug && verbose > 1) {
				printf("                         = = = ERROR = = =                          \n");
				FFLUSH(stdout);
			}
			break;
		case 0:
			if (debug && verbose > 1) {
				printf("                         + + +nothing + + +                         \n");
				FFLUSH(stdout);
			}
			return __EVENT_NO_MSG;
		case 1:
		case 2:
		case 3:
			if (debug && verbose > 1) {
				printf("                         ^^^^^^polled^^^^^^                         \n");
				FFLUSH(stdout);
			}
			if (pfd[0].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = __TEST_UNKNOWN;
				int flags = 0;
				char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, pt_buf};
				if (debug && verbose > 1) {
					printf("pt getmsg...                                                        \n");
					FFLUSH(stdout);
				}
				if (getmsg(test_fd[0], &ctrl, &data, &flags) < 0) {
					printf("****ERROR: pt getmsg failed\n");
					printf("           %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return __EVENT_NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("pt gotmsg... [%2d,%2d]                                                \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = pt_decode_msg(ctrl.buf)) != __TEST_UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = pt_decode_data()) != __TEST_UNKNOWN)
						return ret;
				}
			}
			if (pfd[1].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = __TEST_UNKNOWN;
				int flags = 0;
				char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, iut_buf};
				if (debug && verbose > 1) {
					printf("                                             iut getmsg             \n");
					FFLUSH(stdout);
				}
				if (getmsg(test_fd[1], &ctrl, &data, &flags) < 0) {
					printf("                                   ****ERROR: iut getmsg failed\n");
					printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return __EVENT_NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("                                             iut gotmsg [%2d,%2d]     \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = iut_decode_msg(ctrl.buf)) != __TEST_UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = iut_decode_data()) != __TEST_UNKNOWN)
						return ret;
				}
			}
			if (pfd[2].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = __TEST_UNKNOWN;
				int flags = 0;
				char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, mgm_buf};
				if (debug && verbose > 1) {
					printf("  mgm getmsg...                                                     \n");
					FFLUSH(stdout);
				}
				if (getmsg(test_fd[2], &ctrl, &data, &flags) < 0) {
					printf("  ****ERROR: mgm getmsg failed\n");
					printf("             %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return __EVENT_NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("  mgm gotmsg... [%2d,%2d]                                             \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = mgm_decode_msg(ctrl.buf)) != __TEST_UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = mgm_decode_data()) != __TEST_UNKNOWN)
						return ret;
				}
			}
		default:
			break;
		}
	}
}
static int
get_event(void)
{
	return wait_event(-1);
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

static const char sref_none[] = "(none)";

#if 0
static int
check_snibs(unsigned char bsnib, unsigned char fsnib)
{
	printf("                 check b/f sn/ib:  ---> (%02x/%02x)\n", bsnib, fsnib);
	FFLUSH(stdout);
	if ((bib[1] | bsn[1]) == bsnib && (fib[1] | fsn[1]) == fsnib)
		return __RESULT_SUCCESS;
	return __RESULT_FAILURE;
}
#endif

#define test_group_1 "Link State Control - Expected signal units/orders"
#define tgrp_case_1_1a test_group_1
#define sref_case_1_1a "Q.781/1.1"
#define numb_case_1_1a "1.1(a)"
#define name_case_1_1a "Initialization (power-up)"
#define desc_case_1_1a "\
Link State Control - Expected signal units/orders\n\
Initialization (Power-up)"
static int
test_case_1_1a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_POWER_ON);
			send(__TEST_OUT_OF_SERVICE);
			signal(__TEST_POWER_ON);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_1b test_group_1
#define sref_case_1_1b "Q.781/1.1"
#define numb_case_1_1b "1.1(b)"
#define name_case_1_1b "Initialization (power-up)"
#define desc_case_1_1b "\
Link State Control - Expected signal units/orders\n\
Initialization (Power-up)"
static int
test_case_1_1b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
			case __EVENT_NO_MSG:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_2 test_group_1
#define sref_case_1_2 "Q.781/1.2"
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Timer T2"
#define desc_case_1_2 "\
Link State Control - Expected signal units/orders\n\
Timer T2"
static int
test_case_1_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_ALIGNMENT:
				start_tt(iutconf.sl.t2 * 20);
				beg_time = milliseconds(t2);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T2  ", beg_time, timer[t2].lo, timer[t2].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_3 test_group_1
#define sref_case_1_3 "Q.781/1.3"
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Timer T3"
#define desc_case_1_3 "\
Link State Control - Expected signal units/orders\n\
Timer T3"
static int
test_case_1_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 2;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				state = 3;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				start_tt(iutconf.sl.t3 * 20);
				beg_time = milliseconds(t3);
				state = 4;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T3  ", beg_time, timer[t3].lo, timer[t3].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_4 test_group_1
#define sref_case_1_4 "Q.781/1.4"
#define numb_case_1_4 "1.4"
#define name_case_1_4 "Timer T1 & Timer T4 (Normal)"
#define desc_case_1_4 "\
Link State Control - Expected signal units/orders\n\
Timer T1 & Timer T4 (Normal)"
static int
test_case_1_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 2;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				beg_time = milliseconds(t4n);
				state = 3;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 4;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				if (check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
					return __RESULT_FAILURE;
				start_tt(iutconf.sl.t1 * 20);
				beg_time = milliseconds(t1);
				state = 5;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T1  ", beg_time, timer[t1].lo, timer[t1].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_5a "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\
"
static int
test_case_1_5a(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				expand = 1;
				return __RESULT_FAILURE;
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				start_tt(1000);
				state = 3;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_5b "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\
"
static int
test_case_1_5b(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				expand = 1;
				return __RESULT_FAILURE;
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				start_tt(1000);
				state = 3;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_5a_p test_group_1
#define sref_case_1_5a_p "Q.781/1.5"
#define numb_case_1_5a_p "1.5(a)"
#define name_case_1_5a_p "Normal alignment procedure"
#define desc_case_1_5a_p "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\n\
With proving\
"
static int
test_case_1_5a_p(void)
{
	return test_case_1_5a(1);
}

#define tgrp_case_1_5b_p test_group_1
#define sref_case_1_5b_p "Q.781/1.5"
#define numb_case_1_5b_p "1.5(b)"
#define name_case_1_5b_p "Normal alignment procedure"
#define desc_case_1_5b_p "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\n\
With proving\
"
static int
test_case_1_5b_p(void)
{
	return test_case_1_5b(1);
}

#define tgrp_case_1_5a_np test_group_1
#define sref_case_1_5a_np "Q781/1.5"
#define numb_case_1_5a_np "1.5(a)np"
#define name_case_1_5a_np "Normal alignment procedure"
#define desc_case_1_5a_np "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Forward direction\n\
Without proving\
"
static int
test_case_1_5a_np(void)
{
	return test_case_1_5a(0);
}

#define tgrp_case_1_5b_np test_group_1
#define sref_case_1_5b_np "Q.781/1.5"
#define numb_case_1_5b_np "1.5(b)np"
#define name_case_1_5b_np "Normal alignment procedure"
#define desc_case_1_5b_np "\
Link State Control = Expected signal units/orders\n\
Normal alignment procedure\n\
Reverse direction\n\
With outproving\
"
static int
test_case_1_5b_np(void)
{
	return test_case_1_5b(0);
}

#define desc_case_1_6 "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure - correct procedure (MSU)\
"
static int
test_case_1_6(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_DATA);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_ACK:
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_DATA:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_6_p test_group_1
#define sref_case_1_6_p "Q.781/1.6"
#define numb_case_1_6_p "1.6"
#define name_case_1_6_p "Correct alignment procedure (MSU)"
#define desc_case_1_6_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure = correct procedure (MSU)\n\
With proving\
"
static int
test_case_1_6_p(void)
{
	return test_case_1_6(1);
}

#define tgrp_case_1_6_np test_group_1
#define sref_case_1_6_np "Q.781/1.6"
#define numb_case_1_6_np "1.6np"
#define name_case_1_6_np "Correct alignment procedure (MSU)"
#define desc_case_1_6_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment procedure = correct procedure (MSU)\n\
Without proving\
"
static int
test_case_1_6_np(void)
{
	return test_case_1_6(0);
}

#define tgrp_case_1_7 test_group_1
#define sref_case_1_7 "Q.781/1.7"
#define numb_case_1_7 "1.7"
#define name_case_1_7 "SIO received during proving"
#define desc_case_1_7 "\
Link State Control - Expected signal units/orders\n\
SIO received during normal proving period\
"
static int
test_case_1_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 / 2);
				show_timeout = 1;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __EVENT_TIMEOUT:
				start_tt(iutconf.sl.t4n * 20);
				send(__TEST_ALIGNMENT);
				beg_time = milliseconds(t4n);
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_8a "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\
"
static int
test_case_1_8a(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_8b "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\
"
static int
test_case_1_8b(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_LPO);
				send(__TEST_START);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_PROCESSOR_OUTAGE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_8a_p test_group_1
#define sref_case_1_8a_p "Q.781/1.8"
#define numb_case_1_8a_p "1.8(a)"
#define name_case_1_8a_p "Normal alignment PO set (FISU)"
#define desc_case_1_8a_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\n\
With proving\
"
static int
test_case_1_8a_p(void)
{
	return test_case_1_8a(1);
}

#define tgrp_case_1_8b_p test_group_1
#define sref_case_1_8b_p "Q.781/1.8"
#define numb_case_1_8b_p "1.8(b)"
#define name_case_1_8b_p "Normal alignment PO set (FISU)"
#define desc_case_1_8b_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\n\
With proving\
"
static int
test_case_1_8b_p(void)
{
	return test_case_1_8b(1);
}

#define tgrp_case_1_8a_np test_group_1
#define sref_case_1_8a_np "Q.781/1.8"
#define numb_case_1_8a_np "1.8(a)np"
#define name_case_1_8a_np "Normal alignment PO set (FISU)"
#define desc_case_1_8a_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Forward direction\n\
Without proving\
"
static int
test_case_1_8a_np(void)
{
	return test_case_1_8a(0);
}

#define tgrp_case_1_8b_np test_group_1
#define sref_case_1_8b_np "Q.781/1.8"
#define numb_case_1_8b_np "1.8(b)np"
#define name_case_1_8b_np "Normal alignemnt PO set (FISU)"
#define desc_case_1_8b_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (FISU)\n\
Reverse direction\n\
Without proving\
"
static int
test_case_1_8b_np(void)
{
	return test_case_1_8b(0);
}

#define desc_case_1_9a "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\
"
static int
test_case_1_9a(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(1000);
				send(__TEST_DATA);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_9b "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\
"
static int
test_case_1_9b(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_LPO);
				send(__TEST_START);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				expand = 0;
				signal(__TEST_SEND_MSU);
				send(__TEST_PROVING_NORMAL);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				expand = 1;
				break;
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
			case __EVENT_TIMEOUT:
			case __TEST_IN_SERVICE:
			case __TEST_DATA:
				expand = 1;
				start_tt(1000);
				send(__TEST_PROCESSOR_OUTAGE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __TEST_DATA:
				send(__TEST_PROCESSOR_OUTAGE);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_9a_p test_group_1
#define sref_case_1_9a_p "Q.781/1.9"
#define numb_case_1_9a_p "1.9(a)"
#define name_case_1_9a_p "Normal alignment PO set (MSU)"
#define desc_case_1_9a_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\n\
With proving\
"
static int
test_case_1_9a_p(void)
{
	return test_case_1_9a(1);
}

#define tgrp_case_1_9b_p test_group_1
#define sref_case_1_9b_p "Q.781/1.9"
#define numb_case_1_9b_p "1.9(b)"
#define name_case_1_9b_p "Normal alignment PO set (MSU)"
#define desc_case_1_9b_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\n\
With proving\
"
static int
test_case_1_9b_p(void)
{
	return test_case_1_9b(1);
}

#define tgrp_case_1_9a_np test_group_1
#define sref_case_1_9a_np "Q.781/1.9"
#define numb_case_1_9a_np "1.9(a)np"
#define name_case_1_9a_np "Normal alignment PO set (MSU)"
#define desc_case_1_9a_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Forward direction\n\
Without proving\
"
static int
test_case_1_9a_np(void)
{
	return test_case_1_9a(0);
}

#define tgrp_case_1_9b_np test_group_1
#define sref_case_1_9b_np "Q.781/1.9"
#define numb_case_1_9b_np "1.9(b)np"
#define name_case_1_9b_np "Normal alignment PO set (MSU)"
#define desc_case_1_9b_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set (MSU)\n\
Reverse direction\n\
Without proving\
"
static int
test_case_1_9b_np(void)
{
	return test_case_1_9b(0);
}

#define desc_case_1_10 "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\
"
static int
test_case_1_10(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_LPR);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				start_tt(1000);
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_10_p test_group_1
#define sref_case_1_10_p "Q.781/1.10"
#define numb_case_1_10_p "1.10"
#define name_case_1_10_p "Normal alignment PO set and cleared"
#define desc_case_1_10_p "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\n\
With proving\
"
static int
test_case_1_10_p(void)
{
	return test_case_1_10(1);
}

#define tgrp_case_1_10_np test_group_1
#define sref_case_1_10_np "Q.781/1.10"
#define numb_case_1_10_np "1.10np"
#define name_case_1_10_np "Normal alignment PO set and cleared"
#define desc_case_1_10_np "\
Link State Control - Expected signal units/orders\n\
Normal alignment with PO set and cleared\n\
Without proving\
"
static int
test_case_1_10_np(void)
{
	return test_case_1_10(0);
}

#define desc_case_1_11 "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\
"
static int
test_case_1_11(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_LPO);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				send(__TEST_PROCESSOR_OUTAGE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_11_p test_group_1
#define sref_case_1_11_p "Q.781/1.11"
#define numb_case_1_11_p "1.11"
#define name_case_1_11_p "RPO set \"Aligned not ready\""
#define desc_case_1_11_p "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\n\
With proving\
"
static int
test_case_1_11_p(void)
{
	return test_case_1_11(1);
}

#define tgrp_case_1_11_np test_group_1
#define sref_case_1_11_np "Q.781/1.11"
#define numb_case_1_11_np "1.11np"
#define name_case_1_11_np "RPO set \"Aligned not ready\""
#define desc_case_1_11_np "\
Link State Control - Expected signal units/orders\n\
Set RPO when \"Aligned not ready\"\n\
Without proving\
"
static int
test_case_1_11_np(void)
{
	return test_case_1_11(0);
}

#define desc_case_1_12a "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\
"
static int
test_case_1_12a(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				send(__TEST_STOP);
				start_tt(1000);
				send(__TEST_OUT_OF_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_12a_p test_group_1
#define sref_case_1_12a_p "Q.781/1.12"
#define numb_case_1_12a_p "1.12(a)"
#define name_case_1_12a_p "SIOS received when \"Aligned not ready\""
#define desc_case_1_12a_p "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\n\
With proving\
"
static int
test_case_1_12a_p(void)
{
	return test_case_1_12a(1);
}

#define tgrp_case_1_12a_np test_group_1
#define sref_case_1_12a_np "Q.781/1.12"
#define numb_case_1_12a_np "1.12(a)np"
#define name_case_1_12a_np "SIOS received when \"Aligned not ready\""
#define desc_case_1_12a_np "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Forward direction\n\
Without proving\
"
static int
test_case_1_12a_np(void)
{
	return test_case_1_12a(0);
}

#define desc_case_1_12b "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\
"
static int
test_case_1_12b(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_LPO);
				send(__TEST_START);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				signal(__TEST_STOP);
				send(__TEST_IN_SERVICE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_12b_p test_group_1
#define sref_case_1_12b_p "Q.781/1.12"
#define numb_case_1_12b_p "1.12(b)"
#define name_case_1_12b_p "SIOS received when \"Aligned not ready\""
#define desc_case_1_12b_p "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\n\
With proving\
"
static int
test_case_1_12b_p(void)
{
	return test_case_1_12b(1);
}

#define tgrp_case_1_12b_np test_group_1
#define sref_case_1_12b_np "Q.781/1.12"
#define numb_case_1_12b_np "1.12(b)np"
#define name_case_1_12b_np "SIOS received when \"Aligned not ready\""
#define desc_case_1_12b_np "\
Link State Control - Expected signal units/orders\n\
SIOS received when \"Aligned not ready\"\n\
Reverse direction\n\
Without proving\
"
static int
test_case_1_12b_np(void)
{
	return test_case_1_12b(0);
}

#define desc_case_1_13 "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\
"
static int
test_case_1_13(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(1000);
				send(__TEST_ALIGNMENT);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_ALIGNMENT);
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_13_p test_group_1
#define sref_case_1_13_p "Q.781/1.13"
#define numb_case_1_13_p "1.13"
#define name_case_1_13_p "SIO received when \"Aligned not ready\""
#define desc_case_1_13_p "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\n\
With proving\
"
static int
test_case_1_13_p(void)
{
	return test_case_1_13(1);
}

#define tgrp_case_1_13_np test_group_1
#define sref_case_1_13_np "Q.781/1.13"
#define numb_case_1_13_np "1.13np"
#define name_case_1_13_np "SIO received when \"Aligned not ready\""
#define desc_case_1_13_np "\
Link State Control - Expected signal units/orders\n\
SIO received when \"Aligned not ready\"\n\
Without proving\
"
static int
test_case_1_13_np(void)
{
	return test_case_1_13(0);
}

#define tgrp_case_1_14 test_group_1
#define sref_case_1_14 "Q.781/1.14"
#define numb_case_1_14 "1.14"
#define name_case_1_14 "Set and clear LPO when \"Initial alignment\""
#define desc_case_1_14 "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Initial alignment\"\
"
static int
test_case_1_14(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				expand = 0;
				signal(__TEST_LPO);
				start_tt(iutconf.sl.t4n * 10 / 2);
				send(__TEST_PROVING_NORMAL);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __EVENT_TIMEOUT:
				expand = 1;
				signal(__TEST_LPR);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				break;
			case __EVENT_TIMEOUT:
				expand = 1;
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_15 "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\
"
static int
test_case_1_15(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				signal(__TEST_LPO);
				start_tt(1000);
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;
			case __TEST_PROCESSOR_OUTAGE:
				start_tt(5000);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __EVENT_TIMEOUT:
				signal(__TEST_LPR);
				start_tt(5000);
				state = 6;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_ENDED:
				send(__TEST_SEQUENCE_SYNC);
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_15_p test_group_1
#define sref_case_1_15_p "Q.781/1.15"
#define numb_case_1_15_p "1.15"
#define name_case_1_15_p "Set and clear LPO when \"Aligned ready\""
#define desc_case_1_15_p "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\n\
With proving\
"
static int
test_case_1_15_p(void)
{
	return test_case_1_15(1);
}

#define tgrp_case_1_15_np test_group_1
#define sref_case_1_15_np "Q.781/1.15"
#define numb_case_1_15_np "1.15np"
#define name_case_1_15_np "Set and clear LPO when \"Aligned ready\""
#define desc_case_1_15_np "\
Link State Control - Expected signal units/orders\n\
Set and clear LPO when \"Aligned ready\"\n\
Without proving\
"
static int
test_case_1_15_np(void)
{
	return test_case_1_15(0);
}

#define desc_case_1_16 "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\
"
static int
test_case_1_16(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(iutconf.sl.t1 * 20);
				beg_time = milliseconds(t1);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T1  ", beg_time, timer[t1].lo, timer[t1].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_16_p test_group_1
#define sref_case_1_16_p "Q.781/1.16"
#define numb_case_1_16_p "1.16"
#define name_case_1_16_p "Timer T1 in \"Aligned not ready\" state"
#define desc_case_1_16_p "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\n\
With proving\
"
static int
test_case_1_16_p(void)
{
	return test_case_1_16(1);
}

#define tgrp_case_1_16_np test_group_1
#define sref_case_1_16_np "Q.781/1.16"
#define numb_case_1_16_np "1.16np"
#define name_case_1_16_np "Timer T1 in \"Aligned not ready\" state"
#define desc_case_1_16_np "\
Link State Control - Expected signal units/orders\n\
Timer T1 in \"Aligned not ready\" state\n\
Without proving\
"
static int
test_case_1_16_np(void)
{
	return test_case_1_16(0);
}

#define tgrp_case_1_17 test_group_1
#define sref_case_1_17 "Q.781/1.17"
#define numb_case_1_17 "1.17"
#define name_case_1_17 "No SIO sent during normal proving period"
#define desc_case_1_17 "\
Link State Control - Expected signal units/orders\n\
No SIO sent during normal proving period\
"
static int
test_case_1_17(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				beg_time = dual_milliseconds(t3, t4n);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T3,4", beg_time, timer[t4n].lo, timer[t3].hi + timer[t4n].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_18 test_group_1
#define sref_case_1_18 "Q.781/1.18"
#define numb_case_1_18 "1.18"
#define name_case_1_18 "Set and cease emergency prior to \"start alignment\""
#define desc_case_1_18 "\
Link State Control - Expected signal units/orders\n\
Set and cease emergency prior to \"start alignment\"\
"
static int
test_case_1_18(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_EMERG);
				signal(__TEST_CEASE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				beg_time = milliseconds(t4n);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_19 test_group_1
#define sref_case_1_19 "Q.781/1.19"
#define numb_case_1_19 "1.19"
#define name_case_1_19 "Set emergency while in \"not aligned state\""
#define desc_case_1_19 "\
Link State Control - Expected signal units/orders\n\
Set emergency while in \"not aligned state\"\
"
static int
test_case_1_19(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				signal(__TEST_EMERG);
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4e * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_EMERG);
				beg_time = milliseconds(t4e);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_20 test_group_1
#define sref_case_1_20 "Q.781/1.20"
#define numb_case_1_20 "1.20"
#define name_case_1_20 "Set emergency when \"aligned\""
#define desc_case_1_20 "\
Link State Control - Expected signal units/orders\n\
Set emergency when \"aligned\"\
"
static int
test_case_1_20(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				signal(__TEST_EMERG);
				expand = 0;
				send(__TEST_ALIGNMENT);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				beg_time = milliseconds(t4n);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_ALIGNMENT);
				expand = 0;
				break;
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_NORMAL);
				beg_time = milliseconds(t4n);
				expand = 0;
				state = 4;
				break;
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_21 test_group_1
#define sref_case_1_21 "Q.781/1.21"
#define numb_case_1_21 "1.21"
#define name_case_1_21 "Both ends set emergency."
#define desc_case_1_21 "\
Link State Control - Expected signal units/orders\n\
Both ends set emergency.\
"
static int
test_case_1_21(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_EMERG);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4e * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_EMERG:
				beg_time = milliseconds(t4e);
				send(__TEST_PROVING_EMERG);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_22 test_group_1
#define sref_case_1_22 "Q.781/1.22"
#define numb_case_1_22 "1.22"
#define name_case_1_22 "Individual end sets emergency"
#define desc_case_1_22 "\
Link State Control - Expected signal units/orders\n\
Individual end sets emergency\
"
static int
test_case_1_22(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_EMERG);
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				signal(__TEST_START);
				start_tt(iutconf.sl.t4e * 20);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				beg_time = milliseconds(t4e);
				state = 3;
				expand = 0;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_23 test_group_1
#define sref_case_1_23 "Q.781/1.23"
#define numb_case_1_23 "1.23"
#define name_case_1_23 "Set emergency during normal proving"
#define desc_case_1_23 "\
Link State Control - Expected signal units/orders\n\
Set emergency during normal proving\
"
static int
test_case_1_23(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				signal(__TEST_EMERG);
				start_tt(iutconf.sl.t4e * 20);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_PROVING_EMERG:
				beg_time = milliseconds(t4e);
				state = 4;
				expand = 0;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_24 test_group_1
#define sref_case_1_24 "Q.781/1.24"
#define numb_case_1_24 "1.24"
#define name_case_1_24 "No SIO send during emergency alignment"
#define desc_case_1_24 "\
Link State Control - Expected signal units/orders\n\
No SIO send during emergency alignment\
"
static int
test_case_1_24(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_EMERG);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 20);
				expand = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_EMERG:
				beg_time = milliseconds(t4e);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_EMERG:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_25 test_group_1
#define sref_case_1_25 "Q.781/1.25"
#define numb_case_1_25 "1.25"
#define name_case_1_25 "Deactivation duing intial alignment"
#define desc_case_1_25 "\
Link State Control - Expected signal units/orders\n\
Deactivation duing intial alignment\
"
static int
test_case_1_25(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				show_timeout = 1;
				start_tt(iutconf.sl.t2 * 10 - 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				signal(__TEST_STOP);
				start_tt(1000);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_26 test_group_1
#define sref_case_1_26 "Q.781/1.26"
#define numb_case_1_26 "1.26"
#define name_case_1_26 "Deactivation during aligned state"
#define desc_case_1_26 "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned state\
"
static int
test_case_1_26(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t3 * 8);	/* 80% of T3 */
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				expand = 0;
				break;
			case __EVENT_TIMEOUT:
				expand = 1;
				signal(__TEST_STOP);
				start_tt(iutconf.sl.t4n * 10);
				state = 3;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define desc_case_1_27 "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\
"
static int
test_case_1_27(int proving)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				if (proving) {
					send(__TEST_PROVING);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#else
			case __TEST_PROVING_NORMAL:
				if (proving) {
					send(__TEST_PROVING_NORMAL);
					expand = 0;
					break;
				}
				return __RESULT_FAILURE;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				signal(__TEST_STOP);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_27_p test_group_1
#define sref_case_1_27_p "Q.781/1.27"
#define numb_case_1_27_p "1.27"
#define name_case_1_27_p "Deactivation during aligned not ready"
#define desc_case_1_27_p "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\n\
With proving\
"
static int
test_case_1_27_p(void)
{
	return test_case_1_27(1);
}

#define tgrp_case_1_27_np test_group_1
#define sref_case_1_27_np "Q.781/1.27"
#define numb_case_1_27_np "1.27np"
#define name_case_1_27_np "Deactivation during aligned not ready"
#define desc_case_1_27_np "\
Link State Control - Expected signal units/orders\n\
Deactivation during aligned not ready\n\
Without proving\
"
static int
test_case_1_27_np(void)
{
	return test_case_1_27(0);
}

#define tgrp_case_1_28 test_group_1
#define sref_case_1_28 "Q.781/1.28"
#define numb_case_1_28 "1.28"
#define name_case_1_28 "SIO received during link in service"
#define desc_case_1_28 "\
Link State Control - Expected signal units/orders\n\
SIO received during link in service\
"
static int
test_case_1_28(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_ALIGNMENT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_29a test_group_1
#define sref_case_1_29a "Q.781/1.29"
#define numb_case_1_29a "1.29(a)"
#define name_case_1_29a "SIO received during link in service"
#define desc_case_1_29a "\
Link State Control - Expected signal units/orders\n\
SIO received during link in service\n\
Forward direction\
"
static int
test_case_1_29a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_STOP);
			send(__TEST_OUT_OF_SERVICE);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_29b test_group_1
#define sref_case_1_29b "Q.781/1.29"
#define numb_case_1_29b "1.29(b)"
#define name_case_1_29b "SIO received during link in service"
#define desc_case_1_29b "\
Link State Control - Expected signal units/orders\n\
SIO received during link in service\n\
Reverse direction\
"
static int
test_case_1_29b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_STOP);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_30a test_group_1
#define sref_case_1_30a "Q.781/1.30"
#define numb_case_1_30a "1.30(a)"
#define name_case_1_30a "Deactivation during LPO"
#define desc_case_1_30a "\
Link State Control - Expected signal units/orders\n\
Deactivation during LPO\n\
Forward direction\
"
static int
test_case_1_30a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_PROCESSOR_OUTAGE:
				signal(__TEST_STOP);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_30b test_group_1
#define sref_case_1_30b "Q.781/1.30"
#define numb_case_1_30b "1.30(b)"
#define name_case_1_30b "Deactivation during LPO"
#define desc_case_1_30b "\
Link State Control - Expected signal units/orders\n\
Deactivation during LPO\n\
Reverse direction\
"
static int
test_case_1_30b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_STOP);
				send(__TEST_OUT_OF_SERVICE);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(SHORT_WAIT))) {
			case __TEST_IUT_RPO:
				break;	/* stupid ITU-T SDLs */
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_31a test_group_1
#define sref_case_1_31a "Q.781/1.31"
#define numb_case_1_31a "1.31(a)"
#define name_case_1_31a "Deactivation during RPO"
#define desc_case_1_31a "\
Link State Control - Expected signal units/orders\n\
Deactivation during RPO\n\
Forward direction\
"
static int
test_case_1_31a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_PROCESSOR_OUTAGE);
			signal(__TEST_STOP);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_31b test_group_1
#define sref_case_1_31b "Q.781/1.31"
#define numb_case_1_31b "1.31(b)"
#define name_case_1_31b "Deactivation during RPO"
#define desc_case_1_31b "\
Link State Control - Expected signal units/orders\n\
Deactivation during RPO\n\
Reverse direction\
"
static int
test_case_1_31b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_STOP);
				send(__TEST_OUT_OF_SERVICE);
				return __RESULT_SUCCESS;
			case __TEST_IUT_RPR:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_32a test_group_1
#define sref_case_1_32a "Q.781/1.32"
#define numb_case_1_32a "1.32(a)"
#define name_case_1_32a "Deactivation during the proving period"
#define desc_case_1_32a "\
Link State Control - Expected signal units/orders\n\
Deactivation during the proving period\n\
Forward direction\
"
static int
test_case_1_32a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __EVENT_TIMEOUT:
				expand = 1;
				send(__TEST_STOP);
				send(__TEST_OUT_OF_SERVICE);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
#else
			case __TEST_PROVING_NORMAL:
#endif
				expand = 0;
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __TEST_OUT_OF_SERVICE:
				expand = 1;
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				expand = 1;
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_32b test_group_1
#define sref_case_1_32b "Q.781/1.32"
#define numb_case_1_32b "1.32(b)"
#define name_case_1_32b "Deactivation during the proving period"
#define desc_case_1_32b "\
Link State Control - Expected signal units/orders\n\
Deactivation during the proving period\n\
Reverse direction\
"
static int
test_case_1_32b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_OUT_OF_SERVICE);
			send(__TEST_START);
			signal(__TEST_START);
			send(__TEST_ALIGNMENT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_ALIGNMENT:
				send(__TEST_PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 20);
				expand = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __EVENT_TIMEOUT:
				expand = 1;
				signal(__TEST_STOP);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_OUT_OF_SERVICE:
				expand = 1;
				send(__TEST_OUT_OF_SERVICE);
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_33 test_group_1
#define sref_case_1_33 "Q.781/1.33"
#define numb_case_1_33 "1.33"
#define name_case_1_33 "SIO received instead of FISUs"
#define desc_case_1_33 "\
Link State Control - Expected signal units/orders\n\
SIO received instead of FISUs\
"
static int
test_case_1_33(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_ALIGNMENT);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_34 test_group_1
#define sref_case_1_34 "Q.781/1.34"
#define numb_case_1_34 "1.34"
#define name_case_1_34 "SIO received instead of FISUs"
#define desc_case_1_34 "\
Link State Control - Expected signal units/orders\n\
SIO received instead of FISUs\
"
static int
test_case_1_34(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_STOP);
				send(__TEST_OUT_OF_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_1_35 test_group_1
#define sref_case_1_35 "Q.781/1.35"
#define numb_case_1_35 "1.35"
#define name_case_1_35 "SIPO received instead of FISUs"
#define desc_case_1_35 "\
Link State Control - Expected signal units/orders\n\
SIPO received instead of FISUs\
"
static int
test_case_1_35(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_LPO);
				send(__TEST_PROCESSOR_OUTAGE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define test_group_2 "Link State Control - Unexpected signal units/orders"
#define tgrp_case_2_1 test_group_2
#define sref_case_2_1 "Q.781/2.1"
#define numb_case_2_1 "2.1"
#define name_case_2_1 "Unexpected signal units/orders in \"Out of service\" state"
#define desc_case_2_1 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Out of service\" state\
"
static int
test_case_2_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_ALIGNMENT);
				send(__TEST_PROVING_NORMAL);
				send(__TEST_PROVING_EMERG);
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				send(__TEST_PROCESSOR_ENDED);
				send(__TEST_IN_SERVICE);
				send(__TEST_BUSY_ENDED);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(__TEST_ACK);
#endif
				send(__TEST_DATA);
				signal(__TEST_STOP);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_2 test_group_2
#define sref_case_2_2 "Q.781/2.2"
#define numb_case_2_2 "2.2"
#define name_case_2_2 "Unexpected signal units/orders in \"Not Aligned\" state"
#define desc_case_2_2 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Not Aligned\" state\
"
static int
test_case_2_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				oldmsg = 0;
				cntmsg = 0;	/* force display __TEST_OUT_OF_SERVICE */
				send(__TEST_OUT_OF_SERVICE);
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				send(__TEST_IN_SERVICE);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(__TEST_ACK);
#endif
				send(__TEST_DATA);
				signal(__TEST_CEASE);
				signal(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_3 test_group_2
#define sref_case_2_3 "Q.781/2.3"
#define numb_case_2_3 "2.3"
#define name_case_2_3 "Unexpected signal units/orders in \"Aligned\" state"
#define desc_case_2_3 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned\" state\
"
static int
test_case_2_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				oldmsg = 0;
				cntmsg = 0;	/* force display __TEST_ALIGNMENT */
				send(__TEST_ALIGNMENT);
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				send(__TEST_IN_SERVICE);
				send(__TEST_PROCESSOR_ENDED);
				send(__TEST_BUSY_ENDED);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(__TEST_ACK);
#endif
				send(__TEST_DATA);
				signal(__TEST_CEASE);
				signal(__TEST_START);
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_4 test_group_2
#define sref_case_2_4 "Q.781/2.4"
#define numb_case_2_4 "2.4"
#define name_case_2_4 "Unexpected signal units/orders in \"Proving\" state"
#define desc_case_2_4 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Proving\" state\
"
static int
test_case_2_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				send(__TEST_PROCESSOR_ENDED);
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_BUSY_ENDED);
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				send(__TEST_IN_SERVICE);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(__TEST_ACK);
#endif
				send(__TEST_DATA);
				signal(__TEST_CEASE);
				signal(__TEST_START);
				state = 3;
				expand = 0;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_5 test_group_2
#define sref_case_2_5 "Q.781/2.5"
#define numb_case_2_5 "2.5"
#define name_case_2_5 "Unexpected signal units/orders in \"Aligned Ready\" state"
#define desc_case_2_5 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned Ready\" state\
"
static int
test_case_2_5(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				signal(__TEST_EMERG);
				signal(__TEST_CEASE);
				signal(__TEST_LPR);
				signal(__TEST_START);
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_6 test_group_2
#define sref_case_2_6 "Q.781/2.6"
#define numb_case_2_6 "2.6"
#define name_case_2_6 "Unexpected signal units/orders in \"Aligned Not Ready\" state"
#define desc_case_2_6 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Aligned Not Ready\" state\
"
static int
test_case_2_6(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				signal(__TEST_EMERG);
				signal(__TEST_CEASE);
				// signal(__TEST_LPR); // spec says __TEST_LPR, but it doesn't make sense...
				signal(__TEST_LPO);
				signal(__TEST_START);
				send(__TEST_IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
			case __TEST_IUT_RPR:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_7 test_group_2
#define sref_case_2_7 "Q.781/2.7"
#define numb_case_2_7 "2.7"
#define name_case_2_7 "Unexpected signal units/orders in \"In Service\" state"
#define desc_case_2_7 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"In Service\" state\
"
static int
test_case_2_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_INVALID_STATUS);
			signal(__TEST_EMERG);
			signal(__TEST_CEASE);
			signal(__TEST_LPR);
			signal(__TEST_START);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_2_8 test_group_2
#define sref_case_2_8 "Q.781/2.8"
#define numb_case_2_8 "2.8"
#define name_case_2_8 "Unexpected signal units/orders in \"Processor Outage\" state"
#define desc_case_2_8 "\
Link State Control - Unexpected signal units/orders\n\
Unexpected signal units/orders in \"Processor Outage\" state\
"
static int
test_case_2_8(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_BUSY);
				send(__TEST_INVALID_STATUS);
				signal(__TEST_EMERG);
				signal(__TEST_CEASE);
				signal(__TEST_START);
				send(__TEST_IN_SERVICE);
				send(__TEST_PROCESSOR_ENDED);
				send(__TEST_BUSY_ENDED);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#deifne test_group_3 "Transmission Failure"
#define tgrp_case_3_1 test_group_3
#define sref_case_3_1 "Q.781/3.1"
#define numb_case_3_1 "3.1"
#define name_case_3_1 "Link aligned ready (Break Tx path)"
#define desc_case_3_1 "\
Transmission Failure\n\
Link aligned ready (Break Tx path)\
"
static int
test_case_3_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				if (send(__TEST_TX_BREAK))
					return __RESULT_INCONCLUSIVE;
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OK_ACK:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:	/* FIXME */
				send(__TEST_TX_MAKE);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TX_MAKE);
				return __RESULT_FAILURE;
			}
			break;
		default:
			send(__TEST_TX_MAKE);
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_2 test_group_3
#define sref_case_3_2 "Q.781/3.2"
#define numb_case_3_2 "3.2"
#define name_case_3_2 "Link aligned ready (Corrupt FIBs - Basic)"
#define desc_case_3_2 "\
Transmission Failure\n\
Link aligned ready (Corrupt FIBs - Basic)\
"
static int
test_case_3_2(void)
{
#if defined M2PA_VERSION_DRAFT11
	return __RESULT_NOTAPPL;
#endif
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_BAD_ACK);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(__TEST_BAD_ACK);
				start_tt(1000);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_3 test_group_3
#define sref_case_3_3 "Q.781/3.3"
#define numb_case_3_3 "3.3"
#define name_case_3_3 "Link aligned not ready (Break Tx path)"
#define desc_case_3_3 "\
Transmission Failure\n\
Link aligned not ready (Break Tx path)\
"
static int
test_case_3_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				if (send(__TEST_TX_BREAK))
					return __RESULT_INCONCLUSIVE;
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_OK_ACK:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:	/* FIXME */
				send(__TEST_TX_MAKE);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TX_MAKE);
				return __RESULT_FAILURE;
			}
			break;
		default:
			send(__TEST_TX_MAKE);
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_4 test_group_3
#define sref_case_3_4 "Q.781/3.4"
#define numb_case_3_4 "3.4"
#define name_case_3_4 "Link aligned not ready (Corrupt FIBs - Basic)"
#define desc_case_3_4 "\
Transmission Failure\n\
Link aligned not ready (Corrupt FIBs - Basic)\
"
static int
test_case_3_4(void)
{
#if defined M2PA_VERSION_DRAFT11
	return __RESULT_NOTAPPL;
#endif
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_LPO);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case __TEST_PROCESSOR_OUTAGE:
				expand = 1;
				send(__TEST_BAD_ACK);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(__TEST_BAD_ACK);
				start_tt(1000);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_5 test_group_3
#define numb_case_3_5 "3.5"
#define name_case_3_5 "Link in service (Break Tx path)"
#define desc_case_3_5 "\
Transmission Failure\n\
Link in service (Break Tx path)\
"
static int
test_case_3_5(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_TX_BREAK))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OK_ACK:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:	/* FIXME */
				send(__TEST_TX_MAKE);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TX_MAKE);
				return __RESULT_FAILURE;
			}
			break;
		default:
			send(__TEST_TX_MAKE);
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_6 test_group_3
#define sref_case_3_6 "Q.781/3.6"
#define numb_case_3_6 "3.6"
#define name_case_3_6 "Link in service (Corrupt FIBs - Basic)"
#define desc_case_3_6 "\
Transmission Failure\n\
Link in service (Corrupt FIBs - Basic)\
"
static int
test_case_3_6(void)
{
#if defined M2PA_VERSION_DRAFT11
	return __RESULT_NOTAPPL;
#endif
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_BAD_ACK);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(__TEST_BAD_ACK);
				start_tt(1000);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_7 test_group_3
#define sref_case_3_7 "Q.781/3.7"
#define numb_case_3_7 "3.7"
#define name_case_3_7 "Link in processor outage (Break Tx path)"
#define desc_case_3_7 "\
Transmission Failure\n\
Link in processor outage (Break Tx path)\
"
static int
test_case_3_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_PROCESSOR_OUTAGE:
				if (send(__TEST_TX_BREAK))
					return __RESULT_INCONCLUSIVE;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OK_ACK:
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_IUT_OUT_OF_SERVICE:	/* FIXME */
				send(__TEST_TX_MAKE);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TX_MAKE);
				return __RESULT_FAILURE;
			}
			break;
		default:
			send(__TEST_TX_MAKE);
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_3_8 test_group_3
#define sref_case_3_8 "Q.781/3.8"
#define numb_case_3_8 "3.8"
#define name_case_3_8 "Link in processor outage (Corrupt FIBs - Basic)"
#define desc_case_3_8 "\
Transmission Failure\n\
Link in processor outage (Corrupt FIBs - Basic)\
"
static int
test_case_3_8(void)
{
#if defined M2PA_VERSION_DRAFT11
	return __RESULT_NOTAPPL;
#endif
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_BAD_ACK);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __EVENT_NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(__TEST_BAD_ACK);
				start_tt(1000);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define test_group_4 "Processor Outage Control"
#define tgrp_case_4_1a test_group_4
#define sref_case_4_1a "Q.781/4.1"
#define numb_case_4_1a "4.1(a)"
#define name_case_4_1a "Set and clear LPO while link in service"
#define desc_case_4_1a "\
Processor Outage Control\n\
Set and clear LPO while link in service\n\
Forward direction\
"
#ifdef M2PA_VERSION_DRAFT11
static int
test_case_4_1a(void)
{
	int dat = 0, msu = 0, ind = 0;
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			send(__TEST_DATA);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (++msu == 1) {
					signal(__TEST_LPO);
					send(__TEST_DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				break;
			case __TEST_ACK:
				if (++dat <= 1)
					break;
				return __RESULT_FAILURE;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (++msu == 2) {
					bsn[0]++;
					send(__TEST_ACK);
					state = 3;
					break;
				}
				break;
			case __TEST_ACK:
				if (++dat <= 1)
					break;
				return __RESULT_FAILURE;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_DATA);
				start_tt(1000);
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				if (++ind <= dat)
					break;
				return (__RESULT_FAILURE);
			case __EVENT_TIMEOUT:
				signal(__TEST_CLEARB);
				signal(__TEST_LPR);
				signal(__TEST_SEND_MSU);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_ENDED:
				send(__TEST_DATA);
				fsn[0] = bsn[1];
				send(__TEST_SEQUENCE_SYNC);
				send(__TEST_DATA);
				start_tt(1000);
				state = 6;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				break;
			case __TEST_IUT_DATA:
				break;
			case __TEST_ACK:
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 9:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}
#else
static int
test_case_4_1a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_DATA:
				if (++count == 2) {
					signal(__TEST_LPO);
					send(__TEST_DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				break;
			case __TEST_ACK:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_ACK:
				break;
			case __TEST_PROCESSOR_OUTAGE:
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;
			case __EVENT_TIMEOUT:
				signal(__TEST_CLEARB);
#if !defined M2PA_VERSION_DRAFT11
				signal(__TEST_SEND_MSU);
#endif
				start_tt(1000);
				signal(__TEST_LPR);
#if defined M2PA_VERSION_DRAFT11
				signal(__TEST_SEND_MSU);
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				state = 5;
#else
				state = 4;
#endif
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;
			case __TEST_DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_ENDED:
				send(__TEST_SEQUENCE_SYNC);
#if defined M2PA_VERSION_DRAFT11
				send(__TEST_DATA);
				signal(__TEST_SEND_MSU);
				state = 6;
				break;
#endif
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
#if defined M2PA_VERSION_DRAFT11
		case 6:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				state = 7;
				break;
			case __TEST_IUT_DATA:
				state = 8;
				break;
			case __TEST_ACK:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				return __RESULT_SUCCESS;
			case __TEST_ACK:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
#endif
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}
#endif

#define tgrp_case_4_1b test_group_4
#define sref_case_4_1b "Q.781/4.1"
#define numb_case_4_1b "4.1(b)"
#define name_case_4_1b "Set and clear LPO while link in service"
#define desc_case_4_1b "\
Processor Outage Control\n\
Set and clear LPO while link in service\n\
Reverse direction\
"
#ifdef M2PA_VERSION_DRAFT11
static int
test_case_4_1b(void)
{
	int dat = 0, msu = 0;
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			send(__TEST_DATA);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (++msu == 2) {
					bsn[0]++;
					send(__TEST_ACK);
					send(__TEST_PROCESSOR_OUTAGE);
					send(__TEST_DATA);
					signal(__TEST_SEND_MSU);
					start_tt(iutconf.sl.t7 * 10);
					state = 2;
					break;
				}
				break;
			case __TEST_ACK:
				if (++dat <= 1)
					break;
				return __RESULT_FAILURE;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				send(__TEST_PROCESSOR_ENDED);
				send(__TEST_DATA);
				start_tt(iutconf.sl.t7 * 10 / 2);
				state = 3;
				break;
			case __TEST_ACK:
				break;
			case __TEST_IUT_DATA:
				break;
			case __TEST_IUT_RPO:
				break;
			case __TEST_DATA:
				start_tt(iutconf.sl.t7 * 10);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			case __TEST_DATA:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				break;
			case __TEST_ACK:
				break;
			case __TEST_IUT_DATA:
				break;
			case __TEST_IUT_RPR:
				break;
			case __TEST_IN_SERVICE:
				fsn[0] = bsn[1];
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 9:
			switch ((event = get_event())) {
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}
#else
static int
test_case_4_1b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_DATA:
				if (++count == 2) {
					signal(__TEST_LPO);
					send(__TEST_DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				break;
			case __TEST_ACK:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_ACK:
				break;
			case __TEST_PROCESSOR_OUTAGE:
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;
			case __EVENT_TIMEOUT:
				signal(__TEST_CLEARB);
				signal(__TEST_SEND_MSU);
				start_tt(1000);
				signal(__TEST_LPR);
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;
			case __TEST_DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_ENDED:
				send(__TEST_SEQUENCE_SYNC);
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}
#endif

#define tgrp_case_4_2 test_group_4
#define sref_case_4_2 "Q.781/4.2"
#define numb_case_4_2 "4.2"
#define name_case_4_2 "RPO during LPO"
#define desc_case_4_2 "\
Processor Outage Control\n\
RPO during LPO\
"
static int
test_case_4_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case __TEST_PROCESSOR_OUTAGE:
				send(__TEST_PROCESSOR_OUTAGE);
				send(__TEST_PROCESSOR_ENDED);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case __TEST_IUT_RPR:
			case __TEST_IUT_RPO:
				break;
			case __TEST_IN_SERVICE:
			case __EVENT_NO_MSG:
				send(__TEST_LPR);
				send(__TEST_PROCESSOR_ENDED);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IN_SERVICE:
			case __TEST_IUT_RPO:
				break;
			case __TEST_IUT_RPR:
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_4_3 test_group_4
#define sref_case_4_3 "Q.781/4.3"
#define numb_case_4_3 "4.3"
#define name_case_4_3 "Clear LPO when \"Both processor outage\""
#define desc_case_4_3 "\
Processor Outage Control\n\
Clear LPO when \"Both processor outage\"\
"
static int
test_case_4_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_LPO);
			send(__TEST_LPO);
			send(__TEST_PROCESSOR_OUTAGE);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __TEST_PROCESSOR_OUTAGE:
				signal(__TEST_LPR);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				send(__TEST_LPR);
				send(__TEST_PROCESSOR_ENDED);
				state = 3;
				break;
			case __TEST_PROCESSOR_ENDED:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IN_SERVICE:
				break;
			case __TEST_IUT_RPR:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define test_group_5 "SU delimitation, alignment, error detection and correction"
#define tgrp_case_5_1 test_group_5
#define sref_case_5_1 "Q.781/5.1"
#define numb_case_5_1 "5.1"
#define name_case_5_1 "More than 7 ones between MSU opening and closing flags"
#define desc_case_5_1 "\
SU delimitation, alignment, error detection and correction\n\
More than 7 ones between MSU opening and closing flags\
"
static int
test_case_5_1(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	unsigned char old_bsn = 0x7f;
	for (;;) {
		switch (state) {
		case 0:
			old_bsn = bsn[1];
			if (send(__TEST_MSU_SEVEN_ONES))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_FISU:
				if (bsn[1] != old_bsn)
					return __RESULT_FAILURE;
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			case __TEST_IUT_DATA:
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_5_2 test_group_5
#define sref_case_5_2 "Q.781/5.2"
#define numb_case_5_2 "5.2"
#define name_case_5_2 "Greater than maximum signal unit length"
#define desc_case_5_2 "\
SU delimitation, alignment, error detection and correction\n\
Greater than maximum signal unit length\
"
static int
test_case_5_2(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_MSU_TOO_LONG);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_FISU:
				if (bsn[1] != 0x7f)
					return check_snibs(0x7f, 0xff);
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return check_snibs(0x7f, 0xff);
			case __TEST_IUT_DATA:
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_5_3 test_group_5
#define sref_case_5_3 "Q.781/5.3"
#define numb_case_5_3 "5.3"
#define name_case_5_3 "Below minimum signal unit length"
#define desc_case_5_3 "\
SU delimitation, alignment, error detection and correction\n\
Below minimum signal unit length\
"
static int
test_case_5_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_MSU_TOO_SHORT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			case __TEST_ACK:
			case __TEST_IUT_DATA:
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_5_4a test_group_5
#define sref_case_5_4a "Q.781/5.4"
#define numb_case_5_4a "5.4(a)"
#define name_case_5_4a "Reception of single and multiple flags between FISUs"
#define desc_case_5_4a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Forward direction\
"
static int
test_case_5_4a(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_FISU_FISU_1FLAG))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_5_4b test_group_5
#define sref_case_5_4b "Q.781/5.4"
#define numb_case_5_4b "5.4(b)"
#define name_case_5_4b "Reception of single and multiple flags between FISUs"
#define desc_case_5_4b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Reverse direction\
"
static int
test_case_5_4b(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_FISU_FISU_2FLAG))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_5_5a test_group_5
#define sref_case_5_5a "Q.781/5.5"
#define numb_case_5_5a "5.5(a)"
#define name_case_5_5a "Reception of single and multiple flags between MSUs"
#define desc_case_5_5a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Forward direction\
"
static int
test_case_5_5a(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_MSU_MSU_1FLAG))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_5_5b test_group_5
#define sref_case_5_5b "Q.781/5.5"
#define numb_case_5_5b "5.5(b)"
#define name_case_5_5b "Reception of single and multiple flags between MSUs"
#define desc_case_5_5b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Reverse direction\
"
static int
test_case_5_5b(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_MSU_MSU_2FLAG))
				return __RESULT_INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define test_group_6 "SUERM check"
#define tgrp_case_6_1 test_group_6
#define sref_case_6_1 "Q.781/6.1"
#define numb_case_6_1 "6.1"
#define name_case_6_1 "Error rate of 1 in 256 - Link remains in service"
#define desc_case_6_1 "\
SUERM check\n\
Error rate of 1 in 256 - Link remains in service\
"
static int
test_case_6_1(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_FISU_CORRUPT);
			stop_tt();
			count = 255;
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (tries < 8192) {
					int i;
					if (tries) {
						send(__TEST_FISU_CORRUPT_S);
						for (i = 0; i < count; i++)
							send(__TEST_FISU_S);
					} else {
						send(__TEST_FISU_CORRUPT);
						send(__TEST_FISU);
						for (i = 1; i < count; i++)
							send(__TEST_FISU_S);
						send(__TEST_COUNT);
						send(__TEST_ETC);
					}
					tries++;
					break;
				}
				send(__TEST_TRIES);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TRIES);
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_6_2 test_group_6
#define sref_case_6_2 "Q.781/6.2"
#define numb_case_6_2 "6.2"
#define name_case_6_2 "Error rate of 1 in 254 - Link out of service"
#define desc_case_6_2 "\
SUERM check\n\
Error rate of 1 in 254 - Link out of service\
"
static int
test_case_6_2(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			stop_tt();
			count = 253;
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				if (tries < 8192) {
					int i;
					if (tries) {
						send(__TEST_FISU_CORRUPT_S);
						for (i = 0; i < count; i++)
							send(__TEST_FISU_S);
					} else {
						send(__TEST_FISU_CORRUPT);
						send(__TEST_FISU);
						for (i = 1; i < count; i++)
							send(__TEST_FISU_S);
						send(__TEST_COUNT);
						send(__TEST_ETC);
					}
					tries++;
					break;
				}
				send(__TEST_TRIES);
				return __RESULT_FAILURE;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_TRIES);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TRIES);
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_6_3 test_group_6
#define sref_case_6_3 "Q.781/6.3"
#define numb_case_6_3 "6.3"
#define name_case_6_3 "Consequtive corrupt SUs"
#define desc_case_6_3 "\
SUERM check\n\
Consequtive corrupt SUs\
"
static int
test_case_6_3(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			count++;
			send(__TEST_FISU_CORRUPT);
			stop_tt();
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				count++;
				send(__TEST_FISU_CORRUPT_S);
				if (count > 128) {
					send(__TEST_COUNT);
					return __RESULT_FAILURE;
				}
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				if (count > 1)
					send(__TEST_COUNT);
				if (count > 70)
					return __RESULT_FAILURE;
				else
					return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_6_4 test_group_6
#define sref_case_6_4 "Q.781/6.4"
#define numb_case_6_4 "6.4"
#define name_case_6_4 "Time controlled break of the link"
#define desc_case_6_4 "\
SUERM check\n\
Time controlled break of the link\
"
static int
test_case_6_4(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(__TEST_TX_BREAK))
				return __RESULT_INCONCLUSIVE;
			start_tt(100);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				send(__TEST_TX_MAKE);
				start_tt(1000);
				state = 2;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_IN_SERVICE:
				break;
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				send(__TEST_TX_MAKE);
				return __RESULT_SUCCESS;
			default:
				send(__TEST_TX_MAKE);
				return __RESULT_FAILURE;
			}
			break;
		default:
			send(__TEST_TX_MAKE);
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define test_group_7 "AERM check"
#define tgrp_case_7_1 test_group_7
#define sref_case_7_1 "Q.781/7.1"
#define numb_case_7_1 "7.1"
#define name_case_7_1 "Error rate below the normal threshold"
#define desc_case_7_1 "\
AERM check\n\
Error rate below the normal threshold\
"
static int
test_case_7_1(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				break;
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				beg_time = milliseconds(t4);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __EVENT_TIMEOUT:
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
			case __TEST_PROVING_NORMAL:
				if (count < iutconf.sdt.Tin - 1) {
					send(__TEST_INVALID_STATUS);
					count++;
				} else {
					send(__TEST_COUNT);
					send(__TEST_PROVING_NORMAL);
					start_tt(iutconf.sl.t4n * 10 / 2 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __TEST_FISU:
				expand = 1;
				send(__TEST_FISU);
				return __RESULT_SUCCESS;
			case __EVENT_TIMEOUT:
				expand = 1;
				return __RESULT_FAILURE;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_7_2 test_group_7
#define sref_case_7_2 "Q.781/7.2"
#define numb_case_7_2 "7.2"
#define name_case_7_2 "Error rate at the normal threshold"
#define desc_case_7_2 "\
AERM check\n\
Error rate at the normal threshold\
"
static int
test_case_7_2(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				break;
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __EVENT_TIMEOUT:
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((even = wait_event(0))) {
			case __EVENT_NO_MSG:
			case __TEST_PROVING_NORMAL:
				if (count < iutconf.sdt.Tin) {
					send(__TEST_INVALID_STATUS);
					count++;
				} else {
					send(__TEST_COUNT);
					send(__TEST_PROVING_NORMAL);
					beg_time = milliseconds(t4);
					start_tt(iutconf.sl.t4n * 10 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __TEST_IUT_IN_SERVICE:
				expand = 1;
				break;
			case __TEST_FISU:
				expand = 1;
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_7_3 test_group_7
#define sref_case_7_3 "Q.781/7.3"
#define numb_case_7_3 "7.3"
#define name_case_7_3 "Error rate above the normal threshold"
#define desc_case_7_3 "\
AERM check\n\
Error rate above the normal threshold\
"
static int
test_case_7_3(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				break;
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				tries = 1;
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __EVENT_TIMEOUT:
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((even = wait_event(0))) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_COUNT);
				if (tries == iutconf.sl.M)
					return __RESULT_SUCCESS;
				return __RESULT_FAILURE;
			case __EVENT_NO_MSG:
			case __TEST_PROVING_NORMAL:
				if (count <= iutconf.sdt.Tin) {
					send(__TEST_INVALID_STATUS);
					count++;
				} else {
					send(__TEST_COUNT);
					count = 0;
					send(__TEST_PROVING_NORMAL);
					if (tries < iutconf.sl.M) {
						start_tt(iutconf.sl.t4n * 10 / 2);
						state = 3;
						tries++;
					} else {
						start_tt(iutconf.sl.t4n * 10 + 200);
						state = 5;
					}
					expand = 0;
				}
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_NORMAL);
				expand = 0;
				break;
			case __TEST_OUT_OF_SERVICE:
				expand = 1;
				break;
			case __TEST_OUT_OF_SERVICE:
				expand = 1;
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_7_4 test_group_7
#define sref_case_7_4 "Q.781/7.4"
#define numb_case_7_4 "7.4"
#define name_case_7_4 "Error rate at the emergency threshold"
#define desc_case_7_4 "\
AERM check\n\
Error rate at the emergency threshold\
"
static int
test_case_7_4(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_OUT_OF_SERVICE);
				break;
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_ALIGNMENT);
				break;
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
			case __EVENT_TIMEOUT:
				state = 4;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case __EVENT_NO_MSG:
			case __TEST_PROVING_NORMAL:
				if (count < iutconf.sdt.Tie) {
					send(__TEST_INVALID_STATUS);
					count++;
				} else {
					send(__TEST_COUNT);
					send(__TEST_PROVING_EMERG);
					beg_time = milliseconds(t4e);
					start_tt(iutconf.sl.t4e * 10 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
			case __TEST_IUT_IN_SERVICE:
				expand = 1;
				break;
			case __TEST_FISU:
				expand = 1;
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define test_group_8 "Transmission and reception control (Basic)"
#define tgrp_case_8_1 test_group_8
#define sref_case_8_1 "Q.781/8.1"
#define numb_case_8_1 "8.1"
#define name_case_8_1 "MSU transmission and reception"
#define desc_case_8_1 "\
Transmission and reception control (Basic)\n\
MSU transmission and reception\
"
static int
test_case_8_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_DATA);
			start_tt(5000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ACK:
				break;
			case __TEST_IUT_DATA:
				signal(__TEST_SEND_MSU);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ACK:
				break;
			case __TEST_DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_2 test_group_8
#define sref_case_8_2 "Q.781/8.2"
#define numb_case_8_2 "8.2"
#define name_case_8_2 "Negative acknowledgement of an MSU"
#define desc_case_8_2 "\
Transmission and reception control (Basic)\n\
Negative acknowledgement of an MSU\
"
static int
test_case_8_2(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (check_snibs(0xff, 0x80))
					return __RESULT_FAILURE;
				state = 2;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = bib[0] = 0x80;
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (check_snibs(0xff, 0x81))
					return __RESULT_FAILURE;
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				send(__TEST_FISU);
				state = 3;
				break;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = bib[0] = 0x80;
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (check_snibs(0xff, 0x00))
					return __RESULT_FAILURE;
				state = 4;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (check_snibs(0xff, 0x01))
					return __RESULT_FAILURE;
				return __RESULT_SUCCESS;
			case __TEST_FISU:
				fsn[0] = bsn[0] = 0x7f;
				fib[0] = 0x80;
				bib[0] = 0x00;
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_3 test_group_8
#define sref_case_8_3 "Q.781/8.3"
#define numb_case_8_3 "8.3"
#define name_case_8_3 "Check RTB full"
#define desc_case_8_3 "\
Transmission and reception control (Basic)\n\
Check RTB full\
"
static int
test_case_8_3(void)
{
	int i;
	int n = iutconf.sl.N1;
	for (;;) {
		switch (state) {
		case 0:
			stop_tt();
			expand = 0;
			for (i = 0; i < n; i++)
				signal(__TEST_SEND_MSU);
			for (i = 0; i < n; i++)
				signal(__TEST_SEND_MSU);
			count = 0;
			show_fisus = 0;
			show_timeout = 1;
			start_tt(iutconf.sl.t7 * 10 / 2 - 100);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(SHORT_WAIT))) {
			case __TEST_DATA:
				if (++count == n) {
					nacks = n;
					signal(__TEST_ETC);
					signal(__TEST_COUNT);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
					bsn[0] += nacks;
#endif
					send(__TEST_ACK);
					nacks = 1;
					count = 0;
					oldret = 0;
					cntret = 0;
					start_tt(iutconf.sl.t7 * 10 / 2 + 200);
					state = 2;
				}
				break;
			case __EVENT_TIMEOUT:
				expand = 1;
				signal(__TEST_ETC);
				signal(__TEST_COUNT);
				nacks = count;
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] += nacks;
#endif
				send(__TEST_ACK);
				nacks = 1;
				count = 0;
				oldret = 0;
				cntret = 0;
				start_tt(iutconf.sl.t7 * 10 / 2 + 200);
				show_timeout = 1;
				show_fisus = 1;
				state = 2;
				break;
			default:
				expand = 1;
				signal(__TEST_ETC);
				signal(__TEST_COUNT);
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (++count == n) {
					expand = 1;
					nacks = n;
					signal(__TEST_ETC);
					signal(__TEST_COUNT);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
					bsn[0] += nacks;
#endif
					send(__TEST_ACK);
					nacks = 1;
					return __RESULT_SUCCESS;
				}
				expand = 0;
				break;
			default:
				expand = 1;
				signal(__TEST_ETC);
				signal(__TEST_COUNT);
				return __RESULT_FAILURE;
			}
			break;
		default:
			expand = 1;
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_4 test_group_8
#define sref_case_8_4 "Q.781/8.4"
#define numb_case_8_4 "8.4"
#define name_case_8_4 "Single MSU with erroneous FIB"
#define desc_case_8_4 "\
Transmission and reception control (Basic)\n\
Single MSU with erroneous FIB\
"
static int
test_case_8_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_ACK);
			signal(__TEST_SEND_MSU);
			start_tt(iutconf.sl.t7 * 10 + 200);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_ACK);
				send(__TEST_DATA);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				state = 3;
				break;
			case __TEST_ACK:
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_5 test_group_8
#define sref_case_8_5 "Q.781/8.5"
#define numb_case_8_5 "8.5"
#define name_case_8_5 "Duplicated FSN"
#define desc_case_8_5 "\
Transmission and reception control (Basic)\n\
Duplicated FSN\
"
static int
test_case_8_5(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return __RESULT_NOTAPPL;	/* can't do this */
#else
	int inds = 0;
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			send(__TEST_DATA);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				inds++;
				break;
			case __TEST_ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0xffffff) {
					if (bsn[1] != 0)
						return __RESULT_FAILURE;
					fsn[0]--;
					fsn[0] &= 0xffffff;
					send(__TEST_DATA);
					state = 2;
					break;
				}
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 0) {
					if (bsn[1] != 1)
						return __RESULT_FAILURE;
					fsn[0]--;
					fsn[0] &= 0xffffff
					send(__TEST_DATA);
					state = 2;
					break;
				}
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(SHORT_WAIT))) {
			case __TEST_IUT_DATA:
				if (inds++)
					return __RESULT_FAILURE;
				break;
			default:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0)
					return __RESULT_FAILURE;
				if (inds) {
					send(__TEST_DATA);
					state = 3;
					break;
				}
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 1)
					return __RESULT_FAILURE;
				if (inds) {
					send(__TEST_DATA);
					state = 3;
					break;
				}
#endif
				break;
			}
			break;
		case 3:
			switch ((event = wait_event(NORMAL_WAIT))) {
			case __TEST_IUT_DATA:
				if (++inds > 2)
					return __RESULT_FAILURE;
				break;
			default:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0) {
					if (bsn[1] != 1)
						return __RESULT_FAILURE;
					if (inds == 2)
						return __RESULT_SUCCESS;
				}
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 1) {
					if (bsn[1] != 2)
						return __RESULT_FAILURE;
					if (inds == 2)
						return __RESULT_SUCCESS;
				}
#endif
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_6 test_group_8
#define sref_case_8_6 "Q.781/8.6"
#define numb_case_8_6 "8.6"
#define name_case_8_6 "Erroneous retransmission - Single MSU"
#define desc_case_8_6 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Single MSU\
"
static int
test_case_8_6(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			fib[0] = 0x00;
			send(__TEST_DATA);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_IUT_DATA:
				break;
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				send(__TEST_FISU);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				send(__TEST_FISU);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0xff || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return __RESULT_FAILURE;
					fib[0] = 0x00;
					fsn[0] = 0x7f;
					oldmsg = 0;
					cntmsg = 0;
					send(__TEST_DATA);
					state = 4;
					break;
				}
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				break;
			case __TEST_FISU:
				if ((bib[1] | bsn[1]) != 0x7f || (fib[1] | fsn[1]) != 0xff) {
					if (check_snibs(0x00, 0xff))
						return __RESULT_FAILURE;
					return __RESULT_SUCCESS;
				}
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_7 test_group_8
#define sref_case_8_7 "Q.781/8.7"
#define numb_case_8_7 "8.7"
#define name_case_8_7 "Erroneous retransmission - Multiple FISUs"
#define desc_case_8_7 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Multiple FISUs\
"
static int
test_case_8_7(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_FISU);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				send(__TEST_FISU);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				send(__TEST_FISU);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				send(__TEST_FISU);
				state = 4;
				start_tt(100);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_FISU:
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_8 test_group_8
#define sref_case_8_8 "Q.781/8.8"
#define numb_case_8_8 "8.8"
#define name_case_8_8 "Single FISU with corrupt FIB"
#define desc_case_8_8 "\
Transmission and reception control (Basic)\n\
Single FISU with corrupt FIB\
"
static int
test_case_8_8(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(__TEST_FISU);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x00;
				send(__TEST_FISU);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case __TEST_FISU:
			case __EVENT_NO_MSG:
				fib[0] = 0x80;
				send(__TEST_FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_9a test_group_8
#define sref_case_8_9a "Q.781/8.9"
#define numb_case_8_9a "8.9(a)"
#define name_case_8_9a "Single FISU prior to RPO being set"
#define desc_case_8_9a "\
Transmission and reception control (Basic)\n\
Single FISU prior to RPO being set\n\
Forward direction\
"
static int
test_case_8_9a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			send(__TEST_LPO);
			send(__TEST_PROCESSOR_OUTAGE);
			state = 6;
			break;
		case 6:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			send(__TEST_DATA);
			state = 3;
			break;
		case 3:
			start_tt(1000);
			state = 4;
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_RPR:
			case __TEST_ACK:
				break;
			case __TEST_IUT_DATA:
				send(__TEST_DATA);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_IN_SERVICE:
				break;
			case __TEST_IUT_RPR:
				break;
			case __TEST_ACK:
				break;
			case __TEST_IUT_DATA:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_9b test_group_8
#define sref_case_8_9b "Q.781/8.9"
#define numb_case_8_9b "8.9(b)"
#define name_case_8_9b "Single FISU prior to RPO being set"
#define desc_case_8_9b "\
Transmission and reception control (Basic)\n\
Single FISU prior to RPO being set\n\
Reverse direction\
"
static int
test_case_8_9b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = fsn[1];
				signal(__TEST_LPO);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_OUTAGE:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			signal(__TEST_LPR);
			state = 4;
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_PROCESSOR_ENDED:
				send(__TEST_SEQUENCE_SYNC);
				signal(__TEST_SEND_MSU);
				state = 5;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				start_tt(1000);
				state = 6;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_10 test_group_8
#define sref_case_8_10 "Q.781/8.10"
#define numb_case_8_10 "8.10"
#define name_case_8_10 "Abnormal BSN - single MSU"
#define desc_case_8_10 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - single MSU\
"
static int
test_case_8_10(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return __RESULT_NOTAPPL;	/* can't do this */
#else
	uint inds = 0;
	for (;;) {
		switch (state) {
		case 0:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
			bsn[0] = 0xffffff;
#else
			bsn[0] = 0x3fff;
#endif
			send(__TEST_DATA);
			state = 3;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				inds++;
				break;
			case __TEST_ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0xffffff)
					if (bsn[1] != 0)
						return __RESULT_FAILURE;
				bsn[0] = 0xffffff;
				send(__TEST_DATA);
				state = 4;
				break;
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 0)
					if (bsn[1] != 1)
						return __RESULT_FAILURE;
				bsn[0] = 0;
				send(__TEST_DATA);
				state = 4;
				break;
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				if (inds++ > 1)
					return __RESULT_FAILURE;
				if (inds == 2)
					return __RESULT_SUCCESS;
				break;
			case __TEST_ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0)
					if (bsn[1] != 1)
						return __RESULT_FAILURE;
				break;
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 1)
					if (bsn[1] != 2)
						return __RESULT_FAILURE;
				break;
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_11 tgrp_csae_8
#define sref_case_8_11 "Q.781/8.11"
#define numb_case_8_11 "8.11"
#define name_case_8_11 "Abnormal BSN - two consecutive FISUs"
#define desc_case_8_11 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - two consecutive FISUs\
"
static int
test_case_8_11(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return __RESULT_NOTAPPL;	/* can't do this */
#else
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
		case 1:
			switch ((event = wait_event(0))) {
			case __TEST_ACK:
			case __EVENT_NO_MSG:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				bsn[0] = 0x7fffff;
#else
				bsn[0] = 0x3fff;
#endif
				send(__TEST_ACK);
				oldmsg = 0;
				cntmsg = 0;
				send(__TEST_ACK);
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				bsn[0] = 0;
#else
				bsn[0] = 0;
#endif
				send(__TEST_ACK);
				state = 2;
				start_tt(1000);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_ACK:
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_8_12a test_group_8
#define sref_case_8_12a "Q.781/8.12"
#define numb_case_8_12a "8.12(a)"
#define name_case_8_12a "Excessive delay of acknowledgement"
#define desc_case_8_12a "\
Transmission and reception control (Basic)\n\
Excessive delay of acknowledgement\n\
Single data\
"
static int
test_case_8_12a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_12b test_group_8
#define sref_case_8_12b "Q.781/8.12"
#define numb_case_8_12b "8.12(b)"
#define name_case_8_12b "Excessive delay of acknowledgement"
#define desc_case_8_12b "\
Transmission and reception control (Basic)\n\
Excessive delay of acknowledgement\n\
Multiple data\
"
static int
test_case_8_12b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_13 test_group_8
#define sref_case_8_13 "Q.781/8.13"
#define numb_case_8_13 "8.13"
#define name_case_8_13 "Level 3 Stop command"
#define desc_case_8_13 "\
Transmission and reception control (Basic)\n\
Level 3 Stop command\
"
static int
test_case_8_13(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = wait_event(SHORT_WAIT))) {
			case __EVENT_NO_MSG:
				signal(__TEST_STOP);
				start_tt(1000);
				state = 1;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_8_14 test_group_8
#define sref_case_8_14 "Q.781/8.14"
#define numb_case_8_14 "8.14"
#define name_case_8_14 "Abnormal FIBR"
#define desc_case_8_14 "\
Transmission and reception control (Basic)\n\
Abnormal FIBR\
"
static int
test_case_8_14(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return __RESULT_NOTAPPL;	/* can't do this */
#else
	int nind = 0;
	for (;;) {
		switch (state) {
		case 0:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
			bsn[0] = 0xffffff;
#else
			bsn[0] = 0x3fff;
#endif
			send(__TEST_DATA);
			state = 3;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				if (nind < 1)
					nind++;
				else
					return __RESULT_FAILURE;
				break;
			case __TEST_ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (fsn[1] != 0xffffff)
					return __RESULT_FAILURE;
				if (bsn[1] != 0xffffff)
					if (bsn[1] != 0)
						return __RESULT_FAILURE;
				fsn[0] = 1;
				send(__TEST_DATA);
				state = 4;
				break;
#else
				if (fsn[1] != 0)
					return __RESULT_FAILURE;
				if (bsn[1] != 0)
					if (bsn[1] != 1)
						return __RESULT_FAILURE;
				fsn[0] = 2;
				send(__TEST_DATA);
				state = 4;
				break;
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				if (nind < 1)
					nind++;
				else
					return __RESULT_FAILURE;
				break;
#if defined M2PA_VERSION_DRAFT11
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
#else
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
#endif
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define test_group_9 "Transmission and reception control (PCR)"
#define tgrp_case_9_1 test_group_9
#define sref_case_9_1 "Q.781/9.1"
#define numb_case_9_1 "9.1"
#define name_case_9_1 "MSU transmission and reception"
#define desc_case_9_1 "\
Transmission and reception control (PCR)\n\
MSU transmission and reception\
"
static int
test_case_9_1(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				if (count < 4) {
					cntret = -1;
					bsn[0] = 0x7f;
					send(__TEST_FISU);
					count++;
					break;
				}
				send(__TEST_FISU);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				send(__TEST_FISU);
				break;
			case __TEST_FISU:
				if (check_snibs(0xff, 0x80))
					return __RESULT_FAILURE;
				send(__TEST_DATA);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				break;
			case __TEST_FISU:
				if ((bsn[1] | bib[1]) != 0xff || (fsn[1] | fib[1]) != 0x80)
					return check_snibs(0x80, 0x80);
				send(__TEST_FISU);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_2 test_group_9
#define sref_case_9_2 "Q.781/9.2"
#define numb_case_9_2 "9.2"
#define name_case_9_2 "Priority control"
#define desc_case_9_2 "\
Transmission and reception control (PCR)\n\
Priority control\
"
static int
test_case_9_2(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	int fsn_lo = 0, fsn_hi = 0, fsn_ex = 0;
	for (;;) {
		switch (state) {
		case 0:
			fsn_lo = 0;
			signal(__TEST_SEND_MSU);
			signal(__TEST_SEND_MSU);
			fsn_hi = 1;
			fsn_ex = fsn_lo;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				if (fsn[1] < fsn_hi) {
					if (fsn[1] != fsn_ex)
						return __RESULT_FAILURE;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					bsn[0] = 0x7f;
					send(__TEST_FISU);
					break;
				}
				if (fsn[1] != fsn_hi)
					return __RESULT_FAILURE;
				signal(__TEST_ETC);
				signal(__TEST_TRIES);
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				signal(__TEST_SEND_MSU);
				fsn_hi++;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] < fsn_hi) {
					if (fsn[1] != fsn_ex)
						return __RESULT_FAILURE;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					bsn[0] = 0x7f;
					send(__TEST_FISU);
					break;
				}
				if (fsn[1] != fsn_hi)
					return __RESULT_FAILURE;
				signal(__TEST_ETC);
				signal(__TEST_TRIES);
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_FISU:
				if (fsn[1] == 2)
					return __RESULT_SUCCESS;
				return __RESULT_FAILURE;
			case __TEST_DATA:
				bsn[0] = fsn_lo;
				send(__TEST_FISU);
				if (fsn[1] != fsn_ex) {
					if (fsn[1] == fsn_ex + 1) {
						if (fsn_lo < fsn_hi) {
							fsn_lo++;
							fsn_ex++;
							signal(__TEST_ETC);
							signal(__TEST_TRIES);
							tries = 0;
						}
					} else
						fsn_ex--;
				} else if (++tries > 100)
					return __RESULT_FAILURE;
				if (++fsn_ex > fsn_hi)
					fsn_ex = fsn_lo;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_3 test_group_9
#define sref_case_9_3 "Q.781/9.3"
#define numb_case_9_3 "9.3"
#define name_case_9_3 "Forced retransmission with the value N1"
#define desc_case_9_3 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N1\
"
static int
test_case_9_3(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;
	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3)
		return __RESULT_INCONCLUSIVE;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				count = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n)
					return __RESULT_FAILURE;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 0 && count) {
					bsn[0] = 0x0;
					send(__TEST_FISU);
					count = 1;
					state = 3;
					break;
				}
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n) {
					send(__TEST_FISU);
					return __RESULT_SUCCESS;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = 0;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_4 test_group_9
#define sref_case_9_4 "Q.781/9.4"
#define numb_case_9_4 "9.4"
#define name_case_9_4 "Forced retransmission with the value N2"
#define desc_case_9_4 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N2\
"
static int
test_case_9_4(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1 - 1;
	msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
	n = iutconf.sl.N2 / (msu_len + h) + 1;
	if (msu_len > iutconf.sdl.m)
		return __RESULT_INCONCLUSIVE;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				count = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n)
					return __RESULT_FAILURE;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 0 && count) {
					bsn[0] = 0x0;
					send(__TEST_FISU);
					count = 1;
					state = 3;
					break;
				}
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n) {
					send(__TEST_FISU);
					return __RESULT_SUCCESS;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = 0;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_5 test_group_9
#define sref_case_9_5 "Q.781/9.5"
#define numb_case_9_5 "9.5"
#define name_case_9_5 "Forced retransmission cancel"
#define desc_case_9_5 "\
Transmission and reception control (PCR)\n\
Forced retransmission cancel\
"
static int
test_case_9_5(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;
	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3) {
		n = iutconf.sl.N1 - 1;
		msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
		n = iutconf.sl.N2 / (msu_len + h) + 1;
		if (msu_len > iutconf.sdl.m)
			return __RESULT_INCONCLUSIVE;
	}
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				count = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n)
					return __RESULT_FAILURE;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 3 && count) {
					bsn[0] = n - 1;
					send(__TEST_FISU);
					count = 1;
					state = 3;
					break;
				}
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n - 3)
					return __RESULT_FAILURE;
				if (fsn[1] == n) {
					send(__TEST_FISU);
					return __RESULT_SUCCESS;
				}
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				bsn[0] = n - 1;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_6 test_group_9
#define sref_case_9_6 "Q.781/9.6"
#define numb_case_9_6 "9.6"
#define name_case_9_6 "Reception of forced retransmission"
#define desc_case_9_6 "\
Transmission and reception control (PCR)\n\
Reception of forced retransmission\
"
static int
test_case_9_6(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;
	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3) {
		n = iutconf.sl.N1 - 1;
		msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
		n = iutconf.sl.N2 / (msu_len + h) + 1;
		if (msu_len > iutconf.sdl.m)
			return __RESULT_INCONCLUSIVE;
	}
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				count = 0;
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				if (fsn[1] == n)
					return __RESULT_FAILURE;
				if (fsn[1] == n - 1)
					count++;
				if (fsn[1] > 1 && fsn[1] < n - 3)
					oldisb++;
				if (fsn[1] == 3 && count == 2) {
					bsn[0] = n - 1;
					send(__TEST_FISU);
					return __RESULT_SUCCESS;
				}
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				if (fsn[1] == 2)
					signal(__TEST_ETC);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_7 test_group_9
#define sref_case_9_7 "Q.781/9.7"
#define numb_case_9_7 "9.7"
#define name_case_9_7 "MSU transmission while RPO set"
#define desc_case_9_7 "\
Transmission and reception control (PCR)\n\
MSU transmission while RPO set\
"
static int
test_case_9_7(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			start_tt(5000);
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_LPO);
				send(__TEST_PROCESSOR_OUTAGE);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_IUT_RPO:
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_PROCESSOR_OUTAGE);
				break;
			case __TEST_FISU:
				if (!count++)
					if (check_snibs(0xff, 0x80))
						return __RESULT_FAILURE;
				bsn[0] = 0x7f;
				send(__TEST_PROCESSOR_OUTAGE);
				break;
			case __EVENT_TIMEOUT:
				signal(__TEST_CLEARB);
				send(__TEST_LPR);
				bsn[0] = 0x7f;
				send(__TEST_DATA);
				start_tt(500);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
			case __TEST_IUT_RPR:
				break;
			case __TEST_FISU:
				if (bsn[1] != 0) {
					bsn[0] = 0x7f;
					fsn[0] = 0x7f;
					send(__TEST_DATA);
					break;
				}
				send(__TEST_FISU);
				return __RESULT_SUCCESS;
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_8 test_group_9
#define sref_case_9_8 "Q.781/9.8"
#define numb_case_9_8 "9.8"
#define name_case_9_8 "Abnormal BSN - Single MSU"
#define desc_case_9_8 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Single MSU\
"
static int
test_case_9_8(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			bsn[0] = 0x00;
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			bsn[0] = 0x7f;
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				break;
			case __TEST_FISU:
				if (bsn[1] == 0)
					return __RESULT_SUCCESS;
				bsn[0] = 0x7f;
				fsn[0] = 0x7f;
				send(__TEST_DATA);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_9 test_group_9
#define sref_case_9_9 "Q.781/9.9"
#define numb_case_9_9 "9.9"
#define name_case_9_9 "Abnormal BSN - Two MSUs"
#define desc_case_9_9 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Two MSUs\
"
static int
test_case_9_9(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			bsn[0] = 0x7e;
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			bsn[0] = 0x7f;
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			bsn[0] = 0x7e;
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_DATA:
				return __RESULT_FAILURE;
			case __TEST_FISU:
				bsn[0] = 0x7f;
				fsn[0] = 0x7f;
				send(__TEST_DATA);
				break;
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_10 test_group_9
#define sref_case_9_10 "Q.781/9.10"
#define numb_case_9_10 "9.10"
#define name_case_9_10 "Unexpected FSN"
#define desc_case_9_10 "\
Transmission and reception control (PCR)\n\
Unexpected FSN\
"
static int
test_case_9_10(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			fsn[0] = 0x7f;
			send(__TEST_DATA);
			fsn[0] = 0x01;
			send(__TEST_DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_IUT_DATA:
				if (++count == 2)
					return __RESULT_FAILURE;
				break;
			case __TEST_FISU:
				if (bsn[1] != 0x7f) {
					if (check_snibs(0x80, 0xff))
						return __RESULT_FAILURE;
					return __RESULT_SUCCESS;
				}
				fsn[0] = 0x7f;
				send(__TEST_DATA);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_11 test_group_9
#define sref_case_9_11 "Q.781/9.11"
#define numb_case_9_11 "9.11"
#define name_case_9_11 "Excessive delay of acknowledgement"
#define desc_case_9_11 "\
Transmission and reception control (PCR)\n\
Excessive delay of acknowledgement\
"
static int
test_case_9_11(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				beg_time = milliseconds(t7);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_DATA:
				bsn[0] = 0x7f;
				send(__TEST_FISU);
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_12 test_group_9
#define sref_case_9_12 "Q.781/9.12"
#define numb_case_9_12 "9.12"
#define name_case_9_12 "FISU with FSN expected for MSU"
#define desc_case_9_12 "\
Transmission and reception control (PCR)\n\
FISU with FSN expected for MSU\
"
static int
test_case_9_12(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			fsn[0] = 0x00;
			send(__TEST_FISU);
			fsn[0] = 0x7f;
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __EVENT_TIMEOUT:
				return check_snibs(0xff, 0xff);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define tgrp_case_9_13 test_group_9
#define sref_case_9_13 "Q.781/9.13"
#define numb_case_9_13 "9.13"
#define name_case_9_13 "Level 3 Stop command"
#define desc_case_9_13 "\
Transmission and reception control (PCR)\n\
Level 3 Stop command\
"
static int
test_case_9_13(void)
{
	return __RESULT_NOTAPPL;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_STOP);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_FISU:
				send(__TEST_FISU);
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_OUT_OF_SERVICE:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
#endif
}

#define test_group_10 "Congestion Control"
#define tgrp_case_10_1 test_group_10
#define sref_case_10_1 "Q.781/10.1"
#define numb_case_10_1 "10.1"
#define name_case_10_1 "Congestion abatement"
#define desc_case_10_1 "\
Congestion Control\n\
Congestion abatement\
"
static int
test_case_10_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_CONG_D);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_BUSY:
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(NORMAL_WAIT))) {
			case __EVENT_NO_MSG:
				signal(__TEST_NO_CONG);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __TEST_BUSY_ENDED:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_10_2a test_group_10
#define sref_case_10_2a "Q.781/10.2"
#define numb_case_10_2a "10.2(a)"
#define name_case_10_2a "Timer T7 during receive congestion"
#define desc_case_10_2a "\
Congestion Control\n\
Timer T7\n\
During receive congestion\
"
static int
test_case_10_2a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				send(__TEST_BUSY);
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				start_tt((iutconf.sl.t6 - iutconf.sl.t7) * 10 - 300);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				bsn[0] = fsn[1];
#endif
				send(__TEST_BUSY_ENDED);
				send(__TEST_ACK);
				state = 4;
				start_tt(1000);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_10_2b test_group_10
#define sref_case_10_2b "Q.781/10.2"
#define numb_case_10_2b "10.2(b)"
#define name_case_10_2b "Timer T7 after receive congestion"
#define desc_case_10_2b "\
Congestion Control\n\
Timer T7\n\
After receive congestion\
"
static int
test_case_10_2b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				send(__TEST_BUSY);
				start_tt(iutconf.sl.t6 * 10 - 300);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				send(__TEST_BUSY_ENDED);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				state = 4;
				start_tt(1000);
				break;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return __RESULT_FAILURE;
			}
		case 4:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_FAILURE;
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_10_2c test_group_10
#define sref_case_10_2c "Q.781/10.2"
#define numb_case_10_2c "10.2(c)"
#define name_case_10_2c "Timer T7 after receive congestion"
#define desc_case_10_2c "\
Congestion Control\n\
Timer T7\n\
After receive congestion\
"
static int
test_case_10_2c(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				send(__TEST_BUSY);
				start_tt(iutconf.sl.t6 * 10 - 300);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				start_tt(iutconf.sl.t7 * 10 - 300);
				beg_time = milliseconds(t7);
				send(__TEST_BUSY_ENDED);
				state = 3;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				bsn[0] = fsn[1];
				send(__TEST_ACK);
				state = 4;
				start_tt(1000);
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case __EVENT_TIMEOUT:
				return __RESULT_SUCCESS;
			default:
				return __RESULT_FAILURE;
			}
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

#define tgrp_case_10_3 test_group_10
#define sref_case_10_3 "Q.781/10.3"
#define numb_case_10_3 "10.3"
#define name_case_10_3 "Timer T6"
#define desc_case_10_3 "\
Congestion Control\n\
Timer T6\
"
static int
test_case_10_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(__TEST_SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_DATA:
				send(__TEST_BUSY);
				start_tt(timer[t6].hi * 10 + 200);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return __RESULT_FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				break;
			case __TEST_IUT_OUT_OF_SERVICE:
				return check_time("T6  ", beg_time, timer[t6].lo, timer[t6].hi);
			default:
				return __RESULT_FAILURE;
			}
			break;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
}

/*
 *  =========================================================================
 *
 *  OPENING, CONFIGURING and CLOSING Devices
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Opening, configuring and closing the Management Stream (MGM)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Open and configure for SCTP operation the Protocol Tester (PT).
 */
static int
mgm_open(void)
{
	int ret;
	if (verbose > 1) {
		printf("                 :open |                                            \n");
		FFLUSH(stdout);
	}
	if ((test_fd[2] = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("  ****ERROR: open failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                :ioctl |                                            \n");
		FFLUSH(stdout);
	}
	if (ioctl(test_fd[2], I_SRDOPT, RMSGD) < 0) {
		printf("  ****ERROR: ioctl failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if ((ret = control(__TEST_OPTMGMT_REQ)) != __RESULT_SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_OK_ACK)
		return __RESULT_FAILURE;
	if ((ret = control(__TEST_BIND_REQ)) != __RESULT_SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_BIND_ACK)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  Close the Protocol Tester (PT).
 */
static int
mgm_close(void)
{
	if (verbose > 1) {
		printf("                :close |                                            \n");
		FFLUSH(stdout);
	}
	if (close(test_fd[2]) < 0) {
		printf("  ****ERROR: close failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Opening, configuring and closing the Protocol Tester (PT)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Open and configure for SCTP operation the Protocol Tester (PT).
 */
static int
pt_open(void)
{
	int ret;
	if (verbose > 1) {
		printf("                  :open                                             \n");
		FFLUSH(stdout);
	}
	if ((test_fd[0] = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("****ERROR: open failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                 :ioctl                                             \n");
		FFLUSH(stdout);
	}
	if (ioctl(test_fd[0], I_SRDOPT, RMSGD) < 0) {
		printf("****ERROR: ioctl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if ((ret = send(__TEST_OPTMGMT_REQ)) != __RESULT_SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_OK_ACK)
		return __RESULT_FAILURE;
	if ((ret = send(__TEST_BIND_REQ)) != __RESULT_SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_BIND_ACK)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  Close the Protocol Tester (PT).
 */
static int
pt_close(void)
{
	if (verbose > 1) {
		printf("                 :close                                             \n");
		FFLUSH(stdout);
	}
	if (close(test_fd[0]) < 0) {
		printf("****ERROR: close failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Opening, configuring and closing the Implementation Under Test (IUT)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Open and configure for SCTP operation the Implementation Under Test (IUT).
 */
static int
iut_open(void)
{
	int ret;
	if (verbose > 1) {
		printf("                                             :open                  \n");
		FFLUSH(stdout);
	}
	if ((test_fd[1] = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("                                   ****ERROR: open failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :ioctl                 \n");
		FFLUSH(stdout);
	}
	if (ioctl(test_fd[1], I_SRDOPT, RMSGD) < 0) {
		printf("                                   ****ERROR: ioctl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if ((ret = signal(__TEST_OPTMGMT_REQ)) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_OK_ACK)
		return __RESULT_FAILURE;
	if ((ret = signal(__TEST_BIND_REQ)) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_BIND_ACK)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  Connect the Implementation Under Test (IUT) to the Protocol Tester (PT).
 */
static int
iut_connect(void)
{
	int ret;
	if (verbose > 1) {
		printf("                                             :connect               \n");
		FFLUSH(stdout);
	}
	if ((ret = signal(__TEST_CONN_REQ)) != __RESULT_SUCCESS)
		return ret;
	if ((event = wait_event(LONG_WAIT)) != __TEST_CONN_IND)
		return __RESULT_FAILURE;
	if ((ret = control(__TEST_CONN_RES)) != __RESULT_SUCCESS)
		return ret;
	ret = wait_event(LONG_WAIT);
	if (ret != __TEST_OK_ACK && ret != __TEST_CONN_CON)
		return __RESULT_FAILURE;
	ret = wait_event(LONG_WAIT);
	if (ret != __TEST_OK_ACK && ret != __TEST_CONN_CON)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  Push and configure Implementation Under Test (IUT) for SL operation.
 */
static int
iut_m2pa_push(void)
{
	struct strioctl ctl;
	if (verbose > 1) {
		printf("                                             :push m2pa-sl          \n");
		FFLUSH(stdout);
	}
	if (ioctl(test_fd[1], I_PUSH, "m2pa-sl") < 0) {
		printf("                                   ****ERROR: push m2pa-sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :options sdl           \n");
		FFLUSH(stdout);
	}
	iutconf.opt.popt = iut_options;
	ctl.ic_cmd = SDL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.opt);
	ctl.ic_dp = (char *) &iutconf.opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sdl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sdl            \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdl);
	ctl.ic_dp = (char *) &iutconf.sdl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sdl failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :options sdt           \n");
		FFLUSH(stdout);
	}
	iutconf.opt.popt = iut_options;
	ctl.ic_cmd = SDT_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.opt);
	ctl.ic_dp = (char *) &iutconf.opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sdt failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sdt            \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdt);
	ctl.ic_dp = (char *) &iutconf.sdt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sdt failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :options sl            \n");
		FFLUSH(stdout);
	}
	iutconf.opt.popt = iut_options;
	ctl.ic_cmd = SL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.opt);
	ctl.ic_dp = (char *) &iutconf.opt;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sl             \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sl);
	ctl.ic_dp = (char *) &iutconf.sl;
	if (ioctl(test_fd[1], I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}
static int
iut_enable(void)
{
	int ret;
	if ((ret = signal(__TEST_ENABLE_REQ)) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != __TEST_ENABLE_CON)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
static int
iut_disable(void)
{
	int ret;
	if ((ret = signal(__TEST_DISABLE_REQ)) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (wait_event(SHORT_WAIT) != __TEST_DISABLE_CON)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
static int
iut_pop(void)
{
	if (verbose > 1) {
		printf("                                             :pop                   \n");
		FFLUSH(stdout);
	}
	if (ioctl(test_fd[1], I_POP, 0) < 0) {
		printf("                                   ****ERROR: pop failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}
static int
iut_disconnect(void)
{
	int ret;
	if ((ret = signal(__TEST_DISCON_REQ)) != __RESULT_SUCCESS)
		return ret;
	if (wait_event(SHORT_WAIT) != __TEST_OK_ACK)
		return __RESULT_FAILURE;
	if (wait_event(LONG_WAIT) != __TEST_DISCON_IND)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  Close Implementation Under Test (IUT).
 */
static int
iut_close(void)
{
	if (verbose > 1) {
		printf("                                             :close                 \n");
		FFLUSH(stdout);
	}
	if (close(test_fd[1]) < 0) {
		printf("                                   ****ERROR: close failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
begin_tests(void)
{
	// ioctl( test_fd[0], I_FLUSH, FLUSHRW); /* flush Protocol Tester (PT) */
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
	oldmgm = 0;
	cntmgm = 0;
	msu_len = MSU_LEN;
	return (__RESULT_SUCCESS);
}
static int
begin_test_case(void)
{
	state = 0;
	event = 0;
	count = 0;
	tries = 0;
	beg_time = 0;
	expand = 1;
	return (__RESULT_SUCCESS);
}
#if 0
static int
end_tests(void)
{
	return (__RESULT_SUCCESS);
}
#endif

/*
 *  =========================================================================
 *
 *  PRECONDITIONS and POSTCONDITIONS
 *
 *  =========================================================================
 */
/*
 *  Common Postcondition.
 */
static int
iut_power_off(void)
{
	signal(__TEST_STOP);
	stop_tt();
	show_msus = 0;
	show_fisus = 1;
	// while ( wait_event(0) != __EVENT_NO_MSG );
	iut_disable();
	ioctl(test_fd[1], I_FLUSH, FLUSHRW);
	iut_pop();
	iut_disconnect();
	iut_close();
	ioctl(test_fd[0], I_FLUSH, FLUSHRW);
	pt_close();
	ioctl(test_fd[2], I_FLUSH, FLUSHRW);
	mgm_close();
	return __RESULT_SUCCESS;
}

/* 
 *  Common Preconditions:
 */
static int
link_power_off(void)
{
	iut_options = 0;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (pt_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_connect() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_m2pa_push() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_enable() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	start_tt(LONGEST_WAIT);
	return __RESULT_SUCCESS;
}
static int
link_out_of_service(void)
{
	iut_options = 0;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (pt_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_connect() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_m2pa_push() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_enable() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (signal(__TEST_POWER_ON) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	start_tt(LONGEST_WAIT);
	return __RESULT_SUCCESS;
}
static int
link_out_of_service_np(void)
{
	iut_options = SS7_POPT_NOPR;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	bib[0] = fib[0] = bib[1] = fib[1] = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	bsn[0] = fsn[0] = bsn[1] = fsn[1] = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (pt_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_open() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_connect() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_m2pa_push() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (iut_enable() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	if (signal(__TEST_POWER_ON) != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	start_tt(LONGEST_WAIT);
	return __RESULT_SUCCESS;
}
static int
link_in_service(void)
{
	if (link_out_of_service() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case __TEST_OUT_OF_SERVICE:
				send(__TEST_POWER_ON);
				send(__TEST_OUT_OF_SERVICE);
				signal(__TEST_START);
				state = 1;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case __TEST_ALIGNMENT:
				send(__TEST_START);
				send(__TEST_ALIGNMENT);
				state = 2;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 10 + 200);
				expand = 0;
				state = 3;
				break;
			default:
				return __RESULT_INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case __TEST_PROVING:
				send(__TEST_PROVING);
				expand = 0;
				break;
#else
			case __TEST_PROVING_NORMAL:
				send(__TEST_PROVING_EMERG);
				expand = 0;
				break;
#endif
			case __TEST_IN_SERVICE:
				expand = 1;
				send(__TEST_IN_SERVICE);
				break;
			case __TEST_IUT_IN_SERVICE:
				expand = 1;
				start_tt(LONGEST_WAIT);
				return __RESULT_SUCCESS;
			default:
				expand = 1;
				return __RESULT_INCONCLUSIVE;
			}
			break;
		default:
			return __RESULT_INCONCLUSIVE;
		}
	}
	return __RESULT_INCONCLUSIVE;
}
static int
link_in_service_basic(void)
{
	iut_options = 0;
	return link_in_service();
}
static int
link_in_service_pcr(void)
{
	// iut_options = SS7_POPT_PCR;
	return link_in_service();
}


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
		numb_case_1_1a, tgrp_case_1_1a, name_case_1_1a, desc_case_1_1a, sref_case_1_1a, {
			{ link_power_off, test_case_1_1a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_1b, tgrp_case_1_1b, name_case_1_1b, desc_case_1_1b, sref_case_1_1b, {
			{ link_out_of_service, test_case_1_1b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_2, tgrp_case_1_2, name_case_1_2, desc_case_1_2, sref_case_1_2, {
			{ link_out_of_service, test_case_1_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_3, tgrp_case_1_3, name_case_1_3, desc_case_1_3, sref_case_1_3, {
			{ link_out_of_service, test_case_1_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_4, tgrp_case_1_4, name_case_1_4, desc_case_1_4, sref_case_1_4, {
			{ link_out_of_service, test_case_1_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_5a_p, tgrp_case_1_5a_p, name_case_1_5a_p, desc_case_1_5a_p, sref_case_1_5a_p, {
			{ link_out_of_service, test_case_1_5a_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_5b_p, tgrp_case_1_5b_p, name_case_1_5b_p, desc_case_1_5b_p, sref_case_1_5b_p, {
			{ link_out_of_service, test_case_1_5b_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_5a_np, tgrp_case_1_5a_np, name_case_1_5a_np, desc_case_1_5a_np, sref_case_1_5a_np, {
			{ link_out_of_service_np, test_case_1_5a_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_5b_np, tgrp_case_1_5b_np, name_case_1_5b_np, desc_case_1_5b_np, sref_case_1_5b_np, {
			{ link_out_of_service_np, test_case_1_5b_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_6_p, tgrp_case_1_6_p, name_case_1_6_p, desc_case_1_6_p, sref_case_1_6_p, {
			{ link_out_of_service, test_case_1_6_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_6_np, tgrp_case_1_6_np, name_case_1_6_np, desc_case_1_6_np, sref_case_1_6_np, {
			{ link_out_of_service_np, test_case_1_6_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_7, tgrp_case_1_7, name_case_1_7, desc_case_1_7, sref_case_1_7, {
			{ link_out_of_service, test_case_1_7, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_8a_p, tgrp_case_1_8a_p, name_case_1_8a_p, desc_case_1_8a_p, sref_case_1_8a_p, {
			{ link_out_of_service, test_case_1_8a_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_8b_p, tgrp_case_1_8b_p, name_case_1_8b_p, desc_case_1_8b_p, sref_case_1_8b_p, {
			{ link_out_of_service, test_case_1_8b_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_8a_np, tgrp_case_1_8a_np, name_case_1_8a_np, desc_case_1_8a_np, sref_case_1_8a_np, {
			{ link_out_of_service_np, test_case_1_8a_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_8b_np, tgrp_case_1_8b_np, name_case_1_8b_np, desc_case_1_8b_np, sref_case_1_8b_np, {
			{ link_out_of_service_np, test_case_1_8b_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_9a_p, tgrp_case_1_9a_p, name_case_1_9a_p, desc_case_1_9a_p, sref_case_1_9a_p, {
			{ link_out_of_service, test_case_1_9a_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_9b_p, tgrp_case_1_9b_p, name_case_1_9b_p, desc_case_1_9b_p, sref_case_1_9b_p, {
			{ link_out_of_service, test_case_1_9b_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_9a_np, tgrp_case_1_9a_np, name_case_1_9a_np, desc_case_1_9a_np, sref_case_1_9a_np, {
			{ link_out_of_service_np, test_case_1_9a_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_9b_np, tgrp_case_1_9b_np, name_case_1_9b_np, desc_case_1_9b_np, sref_case_1_9b_np, {
			{ link_out_of_service_np, test_case_1_9b_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_10_p, tgrp_case_1_10_p, name_case_1_10_p, desc_case_1_10_p, sref_case_1_10_p, {
			{ link_out_of_service, test_case_1_10_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_10_np, tgrp_case_1_10_np, name_case_1_10_np, desc_case_1_10_np, sref_case_1_10_np, {
			{ link_out_of_service_np, test_case_1_10_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_11_p, tgrp_case_1_11_p, name_case_1_11_p, desc_case_1_11_p, sref_case_1_11_p, {
			{ link_out_of_service, test_case_1_11_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_11_np, tgrp_case_1_11_np, name_case_1_11_np, desc_case_1_11_np, sref_case_1_11_np, {
			{ link_out_of_service_np, test_case_1_11_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_12a_p, tgrp_case_1_12a_p, name_case_1_12a_p, desc_case_1_12a_p, sref_case_1_12a_p, {
			{ link_out_of_service, test_case_1_12a_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_12b_p, tgrp_case_1_12b_p, name_case_1_12b_p, desc_case_1_12b_p, sref_case_1_12b_p, {
			{ link_out_of_service, test_case_1_12b_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_12a_np, tgrp_case_1_12a_np, name_case_1_12a_np, desc_case_1_12a_np, sref_case_1_12a_np, {
			{ link_out_of_service_np, test_case_1_12a_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_12b_np, tgrp_case_1_12b_np, name_case_1_12b_np, desc_case_1_12b_np, sref_case_1_12b_np, {
			{ link_out_of_service_np, test_case_1_12b_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_13_p, tgrp_case_1_13_p, name_case_1_13_p, desc_case_1_13_p, sref_case_1_13_p, {
			{ link_out_of_service, test_case_1_13_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_13_np, tgrp_case_1_13_np, name_case_1_13_np, desc_case_1_13_np, sref_case_1_13_np, {
			{ link_out_of_service_np, test_case_1_13_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_14, tgrp_case_1_14, name_case_1_14, desc_case_1_14, sref_case_1_14, {
			{ link_out_of_service, test_case_1_14, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_15_p, tgrp_case_1_15_p, name_case_1_15_p, desc_case_1_15_p, sref_case_1_15_p, {
			{ link_out_of_service, test_case_1_15_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_15_np, tgrp_case_1_15_np, name_case_1_15_np, desc_case_1_15_np, sref_case_1_15_np, {
			{ link_out_of_service_np, test_case_1_15_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_16_p, tgrp_case_1_16_p, name_case_1_16_p, desc_case_1_16_p, sref_case_1_16_p, {
			{ link_out_of_service, test_case_1_16_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_16_np, tgrp_case_1_16_np, name_case_1_16_np, desc_case_1_16_np, sref_case_1_16_np, {
			{ link_out_of_service_np, test_case_1_16_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_17, tgrp_case_1_17, name_case_1_17, desc_case_1_17, sref_case_1_17, {
			{ link_out_of_service, test_case_1_17, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_18, tgrp_case_1_18, name_case_1_18, desc_case_1_18, sref_case_1_18, {
			{ link_out_of_service, test_case_1_18, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_19, tgrp_case_1_19, name_case_1_19, desc_case_1_19, sref_case_1_19, {
			{ link_out_of_service, test_case_1_19, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_20, tgrp_case_1_20, name_case_1_20, desc_case_1_20, sref_case_1_20, {
			{ link_out_of_service, test_case_1_20, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_21, tgrp_case_1_21, name_case_1_21, desc_case_1_21, sref_case_1_21, {
			{ link_out_of_service, test_case_1_21, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_22, tgrp_case_1_22, name_case_1_22, desc_case_1_22, sref_case_1_22, {
			{ link_out_of_service, test_case_1_22, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_23, tgrp_case_1_23, name_case_1_23, desc_case_1_23, sref_case_1_23, {
			{ link_out_of_service, test_case_1_23, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_24, tgrp_case_1_24, name_case_1_24, desc_case_1_24, sref_case_1_24, {
			{ link_out_of_service, test_case_1_24, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_25, tgrp_case_1_25, name_case_1_25, desc_case_1_25, sref_case_1_25, {
			{ link_out_of_service, test_case_1_25, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_26, tgrp_case_1_26, name_case_1_26, desc_case_1_26, sref_case_1_26, {
			{ link_out_of_service, test_case_1_26, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_27_p, tgrp_case_1_27_p, name_case_1_27_p, desc_case_1_27_p, sref_case_1_27_p, {
			{ link_out_of_service, test_case_1_27_p, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_27_np, tgrp_case_1_27_np, name_case_1_27_np, desc_case_1_27_np, sref_case_1_27_np, {
			{ link_out_of_service_np, test_case_1_27_np, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_28, tgrp_case_1_28, name_case_1_28, desc_case_1_28, sref_case_1_28, {
			{ link_in_service_basic, test_case_1_28, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_29a, tgrp_case_1_29a, name_case_1_29a, desc_case_1_29a, sref_case_1_29a, {
			{ link_in_service_basic, test_case_1_29a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_29b, tgrp_case_1_29b, name_case_1_29b, desc_case_1_29b, sref_case_1_29b, {
			{ link_in_service_basic, test_case_1_29b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_30a, tgrp_case_1_30a, name_case_1_30a, desc_case_1_30a, sref_case_1_30a, {
			{ link_in_service_basic, test_case_1_30a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_30b, tgrp_case_1_30b, name_case_1_30b, desc_case_1_30b, sref_case_1_30b, {
			{ link_in_service_basic, test_case_1_30b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_31a, tgrp_case_1_31a, name_case_1_31a, desc_case_1_31a, sref_case_1_31a, {
			{ link_in_service_basic, test_case_1_31a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_31b, tgrp_case_1_31b, name_case_1_31b, desc_case_1_31b, sref_case_1_31b, {
			{ link_in_service_basic, test_case_1_31b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_32a, tgrp_case_1_32a, name_case_1_32a, desc_case_1_32a, sref_case_1_32a, {
			{ link_out_of_service, test_case_1_32a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_32b, tgrp_case_1_32b, name_case_1_32b, desc_case_1_32b, sref_case_1_32b, {
			{ link_out_of_service, test_case_1_32b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_33, tgrp_case_1_33, name_case_1_33, desc_case_1_33, sref_case_1_33, {
			{ link_out_of_service, test_case_1_33, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_34, tgrp_case_1_34, name_case_1_34, desc_case_1_34, sref_case_1_34, {
			{ link_out_of_service, test_case_1_34, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_1_35, tgrp_case_1_35, name_case_1_35, desc_case_1_35, sref_case_1_35, {
			{ link_out_of_service, test_case_1_35, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_1, tgrp_case_2_1, name_case_2_1, desc_case_2_1, sref_case_2_1, {
			{ link_out_of_service, test_case_2_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_2, tgrp_case_2_2, name_case_2_2, desc_case_2_2, sref_case_2_2, {
			{ link_out_of_service, test_case_2_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_3, tgrp_case_2_3, name_case_2_3, desc_case_2_3, sref_case_2_3, {
			{ link_out_of_service, test_case_2_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_4, tgrp_case_2_4, name_case_2_4, desc_case_2_4, sref_case_2_4, {
			{ link_out_of_service, test_case_2_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_5, tgrp_case_2_5, name_case_2_5, desc_case_2_5, sref_case_2_5, {
			{ link_out_of_service, test_case_2_5, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_6, tgrp_case_2_6, name_case_2_6, desc_case_2_6, sref_case_2_6, {
			{ link_out_of_service, test_case_2_6, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_7, tgrp_case_2_7, name_case_2_7, desc_case_2_7, sref_case_2_7, {
			{ link_in_service_basic, test_case_2_7, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_2_8, tgrp_case_2_8, name_case_2_8, desc_case_2_8, sref_case_2_8, {
			{ link_in_service_basic, test_case_2_8, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_1, tgrp_case_3_1, name_case_3_1, desc_case_3_1, sref_case_3_1, {
			{ link_out_of_service, test_case_3_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_2, tgrp_case_3_2, name_case_3_2, desc_case_3_2, sref_case_3_2, {
			{ link_in_service_basic, test_case_3_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_3, tgrp_case_3_3, name_case_3_3, desc_case_3_3, sref_case_3_3, {
			{ link_out_of_service, test_case_3_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_4, tgrp_case_3_4, name_case_3_4, desc_case_3_4, sref_case_3_4, {
			{ link_out_of_service, test_case_3_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_5, tgrp_case_3_5, name_case_3_5, desc_case_3_5, sref_case_3_5, {
			{ link_in_service_basic, test_case_3_5, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_6, tgrp_case_3_6, name_case_3_6, desc_case_3_6, sref_case_3_6, {
			{ link_in_service_basic, test_case_3_6, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_7, tgrp_case_3_7, name_case_3_7, desc_case_3_7, sref_case_3_7, {
			{ link_in_service_basic, test_case_3_7, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_3_8, tgrp_case_3_8, name_case_3_8, desc_case_3_8, sref_case_3_8, {
			{ link_in_service_basic, test_case_3_8, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_4_1a, tgrp_case_4_1a, name_case_4_1a, desc_case_4_1a, sref_case_4_1a, {
			{ link_in_service_basic, test_case_4_1a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_4_1b, tgrp_case_4_1b, name_case_4_1b, desc_case_4_1b, sref_case_4_1b, {
			{ link_in_service_basic, test_case_4_1b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_4_2, tgrp_case_4_2, name_case_4_2, desc_case_4_2, sref_case_4_2, {
			{ link_in_service_basic, test_case_4_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_4_3, tgrp_case_4_3, name_case_4_3, desc_case_4_3, sref_case_4_3, {
			{ link_in_service_basic, test_case_4_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_1, tgrp_case_5_1, name_case_5_1, desc_case_5_1, sref_case_5_1, {
			{ link_in_service_basic, test_case_5_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_2, tgrp_case_5_2, name_case_5_2, desc_case_5_2, sref_case_5_2, {
			{ link_in_service_basic, test_case_5_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_3, tgrp_case_5_3, name_case_5_3, desc_case_5_3, sref_case_5_3, {
			{ link_in_service_basic, test_case_5_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_4a, tgrp_case_5_4a, name_case_5_4a, desc_case_5_4a, sref_case_5_4a, {
			{ link_in_service_basic, test_case_5_4a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_4b, tgrp_case_5_4b, name_case_5_4b, desc_case_5_4b, sref_case_5_4b, {
			{ link_in_service_basic, test_case_5_4b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_5a, tgrp_case_5_5a, name_case_5_5a, desc_case_5_5a, sref_case_5_5a, {
			{ link_in_service_basic, test_case_5_5a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_5_5b, tgrp_case_5_5b, name_case_5_5b, desc_case_5_5b, sref_case_5_5b, {
			{ link_in_service_basic, test_case_5_5b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_6_1, tgrp_case_6_1, name_case_6_1, desc_case_6_1, sref_case_6_1, {
			{ link_in_service_basic, test_case_6_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_6_2, tgrp_case_6_2, name_case_6_2, desc_case_6_2, sref_case_6_2, {
			{ link_in_service_basic, test_case_6_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_6_3, tgrp_case_6_3, name_case_6_3, desc_case_6_3, sref_case_6_3, {
			{ link_in_service_basic, test_case_6_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_6_4, tgrp_case_6_4, name_case_6_4, desc_case_6_4, sref_case_6_4, {
			{ link_in_service_basic, test_case_6_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_7_1, tgrp_case_7_1, name_case_7_1, desc_case_7_1, sref_case_7_1, {
			{ link_out_of_service, test_case_7_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_7_2, tgrp_case_7_2, name_case_7_2, desc_case_7_2, sref_case_7_2, {
			{ link_out_of_service, test_case_7_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_7_3, tgrp_case_7_3, name_case_7_3, desc_case_7_3, sref_case_7_3, {
			{ link_out_of_service, test_case_7_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_7_4, tgrp_case_7_4, name_case_7_4, desc_case_7_4, sref_case_7_4, {
			{ link_out_of_service, test_case_7_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_1, tgrp_case_8_1, name_case_8_1, desc_case_8_1, sref_case_8_1, {
			{ link_in_service_basic, test_case_8_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_2, tgrp_case_8_2, name_case_8_2, desc_case_8_2, sref_case_8_2, {
			{ link_in_service_basic, test_case_8_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_3, tgrp_case_8_3, name_case_8_3, desc_case_8_3, sref_case_8_3, {
			{ link_in_service_basic, test_case_8_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_4, tgrp_case_8_4, name_case_8_4, desc_case_8_4, sref_case_8_4, {
			{ link_in_service_basic, test_case_8_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_5, tgrp_case_8_5, name_case_8_5, desc_case_8_5, sref_case_8_5, {
			{ link_in_service_basic, test_case_8_5, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_6, tgrp_case_8_6, name_case_8_6, desc_case_8_6, sref_case_8_6, {
			{ link_in_service_basic, test_case_8_6, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_7, tgrp_case_8_7, name_case_8_7, desc_case_8_7, sref_case_8_7, {
			{ link_in_service_basic, test_case_8_7, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_8, tgrp_case_8_8, name_case_8_8, desc_case_8_8, sref_case_8_8, {
			{ link_in_service_basic, test_case_8_8, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_9a, tgrp_case_8_9a, name_case_8_9a, desc_case_8_9a, sref_case_8_9a, {
			{ link_in_service_basic, test_case_8_9a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_9b, tgrp_case_8_9b, name_case_8_9b, desc_case_8_9b, sref_case_8_9b, {
			{ link_in_service_basic, test_case_8_9b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_10, tgrp_case_8_10, name_case_8_10, desc_case_8_10, sref_case_8_10, {
			{ link_in_service_basic, test_case_8_10, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_11, tgrp_case_8_11, name_case_8_11, desc_case_8_11, sref_case_8_11, {
			{ link_in_service_basic, test_case_8_11, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_12a, tgrp_case_8_12a, name_case_8_12a, desc_case_8_12a, sref_case_8_12a, {
			{ link_in_service_basic, test_case_8_12a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_12b, tgrp_case_8_12b, name_case_8_12b, desc_case_8_12b, sref_case_8_12b, {
			{ link_in_service_basic, test_case_8_12b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_13, tgrp_case_8_13, name_case_8_13, desc_case_8_13, sref_case_8_13, {
			{ link_in_service_basic, test_case_8_13, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_8_14, tgrp_case_8_14, name_case_8_14, desc_case_8_14, sref_case_8_14, {
			{ link_in_service_basic, test_case_8_14, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_1, tgrp_case_9_1, name_case_9_1, desc_case_9_1, sref_case_9_1, {
			{ link_in_service_pcr, test_case_9_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_2, tgrp_case_9_2, name_case_9_2, desc_case_9_2, sref_case_9_2, {
			{ link_in_service_pcr, test_case_9_2, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_3, tgrp_case_9_3, name_case_9_3, desc_case_9_3, sref_case_9_3, {
			{ link_in_service_pcr, test_case_9_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_4, tgrp_case_9_4, name_case_9_4, desc_case_9_4, sref_case_9_4, {
			{ link_in_service_pcr, test_case_9_4, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_5, tgrp_case_9_5, name_case_9_5, desc_case_9_5, sref_case_9_5, {
			{ link_in_service_pcr, test_case_9_5, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_6, tgrp_case_9_6, name_case_9_6, desc_case_9_6, sref_case_9_6, {
			{ link_in_service_pcr, test_case_9_6, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_7, tgrp_case_9_7, name_case_9_7, desc_case_9_7, sref_case_9_7, {
			{ link_in_service_pcr, test_case_9_7, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_8, tgrp_case_9_8, name_case_9_8, desc_case_9_8, sref_case_9_8, {
			{ link_in_service_pcr, test_case_9_8, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_9, tgrp_case_9_9, name_case_9_9, desc_case_9_9, sref_case_9_9, {
			{ link_in_service_pcr, test_case_9_9, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_10, tgrp_case_9_10, name_case_9_10, desc_case_9_10, sref_case_9_10, {
			{ link_in_service_pcr, test_case_9_10, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_11, tgrp_case_9_11, name_case_9_11, desc_case_9_11, sref_case_9_11, {
			{ link_in_service_pcr, test_case_9_11, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_12, tgrp_case_9_12, name_case_9_12, desc_case_9_12, sref_case_9_12, {
			{ link_in_service_pcr, test_case_9_12, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_9_13, tgrp_case_9_13, name_case_9_13, desc_case_9_13, sref_case_9_13, {
			{ link_in_service_pcr, test_case_9_13, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_10_1, tgrp_case_10_1, name_case_10_1, desc_case_10_1, sref_case_10_1, {
			{ link_in_service_basic, test_case_10_1, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_10_2a, tgrp_case_10_2a, name_case_10_2a, desc_case_10_2a, sref_case_10_2a, {
			{ link_in_service_basic, test_case_10_2a, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_10_2b, tgrp_case_10_2b, name_case_10_2b, desc_case_10_2b, sref_case_10_2b, {
			{ link_in_service_basic, test_case_10_2b, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_10_2c, tgrp_case_10_2c, name_case_10_2c, desc_case_10_2c, sref_case_10_2c, {
			{ link_in_service_basic, test_case_10_2c, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
		numb_case_10_3, tgrp_case_10_3, name_case_10_3, desc_case_10_3, sref_case_10_3, {
			{ link_in_service_basic, test_case_10_3, iut_power_off,},
		}, &begin_tests, &end_tests, 0, 0,}, {
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
				if (verbose > 1)
					fprintf(stdout, "\nTest Group: %s", tests[i].tgrp);
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
			if (repeat_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE))
				goto rerun;
			if (repeat_on_success && (result == __RESULT_SUCCESS))
				goto rerun;
			tests[i].result = result;
			if (exit_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE)) {
				aborted = 1;
				continue;
			}
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
    -c, --client\n\
        execute client side of test case only.\n\
    -S, --server\n\
        execute server side of test case only.\n\
    -w, --wait\n\
        have server wait indefinitely.\n\
    -r, --repeat\n\
        repeat test cases on success or failure.\n\
    -R, --repeat-fail\n\
        repeat test cases on failure.\n\
    -p, --client-port [PORT]\n\
        port number from which to connect [default: 10000+index*3]\n\
    -P, --server-port [PORT]\n\
        port number to which to connect or upon which to listen\n\
        [default: 10000+index*3+2]\n\
    -i, --client-host [HOSTNAME[,HOSTNAME]*]\n\
        client host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -I, --server-host [HOSTNAME[,HOSTNAME]*]\n\
        server host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
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

#define HOST_BUF_LEN 128

int
main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;
	char *hostc = "127.0.0.1,127.0.0.2,127.0.0.3";
	char *hosts = "127.0.0.1,127.0.0.2,127.0.0.3";
	char hostbufc[HOST_BUF_LEN];
	char hostbufs[HOST_BUF_LEN];
	struct hostent *haddr;

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
			{"client",	no_argument,		NULL, 'c'},
			{"server",	no_argument,		NULL, 'S'},
			{"wait",	no_argument,		NULL, 'w'},
			{"client-port",	required_argument,	NULL, 'p'},
			{"server-port",	required_argument,	NULL, 'P'},
			{"client-host",	required_argument,	NULL, 'i'},
			{"server-host",	required_argument,	NULL, 'I'},
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

		c = getopt_long(argc, argv, "cSwp:P:i:I:rRd:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "cSwp:P:i:I:rRd:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'c':	/* --client */
			client_exec = 1;
			break;
		case 'S':	/* --server */
			server_exec = 1;
			break;
		case 'w':	/* --wait */
			test_duration = INFINITE_WAIT;
			break;
		case 'p':	/* --client-port */
			client_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[0] = ports[3];
			break;
		case 'P':	/* --server-port */
			server_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[1] = ports[3];
			ports[2] = ports[3] + 1;
			break;
		case 'i':	/* --client-host *//* client host */
			client_host_specified = 1;
			strncpy(hostbufc, optarg, HOST_BUF_LEN);
			hostc = hostbufc;
			break;
		case 'I':	/* --server-host *//* server host */
			server_host_specified = 1;
			strncpy(hostbufs, optarg, HOST_BUF_LEN);
			hosts = hostbufs;
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
						if (verbose > 2)
							fprintf(stdout, "Test Group: %s\n", t->tgrp);
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
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", (long)timer_scale);
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
	exit(do_tests(tests_to_run));
}
