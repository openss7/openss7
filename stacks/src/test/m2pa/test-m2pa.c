/*****************************************************************************

 @(#) $RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/01/26 09:16:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written
 authorization of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that
 the recipient will protect this information and keep it confidential, and
 will not disclose the information contained in this document without the
 written permission of its owner.

 The author reserves the right to revise this software and documentation
 for any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of
 the technical arts, or the reflection of changes in the design of any
 techniques, or procedures embodied, described, or referred to herein.
 The author is under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"),
 it is classified as "Commercial Computer Software" under paragraph
 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations
 ("DFARS") (or any successor regulations) and the Government is acquiring
 only the license rights granted herein (the license rights customarily
 provided to non-Government users).  If the Software is supplied to any unit
 or agency of the Government other than DoD, it is classified as "Restricted
 Computer Software" and the Government's rights in the Software are defined
 in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or
 any success regulations) or, in the cases of NASA, in paragraph 18.52.227-86
 of the NASA Supplement to the FAR (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/01/26 09:16:05 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/01/26 09:16:05 $"

static char const ident[] = "$RCSfile: test-m2pa.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/01/26 09:16:05 $";

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
#include <features.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

/* Its a weird place to get htonl, but on recent GNU headers, asm/byteorder.h no
   longer contains the htonl stuff. */
#include <netinet/in.h>
/* Need this for __constant_htonl and friends */
#include <asm/byteorder.h>

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

#define SUCCESS		 0
#define FAILURE		 1
#define INCONCLUSIVE	-1
#define NOTAPPLICABLE	-2
#define SCRIPTERROR	-3

#define BUFSIZE 300
// #define FFLUSH(_stream)
#define FFLUSH(_stream) fflush((_stream))

#define SHORT_WAIT   10
#define NORMAL_WAIT  100
#define LONG_WAIT    500
#define MAXIMUM_WAIT 1000

#ifndef HZ
#define HZ 100
#endif

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
	      tb_abate_1:128 * 272,
		    /*
		       tb_abate_1 - lev 1 cong abate (#bytes) 
		     */
	      tb_onset_1:256 * 272,
		    /*
		       tb_onset_1 - lev 1 cong onset (#bytes) 
		     */
	      tb_discd_1:384 * 272,
		    /*
		       tb_discd_1 - lev 1 cong discard (#bytes) 
		     */
	      tb_abate_2:512 * 272,
		    /*
		       tb_abate_2 - lev 1 cong abate (#bytes) 
		     */
	      tb_onset_2:640 * 272,
		    /*
		       tb_onset_2 - lev 1 cong onset (#bytes) 
		     */
	      tb_discd_2:768 * 272,
		    /*
		       tb_discd_2 - lev 1 cong discard (#bytes) 
		     */
	      tb_abate_3:896 * 272,
		    /*
		       tb_abate_3 - lev 1 cong abate (#bytes) 
		     */
	      tb_onset_3:1024 * 272,
		    /*
		       tb_onset_3 - lev 1 cong onset (#bytes) 
		     */
	      tb_discd_3:1152 * 272,
		    /*
		       tb_discd_3 - lev 1 cong discard (#bytes) 
		     */
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
 *  Timer values for tests: each timer has a low range (minus error margin) and
 *  a high range (plus error margin).
 */

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
		return FAILURE;
	}
	i = (now.tv_sec - test_start) * 1000;
	i += (now.tv_usec + 500L) / 1000;
	i -= beg;
	printf("                           check timeout     %s [%ld.%03ld <= %ld.%03ld <= %ld.%03ld]\n", t, (lo - 100) / 1000, (lo - 100) - (((lo - 100) / 1000) * 1000), i / 1000, i - ((i / 1000) * 1000), (hi + 100) / 1000, (hi + 100) - (((hi + 100) / 1000) * 1000)
	    );
	FFLUSH(stdout);
	if (lo - 100 <= i && i <= hi + 100)
		return SUCCESS;
	else
		return FAILURE;
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
	act.sa_restorer = NULL;
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

/*
 *  Stop overall test timer.
 */
static int
stop_tt(void)
{
	sigset_t mask;
	struct sigaction act;
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
 *  Definitions of signals and events.
 *
 *  -------------------------------------------------------------------------
 */

#define OUT_OF_SERVICE		1	/* M2PA Status signals */
#define ALIGNMENT		2
#define PROVING_NORMAL		3
#define PROVING_EMERG		4
#define IN_SERVICE		5
#define PROCESSOR_OUTAGE	6
#define PROCESSOR_ENDED		7
#define BUSY			8
#define BUSY_ENDED		9
#define INVALID_STATUS		10
#define SEQUENCE_SYNC		11

#define DATA			20	/* Protocol Tester (PT) signals and events */
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
#define PROVING			21
#endif
#define ACK			22

#define TX_BREAK		30
#define TX_MAKE			31
#define BAD_ACK			32
#define MSU_TOO_SHORT		29

#if 0
#define FISU			20
#define FISU_S			21
#define FISU_CORRUPT		23
#define FISU_CORRUPT_S		24
#define MSU_SEVEN_ONES		27
#define MSU_TOO_LONG		28
#define FISU_FISU_1FLAG		60
#define FISU_FISU_2FLAG		61
#define MSU_MSU_1FLAG		62
#define MSU_MSU_2FLAG		63
#endif

#define TIMEOUT			40	/* TIMER events */
#define COUNT			41
#define TRIES			39
#define ETC			49

#if 0
#define SIB_S			42
#endif

#define IUT_IN_SERVICE		43	/* Implementation Under Test (IUT) events */
#define IUT_OUT_OF_SERVICE	44
#define IUT_RPO			45
#define IUT_RPR			46
#define IUT_DATA		47

#define POWER_ON		200	/* Implementation Under Test (IUT) signals */
#define START			201
#define STOP			202
#define LPO			203
#define LPR			204
#define EMERG			205
#define CEASE			206
#define SEND_MSU		207
#define SEND_MSU_S		208
#define CONG_A			209
#define CONG_D			210
#define NO_CONG			211
#define CLEARB			212
#define SYNC			213

#define CONN_REQ		300	/* MGMT controls */
#define CONN_IND		301
#define CONN_RES		302
#define CONN_CON		303
#define DISCON_REQ		304
#define DISCON_IND		305
#define INFO_REQ		306
#define INFO_ACK		307
#define BIND_REQ		308
#define BIND_ACK		309
#define OK_ACK			310
#define ERROR_ACK		311
#define OPTMGMT_REQ		312

#define ENABLE_REQ		400
#define ENABLE_CON		401
#define DISABLE_REQ		402
#define DISABLE_CON		403
#define ERROR_IND		404

#define UNKNOWN			48

#define NO_MSG			-1

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

/*
   listening file descriptor 
 */
static int mgm_fd = 0;
static ulong mgm_tok = 0;
static ulong mgm_seq = 0;
static unsigned char mgm_buf[BUFSIZE];

/* 
   protocol tester file descriptor
 */
static int pt_fd = 0;
static ulong pt_tok = 0;
static ulong pt_seq = 0;
static unsigned char pt_buf[BUFSIZE];
static unsigned char pt_fib = 0x0;
static unsigned char pt_bib = 0x0;
#if defined(M2PA_VERSION_DRAFT9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
static unsigned int pt_fsn = 0xffffff;
static unsigned int pt_bsn = 0xffffff;
#else
static unsigned int pt_fsn = 0x0;
static unsigned int pt_bsn = 0x0;
#endif
// static unsigned char pt_li = 0;
// static unsigned char pt_sio = 0;

/*
   implementation under test file descriptor 
 */
static int iut_fd = 0;
static ulong iut_tok = 0;
static ulong iut_seq = 0;
static unsigned char iut_buf[BUFSIZE];
static unsigned char iut_fib = 0x0;
static unsigned char iut_bib = 0x0;
#if defined(M2PA_VERSION_DRAFT9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
static unsigned int iut_fsn = 0xffffff;
static unsigned int iut_bsn = 0xffffff;
#else
static unsigned int iut_fsn = 0x0;
static unsigned int iut_bsn = 0x0;
#endif

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
	printf("%23.23s  --%06X, %06X->  %-23.23s\n", l, pt_fsn & 0xffffff, pt_bsn & 0xffffff, r);
#else
	printf("%23.23s  ----------------->  %-23.23s\n", l, r);
	FFLUSH(stdout);
#endif
}
static void
pt_printf(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	if (pt_fsn != 0xffffff || pt_bsn != 0xffffff)
		printf("%23.23s  --%06X, %06X->  %-23.23s\n", l, pt_fsn & 0xffffff, pt_bsn & 0xffffff, r);
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
	case ALIGNMENT:
		if (!cntmsg)
			pt_printf("ALIGNMENT", "");
		status = M2PA_STATUS_ALIGNMENT;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case PROVING_NORMAL:
		if (!cntmsg)
			pt_printf("PROVING-NORMAL", "");
		status = M2PA_STATUS_PROVING_NORMAL;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case PROVING_EMERG:
		if (!cntmsg)
			pt_printf("PROVING-EMERGENCY", "");
		status = M2PA_STATUS_PROVING_EMERGENCY;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case OUT_OF_SERVICE:
		if (!cntmsg)
			pt_printf("OUT-OF-SERVICE", "");
		status = M2PA_STATUS_OUT_OF_SERVICE;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case IN_SERVICE:
		if (!cntmsg)
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			pt_printf("IN-SERVICE", "");
#else
			pt_printf_sn("READY", "");
#endif
		status = M2PA_STATUS_IN_SERVICE;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case SEQUENCE_SYNC:
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
		if (!cntmsg)
			pt_printf_sn("READY", "");
		status = M2PA_STATUS_IN_SERVICE;
		ptconf.qos_data.sid = 1;
		goto pt_status_putmsg;
#else
		return SUCCESS;
#endif
	case PROCESSOR_OUTAGE:
		if (!cntmsg)
			pt_printf("PROCESSOR-OUTAGE", "");
		status = M2PA_STATUS_PROCESSOR_OUTAGE;
#if defined M2PA_VERSION_DRAFT10||defined(M2PA_VERSION_DRAFT11)
		ptconf.qos_data.sid = 1;
#else
		ptconf.qos_data.sid = 0;
#endif
		goto pt_status_putmsg;
	case PROCESSOR_ENDED:
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
	case BUSY_ENDED:
		if (!cntmsg)
			pt_printf("BUSY-ENDED", "");
		status = M2PA_STATUS_BUSY_ENDED;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case BUSY:
		if (!cntmsg)
			pt_printf("BUSY", "");
		status = M2PA_STATUS_BUSY;
		ptconf.qos_data.sid = 0;
		goto pt_status_putmsg;
	case COUNT:
		printf("           Ct = %5d    ----------------->                         \n", count);
		return SUCCESS;
	case TRIES:
		printf("       %5d iterations  ----------------->                         \n", tries);
		FFLUSH(stdout);
		return SUCCESS;
	case ETC:
		printf("                    .    ----------------->                         \n");
		printf("                    .    ----------------->                         \n");
		printf("                    .    ----------------->                         \n");
		FFLUSH(stdout);
		return SUCCESS;
	case INVALID_STATUS:
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
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(pt_bsn);
		((uint16_t *) pt_buf)[5] = htons(pt_fsn);
		((uint32_t *) pt_buf)[3] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(pt_bsn & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(pt_fsn & 0xffffff);
		((uint32_t *) pt_buf)[4] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)
		data.len = 3 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = status;
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#else
		return FAILURE;
#endif
	case MSU_TOO_SHORT:
		msu_len = 1;
		goto send_data;
	case DATA:
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
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		pt_fsn++;
		pt_fsn &= 0xffffff;
		if (!cntmsg) {
			printf("   [%3d bytes]     DATA  --%06X, %06X->                         \n", msu_len, pt_fsn & 0xffffff, pt_bsn & 0xffffff);
			FFLUSH(stdout);
		}
		data.len = 3 * sizeof(uint32_t) + msu_len + 1;
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(pt_bsn);
		((uint16_t *) pt_buf)[5] = htons(pt_fsn);
		memset(&(((uint32_t *) pt_buf)[3]), 'B', msu_len + 1);
		memset(&(((uint32_t *) pt_buf)[3]), 0, 1);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		pt_fsn++;
		pt_fsn &= 0xffffff;
		if (!cntmsg) {
			printf("   [%3d bytes]     DATA  --%06X, %06X->                         \n", msu_len, pt_fsn & 0xffffff, pt_bsn & 0xffffff);
			FFLUSH(stdout);
		}
		data.len = 4 * sizeof(uint32_t) + msu_len + 1;
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(pt_bsn & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(pt_fsn & 0xffffff);
		memset(&(((uint32_t *) pt_buf)[4]), 'B', msu_len + 1);
		memset(&(((uint32_t *) pt_buf)[4]), 0, 1);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#else
		return FAILURE;
#endif
	case BAD_ACK:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		return INCONCLUSIVE;
#else
		pt_fsn = 0xffff;
		goto pt_ack_putmsg;
#endif
	case ACK:
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
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4_1)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint16_t *) pt_buf)[4] = htons(pt_bsn);
		((uint16_t *) pt_buf)[5] = htons(pt_fsn);
		((uint32_t *) pt_buf)[3] = M2PA_STATUS_NONE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT4_9)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htons(pt_bsn & 0xffffff);
		((uint32_t *) pt_buf)[3] = htons(pt_fsn & 0xffffff);
		((uint32_t *) pt_buf)[4] = M2PA_STATUS_NONE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)
	      pt_ack_putmsg:
		pt_printf("IN-SERVICE", "");
		ptconf.qos_data.sid = 0;
		data.len = 5 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.exdata_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_STATUS_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htons(pt_bsn & 0xffffff);
		((uint32_t *) pt_buf)[3] = htons(pt_fsn & 0xffffff);
		((uint32_t *) pt_buf)[4] = M2PA_STATUS_IN_SERVICE;
		p->npi.type = N_EXDATA_REQ;
		bcopy(&ptconf.qos_data, (&p->npi.exdata_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#elif defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	      pt_ack_putmsg:
		printf("               DATA-ACK  --%06X, %06X->                         \n", pt_fsn & 0xffffff, pt_bsn & 0xffffff);
		FFLUSH(stdout);
		ptconf.qos_data.sid = 1;
		data.len = 4 * sizeof(uint32_t);
		ctrl.len = sizeof(p->npi.data_req) + sizeof(ptconf.qos_data);
		((uint32_t *) pt_buf)[0] = M2PA_DATA_MESSAGE;
		((uint32_t *) pt_buf)[1] = htonl(data.len);
		((uint32_t *) pt_buf)[2] = htonl(pt_bsn & 0xffffff);
		((uint32_t *) pt_buf)[3] = htonl(pt_fsn & 0xffffff);
		p->npi.type = N_DATA_REQ;
		p->npi.data_req.DATA_xfer_flags = 0;
		bcopy(&ptconf.qos_data, (&p->npi.data_req) + 1, sizeof(ptconf.qos_data));
		if (putmsg(pt_fd, &ctrl, &data, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
#else
		return INCONCLUSIVE;
#endif
	case STOP:
		printf("                  :stop                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case POWER_ON:
		printf("              :power on                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case LPO:
		printf("               :set lpo                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case LPR:
		printf("             :clear lpo                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case EMERG:
		printf("             :set emerg                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case CEASE:
		printf("           :clear emerg                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case CONG_A:
		printf("        :make congested                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case CONG_D:
		printf("        :make congested                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case NO_CONG:
		printf("       :clear congested                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case START:
		printf("                 :start                                             \n");
		FFLUSH(stdout);
		return SUCCESS;
	case TX_BREAK:
		printf("              :tx break                                             \n");
		FFLUSH(stdout);
		p->npi.type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = 0;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		ctrl.len = sizeof(N_discon_req_t);
		goto pt_control_putmsg;
	case TX_MAKE:
		return SUCCESS;
	case ENABLE_REQ:
		if (verbose > 1) {
			printf("                :enable                                             \n");
			FFLUSH(stdout);
		}
		return SUCCESS;
	case DISABLE_REQ:
		if (verbose > 1) {
			printf("               :disable                                             \n");
			FFLUSH(stdout);
		}
		return SUCCESS;
	case DISCON_REQ:
		printf("            :disconnect                                             \n");
		FFLUSH(stdout);
		p->npi.type = N_DISCON_REQ;
		p->npi.discon_req.DISCON_reason = 0;
		p->npi.discon_req.RES_length = 0;
		p->npi.discon_req.RES_offset = 0;
		p->npi.discon_req.SEQ_number = 0;
		ctrl.len = sizeof(N_discon_req_t);
		goto pt_control_putmsg;
	case BIND_REQ:
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
	case OPTMGMT_REQ:
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
		if (putmsg(pt_fd, &ctrl, NULL, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
	default:
		if (verbose > 1 || !cntmsg) {
			printf("              :????????                                             \n");
			FFLUSH(stdout);
		}
		return FAILURE;
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
	case POWER_ON:
		printf("                                             :power on              \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_POWER_ON_REQ;
		ctrl.len = SL_POWER_ON_REQ_SIZE;
		goto signal_iut_putmsg;
	case START:
		printf("                                             :start                 \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_START_REQ;
		ctrl.len = SL_START_REQ_SIZE;
		goto signal_iut_putmsg;
	case STOP:
		printf("                                             :stop                  \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_STOP_REQ;
		ctrl.len = SL_STOP_REQ_SIZE;
		goto signal_iut_putmsg;
	case LPO:
		printf("                                             :set lpo               \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		ctrl.len = SL_LOCAL_PROC_OUTAGE_REQ_SIZE;
		goto signal_iut_putmsg;
	case LPR:
		printf("                                             :clear lpo             \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_RESUME_REQ;
		ctrl.len = SL_RESUME_REQ_SIZE;
		goto signal_iut_putmsg;
	case CONG_A:
		printf("                                             :make cong accept      \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		ctrl.len = SL_CONG_ACCEPT_REQ_SIZE;
		goto signal_iut_putmsg;
	case CONG_D:
		printf("                                             :make cong discard     \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CONGESTION_DISCARD_REQ;
		ctrl.len = SL_CONG_DISCARD_REQ_SIZE;
		goto signal_iut_putmsg;
	case NO_CONG:
		printf("                                             :clear congestion      \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_NO_CONGESTION_REQ;
		ctrl.len = SL_NO_CONG_REQ_SIZE;
		goto signal_iut_putmsg;
	case CLEARB:
		printf("                                             :clear buffers         \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_CLEAR_BUFFERS_REQ;
		ctrl.len = SL_CLEAR_BUFFERS_REQ_SIZE;
		goto signal_iut_putmsg;
	case EMERG:
		printf("                                             :set emergency         \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_EMERGENCY_REQ;
		ctrl.len = SL_EMERGENCY_REQ_SIZE;
		goto signal_iut_putmsg;
	case CEASE:
		printf("                                             :clear emergency       \n");
		FFLUSH(stdout);
		p->sl.sl_primitive = SL_EMERGENCY_CEASES_REQ;
		ctrl.len = SL_EMERGENCY_CEASES_REQ_SIZE;
	      signal_iut_putmsg:
		if (putmsg(iut_fd, &ctrl, NULL, RS_HIPRI) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			return FAILURE;
		}
		return SUCCESS;
	case COUNT:
		printf("                                                     Ct=%5d       \n", count);
		FFLUSH(stdout);
		return SUCCESS;
	case TRIES:
		printf("                                                     %4d iterations\n", tries);
		FFLUSH(stdout);
		return SUCCESS;
	case ETC:
		printf("                                                     .              \n");
		printf("                                                     .              \n");
		printf("                                                     .              \n");
		FFLUSH(stdout);
		return SUCCESS;
	case SEND_MSU:
		if (!cntact) {
			printf("                                             :msu                   \n");
			FFLUSH(stdout);
		}
	case SEND_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		p->sl.sl_primitive = SL_PDU_REQ;
		ctrl.len = SL_PDU_REQ_SIZE;
		memset(dbuf, 'B', msu_len);
		data.len = msu_len;
		if (putmsg(iut_fd, NULL, &data, 0) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
	case ENABLE_REQ:
		if (verbose > 1) {
			printf("                                             :enable                \n");
			FFLUSH(stdout);
		}
		p->lmi.enable_req.lmi_primitive = LMI_ENABLE_REQ;
		ctrl.len = sizeof(p->lmi.enable_req);
		goto iut_lmi_putmsg;
	case DISABLE_REQ:
		if (verbose > 1) {
			printf("                                             :disable               \n");
			FFLUSH(stdout);
		}
		p->lmi.disable_req.lmi_primitive = LMI_DISABLE_REQ;
		ctrl.len = sizeof(p->lmi.disable_req);
		goto iut_lmi_putmsg;
	      iut_lmi_putmsg:
		if (putmsg(iut_fd, &ctrl, NULL, RS_HIPRI) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
	case CONN_REQ:
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
	case BIND_REQ:
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
	case DISCON_REQ:
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
	case OPTMGMT_REQ:
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
		if (putmsg(iut_fd, &ctrl, NULL, 0) < 0) {
			printf("                                  ****ERROR: putmsg failed!\n");
			printf("                                             %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
	default:
		if (!cntact) {
			printf("                                             <????????              \n");
			FFLUSH(stdout);
		}
		return FAILURE;
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
	case CONN_RES:
		if (verbose > 1) {
			printf("              >conn res                                             \n");
			FFLUSH(stdout);
		}
		p->npi.type = N_CONN_RES;
		p->npi.conn_res.TOKEN_value = pt_tok;
		p->npi.conn_res.RES_length = 0;
		p->npi.conn_res.RES_offset = 0;
		p->npi.conn_res.SEQ_number = mgm_seq;
		p->npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->npi.conn_res.QOS_length = 0;
		p->npi.conn_res.QOS_offset = 0;
		ctrl.len = sizeof(N_conn_res_t);
		goto mgm_control_putmsg;
	case BIND_REQ:
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
	case OPTMGMT_REQ:
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
		if (putmsg(mgm_fd, &ctrl, NULL, 0) < 0) {
			printf("****ERROR: putmsg failed!\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		return SUCCESS;
	default:
		if (!cntmgm) {
			printf("              >???????? |                        \n");
			FFLUSH(stdout);
		}
		return FAILURE;
	}
	return SUCCESS;
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
	printf("%23.23s  <-%06X, %06X--  %-23.23s\n", l, iut_bsn & 0xffffff, iut_fsn & 0xffffff, r);
#else
	printf("%23.23s  <-----------------  %-23.23s\n", l, r);
#endif
	FFLUSH(stdout);
}
static void
iut_printf(char *l, char *r)
{
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
	if (iut_fsn != 0xffffff || iut_bsn != 0xffffff)
		printf("%23.23s  <-%06X, %06X--  %-23.23s\n", l, iut_bsn & 0xffffff, iut_fsn & 0xffffff, r);
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
#elif defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
		uint mystatus = ((uint32_t *) pt_buf)[3];
#elif defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		uint mystatus = ((uint32_t *) pt_buf)[4];
#else
#error "Draft poorly defined."
#endif
		switch (mystatus) {
		case M2PA_STATUS_OUT_OF_SERVICE:
			ret = OUT_OF_SERVICE;
			break;
		case M2PA_STATUS_IN_SERVICE:
			ret = IN_SERVICE;
			break;
		case M2PA_STATUS_PROVING_NORMAL:
			ret = PROVING_NORMAL;
			break;
		case M2PA_STATUS_PROVING_EMERGENCY:
			ret = PROVING_EMERG;
			break;
		case M2PA_STATUS_ALIGNMENT:
			ret = ALIGNMENT;
			break;
		case M2PA_STATUS_PROCESSOR_OUTAGE:
			ret = PROCESSOR_OUTAGE;
			break;
		case M2PA_STATUS_BUSY:
			ret = BUSY;
			break;
		case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
			ret = PROCESSOR_ENDED;
			break;
		case M2PA_STATUS_BUSY_ENDED:
			ret = BUSY_ENDED;
			break;
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)
		case M2PA_STATUS_NONE:
			ret = ACK;
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
			ret = INVALID_STATUS;
			break;
		}
		}
		break;
	}
#if defined(M2PA_VERSION_DRAFT3_1)
	case M2PA_ACK_MESSAGE:
		ret = ACK;
		break;
#endif
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	case M2PA_PROVING_MESSAGE:
		ret = PROVING;
		break;
#endif
	case M2PA_DATA_MESSAGE:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		if (ntohl(((uint32_t *) pt_buf)[1]) == 4 * sizeof(uint32_t)) {
			ret = ACK;
			break;
		}
#endif
		ret = DATA;
		break;
	default:
		ret = FAILURE;
		break;
	}

	if (ret != oldret) {
		if ((ret == DATA || ret == ACK) && !expand)
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
	if (show_fisus || ret != FISU || verbose > 1) {
		if (ret != oldret || oldisb != (((iut_bib | iut_bsn) << 8) | (iut_fib | iut_fsn))) {
			// if ( oldisb == (((iut_bib|iut_bsn)<<8)|(iut_fib|iut_fsn)) &&
			// ( ( ret == FISU && oldret == DATA ) || ( ret == DATA && oldret == FISU ) 
			// ) )
			// {
			// if ( ret == DATA && !expand )
			// cntmsg++;
			// }
			// else
			cntret = 0;
			oldret = ret;
			oldisb = ((iut_bib | iut_bsn) << 8) | (iut_fib | iut_fsn);
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
		iut_bsn = ntohl(((uint32_t *) pt_buf)[2]) >> 16;
		iut_fsn = ntohl(((uint32_t *) pt_buf)[2]) & 0xffff;
#else
		iut_bsn = ntohl(((uint32_t *) pt_buf)[2]) & 0xffffff;
		iut_fsn = ntohl(((uint32_t *) pt_buf)[3]) & 0xffffff;
#endif
		switch (ret) {
		case IN_SERVICE:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			iut_printf("", "IN-SERVICE");
#else
			iut_printf_sn("", "READY");
#endif
			return ret;
		case ALIGNMENT:
			iut_printf("", "ALIGNMENT");
			return ret;
		case PROVING_NORMAL:
			iut_printf("", "PROVING-NORMAL");
			return ret;
		case PROVING_EMERG:
			iut_printf("", "PROVING-EMERGENCY");
			return ret;
		case OUT_OF_SERVICE:
			iut_printf("", "OUT-OF-SERVICE");
			return ret;
		case PROCESSOR_OUTAGE:
			iut_printf("", "PROCESSOR-OUTAGE");
			return ret;
		case BUSY:
			iut_printf("", "BUSY");
			return ret;
		case INVALID_STATUS:
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)
			printf("                         <-----------------  [INVALID STATUS %5u] \n", ntohl(((uint32_t *) pt_buf)[2]));
#elif defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)
			printf("                         <-%06X, %06X--  [INVALID STATUS %5u] \n", iut_bsn & 0xffffff, iut_fsn & 0xffffff, ntohl(((uint32_t *) pt_buf)[3]));
#elif defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)||defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
			printf("                         <-%06X, %06X--  [INVALID STATUS %5u] \n", iut_bsn & 0xffffff, iut_fsn & 0xffffff, ntohl(((uint32_t *) pt_buf)[4]));
#else
#error "Poorly defined version."
#endif
			FFLUSH(stdout);
			return ret;
		case PROCESSOR_ENDED:
#if defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
			iut_printf_sn("", "PROCESSOR-RECOVERED");
#else
			iut_printf("", "PROCESSOR-OUTAGE-ENDED");
#endif
			return ret;
		case BUSY_ENDED:
			iut_printf("", "BUSY-ENDED");
			return ret;
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)||defined(M2PA_VERSION_DRAFT5)||defined(M2PA_VERSION_DRAFT5_1)
		case ACK:
			iut_printf_sn("", "IN-SERVICE");
			return ret;
#elif defined(M2PA_VERSION_DRAFT3_1)
		case ACK:
			printf("                         <-----------------  ACK [%5u msgs]       \n", ntohl(((uint32_t *) pt_buf)[2]));
			FFLUSH(stdout);
			return ret;
#elif defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT7)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
		case ACK:
			printf("                         <-%06X, %06X--  DATA-ACK               \n", iut_bsn & 0xffffff, iut_fsn & 0xffffff);
			FFLUSH(stdout);
			return ret;
#endif
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
		case PROVING:
			printf("                         <-----------------  PROVING [%5u bytes]  \n", ntohl(((uint32_t *) pt_buf)[1]) - 2 * sizeof(uint32_t));
			FFLUSH(stdout);
			return ret;
#endif
		case DATA:
#if defined(M2PA_VERSION_DRAFT4)||defined(M2PA_VERSION_DRAFT4_1)||defined(M2PA_VERSION_DRAFT4_9)
			pt_bsn = iut_fsn;
#endif
			if (show_msus || verbose > 1) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
				printf("                         <-----------------  DATA [%5u bytes]     \n", iut_bsn, iut_fsn, ntohl(((uint32_t *) pt_buf)[1]) - 2 * sizeof(uint32_t));
#else
				printf("                         <-%06X, %06X--  DATA [%5u bytes]     \n", iut_bsn & 0xffffff, iut_fsn & 0xffffff, ntohl(((uint32_t *) pt_buf)[1]) - 4 * sizeof(uint32_t));
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
pt_decode_msg(unsigned char *buf)
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
		pt_seq = p->npi.conn_ind.SEQ_number;
		return UNKNOWN;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("              !conn con                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("            !discon ind                                             \n");
			FFLUSH(stdout);
		}
		return DISCON_IND;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("              !info ack                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
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
		pt_tok = p->npi.bind_ack.TOKEN_value;
		return BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("             !error ack                                             \n");
			FFLUSH(stdout);
		}
		return ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("                !ok ack                                             \n");
			FFLUSH(stdout);
		}
		return OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("          !unitdata ind                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("           !uderror ind                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("            !datack ind                                             \n");
			FFLUSH(stdout);
		}
#if defined(M2PA_VERSION_DRAFT3_1)
		return ACK;
#else
		return UNKNOWN;
#endif
	case N_RESET_IND:
		if (verbose > 1) {
			printf("             !reset ind                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("             !reset con                                             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	default:
		printf("         !(unknown %3ld)                                             \n", (long)p->npi.type);
		FFLUSH(stdout);
		return UNKNOWN;
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
	return IUT_DATA;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode ctrl at Implementation Under Test
 *
 *  -------------------------------------------------------------------------
 */
static int
iut_decode_msg(unsigned char *buf)
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
		return IUT_RPO;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		if (!cntprm) {
			printf("                                             !rpr                   \n");
			FFLUSH(stdout);
		}
		return IUT_RPR;
	case SL_IN_SERVICE_IND:
		printf("                                             !in service            \n");
		FFLUSH(stdout);
		return IUT_IN_SERVICE;
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
		return IUT_OUT_OF_SERVICE;
	case SL_PDU_IND:
		printf("                                             !msu                   \n");
		FFLUSH(stdout);
		return IUT_DATA;
	case SL_LINK_CONGESTED_IND:
		printf("                                             !congested             \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_LINK_CONGESTION_CEASED_IND:
		printf("                                             !congestion ceased     \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_RETRIEVED_MESSAGE_IND:
		printf("                                             !retrieved message     \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_RETRIEVAL_COMPLETE_IND:
		printf("                                             !retrieval compelte    \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_RB_CLEARED_IND:
		printf("                                             !rb cleared            \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_BSNT_IND:
		printf("                                             !bsnt                  \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case SL_RTB_CLEARED_IND:
		printf("                                             !rtb cleared           \n");
		FFLUSH(stdout);
		return UNKNOWN;
	case LMI_INFO_ACK:
		if (verbose > 1) {
			printf("                                             !info ack              \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case LMI_OK_ACK:
		if (verbose > 1) {
			printf("                                             !ok ack                \n");
			FFLUSH(stdout);
		}
		return OK_ACK;
	case LMI_ERROR_ACK:
		if (verbose > 1) {
			printf("                                             !error ack             \n");
			FFLUSH(stdout);
		}
		return ERROR_ACK;
	case LMI_ENABLE_CON:
		if (verbose > 1) {
			printf("                                             !enable con            \n");
			FFLUSH(stdout);
		}
		return ENABLE_CON;
	case LMI_DISABLE_CON:
		if (verbose > 1) {
			printf("                                             !disable con           \n");
			FFLUSH(stdout);
		}
		return DISABLE_CON;
	case LMI_ERROR_IND:
		if (verbose > 1) {
			printf("                                             !error ind             \n");
			FFLUSH(stdout);
		}
		return ERROR_IND;
	case LMI_STATS_IND:
		if (verbose > 1) {
			printf("                                             !stats ind             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case LMI_EVENT_IND:
		if (verbose > 1) {
			printf("                                             !event ind             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_CONN_IND:
		if (verbose > 1) {
			printf("                                             !conn ind              \n");
			FFLUSH(stdout);
		}
		iut_seq = p->npi.conn_ind.SEQ_number;
		return UNKNOWN;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("                                             !conn con              \n");
			FFLUSH(stdout);
		}
		return CONN_CON;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("                                             !discon ind            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("                                             !info ack              \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
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
		iut_tok = p->npi.bind_ack.TOKEN_value;
		return BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("                                             !error ack             \n");
			FFLUSH(stdout);
		}
		return ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("                                             !ok ack                \n");
			FFLUSH(stdout);
		}
		return OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("                                             !unitdata ind          \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("                                             !uderror ind           \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("                                             !datack ind            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_RESET_IND:
		if (verbose > 1) {
			printf("                                             !reset ind             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("                                             !reset con             \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	default:
		printf("                                             !(unknown %5ld)       \n", p->sl.sl_primitive);
		FFLUSH(stdout);
		return UNKNOWN;
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
	return UNKNOWN;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode ctrl at Management Stream
 *
 *  -------------------------------------------------------------------------
 */
static int
mgm_decode_msg(unsigned char *buf)
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
		mgm_seq = p->npi.conn_ind.SEQ_number;
		return CONN_IND;
	case N_CONN_CON:
		if (verbose > 1) {
			printf("             !conn con |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_DISCON_IND:
		if (verbose > 1) {
			printf("           !discon ind |                                            \n");
			FFLUSH(stdout);
		}
		return DISCON_IND;
	case N_INFO_ACK:
		if (verbose > 1) {
			printf("             !info ack |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
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
		mgm_tok = p->npi.bind_ack.TOKEN_value;
		return BIND_ACK;
	case N_ERROR_ACK:
		if (verbose > 1) {
			printf("            !error ack |                                            \n");
			FFLUSH(stdout);
		}
		return ERROR_ACK;
	case N_OK_ACK:
		if (verbose > 1) {
			printf("               !ok ack |                                            \n");
			FFLUSH(stdout);
		}
		return OK_ACK;
	case N_UNITDATA_IND:
		if (verbose > 1) {
			printf("         !unitdata ind |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_UDERROR_IND:
		if (verbose > 1) {
			printf("          !uderror ind |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_DATACK_IND:
		if (verbose > 1) {
			printf("           !datack ind |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_RESET_IND:
		if (verbose > 1) {
			printf("            !reset ind |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	case N_RESET_CON:
		if (verbose > 1) {
			printf("            !reset con |                                            \n");
			FFLUSH(stdout);
		}
		return UNKNOWN;
	default:
		printf("      !(uninown %5ld) |                                            \n", (long)p->npi.type);
		FFLUSH(stdout);
		return UNKNOWN;
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
			{pt_fd, POLLIN | POLLPRI, 0},
			{iut_fd, POLLIN | POLLPRI, 0},
			{mgm_fd, POLLIN | POLLPRI, 0}
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
			return TIMEOUT;
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
			return NO_MSG;
		case 1:
		case 2:
		case 3:
			if (debug && verbose > 1) {
				printf("                         ^^^^^^polled^^^^^^                         \n");
				FFLUSH(stdout);
			}
			if (pfd[0].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = UNKNOWN;
				int flags = 0;
				unsigned char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, pt_buf};
				if (debug && verbose > 1) {
					printf("pt getmsg...                                                        \n");
					FFLUSH(stdout);
				}
				if (getmsg(pt_fd, &ctrl, &data, &flags) < 0) {
					printf("****ERROR: pt getmsg failed\n");
					printf("           %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("pt gotmsg... [%2d,%2d]                                                \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = pt_decode_msg(ctrl.buf)) != UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = pt_decode_data()) != UNKNOWN)
						return ret;
				}
			}
			if (pfd[1].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = UNKNOWN;
				int flags = 0;
				unsigned char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, iut_buf};
				if (debug && verbose > 1) {
					printf("                                             iut getmsg             \n");
					FFLUSH(stdout);
				}
				if (getmsg(iut_fd, &ctrl, &data, &flags) < 0) {
					printf("                                   ****ERROR: iut getmsg failed\n");
					printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("                                             iut gotmsg [%2d,%2d]     \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = iut_decode_msg(ctrl.buf)) != UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = iut_decode_data()) != UNKNOWN)
						return ret;
				}
			}
			if (pfd[2].revents & (POLLIN | POLLPRI | POLLERR | POLLHUP)) {
				int ret = UNKNOWN;
				int flags = 0;
				unsigned char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf }, data = {
				BUFSIZE, 0, mgm_buf};
				if (debug && verbose > 1) {
					printf("  mgm getmsg...                                                     \n");
					FFLUSH(stdout);
				}
				if (getmsg(mgm_fd, &ctrl, &data, &flags) < 0) {
					printf("  ****ERROR: mgm getmsg failed\n");
					printf("             %s: %s\n", __FUNCTION__, strerror(errno));
					FFLUSH(stdout);
					return NO_MSG;
				} else {
					if (debug && verbose > 1) {
						printf("  mgm gotmsg... [%2d,%2d]                                             \n", ctrl.len, data.len);
						FFLUSH(stdout);
					}
					if (ctrl.len > 0 && (ret = mgm_decode_msg(ctrl.buf)) != UNKNOWN)
						return ret;
					if (data.len > 0 && (ret = mgm_decode_data()) != UNKNOWN)
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
#if 0
static int
check_snibs(unsigned char bsnib, unsigned char fsnib)
{
	printf("                 check b/f sn/ib:  ---> (%02x/%02x)\n", bsnib, fsnib);
	FFLUSH(stdout);
	if ((iut_bib | iut_bsn) == bsnib && (iut_fib | iut_fsn) == fsnib)
		return SUCCESS;
	return FAILURE;
}
#endif

#define desc_case_1_1a "\
Link State Control - Expected signal units/orders\n\
Initialization (Power-up)"
static int
test_case_1_1a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(POWER_ON);
			send(OUT_OF_SERVICE);
			signal(POWER_ON);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
			case NO_MSG:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case ALIGNMENT:
				start_tt(iutconf.sl.t2 * 20);
				beg_time = milliseconds(t2);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T2  ", beg_time, timer[t2].lo, timer[t2].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				start_tt(iutconf.sl.t3 * 20);
				beg_time = milliseconds(t3);
				state = 4;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T3  ", beg_time, timer[t3].lo, timer[t3].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				beg_time = milliseconds(t4n);
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 4;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				if (check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
					return FAILURE;
				start_tt(iutconf.sl.t1 * 20);
				beg_time = milliseconds(t1);
				state = 5;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T1  ", beg_time, timer[t1].lo, timer[t1].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				expand = 1;
				return FAILURE;
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				start_tt(1000);
				state = 3;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				expand = 1;
				return FAILURE;
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				start_tt(1000);
				state = 3;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					break;
				}
				return FAILURE;
#endif
			case IN_SERVICE:
				expand = 1;
				send(DATA);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case ACK:
			case IUT_IN_SERVICE:
			case IUT_DATA:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 / 2);
				show_timeout = 1;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case TIMEOUT:
				start_tt(iutconf.sl.t4n * 20);
				send(ALIGNMENT);
				beg_time = milliseconds(t4n);
				send(PROVING_NORMAL);
				expand = 0;
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				send(IN_SERVICE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(LPO);
				send(START);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case IN_SERVICE:
				expand = 1;
				send(PROCESSOR_OUTAGE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(1000);
				send(DATA);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(LPO);
				send(START);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				expand = 0;
				signal(SEND_MSU);
				send(PROVING_NORMAL);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_RPO:
				expand = 1;
				break;
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
			case TIMEOUT:
			case IN_SERVICE:
			case DATA:
				expand = 1;
				start_tt(1000);
				send(PROCESSOR_OUTAGE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case DATA:
				send(PROCESSOR_OUTAGE);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(LPR);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case IN_SERVICE:
				expand = 1;
				start_tt(1000);
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(LPO);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
			case PROCESSOR_OUTAGE:
				expand = 1;
				send(PROCESSOR_OUTAGE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				send(STOP);
				start_tt(1000);
				send(OUT_OF_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(LPO);
				send(START);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case IN_SERVICE:
				expand = 1;
				signal(STOP);
				send(IN_SERVICE);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(1000);
				send(ALIGNMENT);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case PROCESSOR_OUTAGE:
				send(ALIGNMENT);
				break;
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				expand = 0;
				signal(LPO);
				start_tt(iutconf.sl.t4n * 10 / 2);
				send(PROVING_NORMAL);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case TIMEOUT:
				expand = 1;
				signal(LPR);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				break;
			case TIMEOUT:
				expand = 1;
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case IN_SERVICE:
				expand = 1;
				signal(LPO);
				start_tt(1000);
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;
			case PROCESSOR_OUTAGE:
				start_tt(5000);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case TIMEOUT:
				signal(LPR);
				start_tt(5000);
				state = 6;
				break;
			default:
				return FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case PROCESSOR_ENDED:
				send(SEQUENCE_SYNC);
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				start_tt(iutconf.sl.t1 * 20);
				beg_time = milliseconds(t1);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T1  ", beg_time, timer[t1].lo, timer[t1].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				beg_time = dual_milliseconds(t3, t4n);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T3,4", beg_time, timer[t4n].lo, timer[t3].hi + timer[t4n].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(EMERG);
				signal(CEASE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				beg_time = milliseconds(t4n);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				signal(EMERG);
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4e * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_EMERG:
				send(PROVING_EMERG);
				beg_time = milliseconds(t4e);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_EMERG:
				send(PROVING_EMERG);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				signal(EMERG);
				expand = 0;
				send(ALIGNMENT);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				beg_time = milliseconds(t4n);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(ALIGNMENT);
				expand = 0;
				break;
			case PROVING_EMERG:
				send(PROVING_NORMAL);
				beg_time = milliseconds(t4n);
				expand = 0;
				state = 4;
				break;
#endif
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_EMERG:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(EMERG);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4e * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_EMERG:
				beg_time = milliseconds(t4e);
				send(PROVING_EMERG);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_EMERG:
				send(PROVING_EMERG);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(EMERG);
				send(START);
				send(ALIGNMENT);
				signal(START);
				start_tt(iutconf.sl.t4e * 20);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(PROVING_EMERG);
				expand = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				beg_time = milliseconds(t4e);
				state = 3;
				expand = 0;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				signal(EMERG);
				start_tt(iutconf.sl.t4e * 20);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case PROVING_EMERG:
				beg_time = milliseconds(t4e);
				state = 4;
				expand = 0;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_EMERG:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(EMERG);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 20);
				expand = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_EMERG:
				beg_time = milliseconds(t4e);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_EMERG:
				send(PROVING_EMERG);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				return check_time("T4  ", beg_time, timer[t4e].lo, timer[t4e].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				show_timeout = 1;
				start_tt(iutconf.sl.t2 * 10 - 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				break;
			case TIMEOUT:
				signal(STOP);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t3 * 8);	/* 80% of T3 */
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				expand = 0;
				break;
			case TIMEOUT:
				expand = 1;
				signal(STOP);
				start_tt(iutconf.sl.t4n * 10);
				state = 3;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				if (proving)
					state = 2;
				else
					state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				if (proving) {
					send(PROVING);
					expand = 0;
					break;
				}
				return FAILURE;
#else
			case PROVING_NORMAL:
				if (proving) {
					send(PROVING_NORMAL);
					expand = 0;
					break;
				}
				return FAILURE;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				signal(STOP);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(ALIGNMENT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(STOP);
			send(OUT_OF_SERVICE);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(STOP);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case PROCESSOR_OUTAGE:
				signal(STOP);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case NO_MSG:
				send(PROCESSOR_OUTAGE);
				send(STOP);
				send(OUT_OF_SERVICE);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(SHORT_WAIT))) {
			case IUT_RPO:
				break;	/* stupid ITU-T SDLs */
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(PROCESSOR_OUTAGE);
			signal(STOP);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case PROCESSOR_OUTAGE:
				send(STOP);
				send(OUT_OF_SERVICE);
				return SUCCESS;
			case IUT_RPR:
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case TIMEOUT:
				expand = 1;
				send(STOP);
				send(OUT_OF_SERVICE);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
#else
			case PROVING_NORMAL:
#endif
				expand = 0;
				send(OUT_OF_SERVICE);
				break;
			case OUT_OF_SERVICE:
				expand = 1;
				break;
			case IUT_OUT_OF_SERVICE:
				expand = 1;
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(OUT_OF_SERVICE);
			send(START);
			signal(START);
			send(ALIGNMENT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case ALIGNMENT:
				send(PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 20);
				expand = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case TIMEOUT:
				expand = 1;
				signal(STOP);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case OUT_OF_SERVICE:
				expand = 1;
				send(OUT_OF_SERVICE);
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(ALIGNMENT);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(STOP);
				send(OUT_OF_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(LPO);
				send(PROCESSOR_OUTAGE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				send(ALIGNMENT);
				send(PROVING_NORMAL);
				send(PROVING_EMERG);
				send(PROCESSOR_OUTAGE);
				send(BUSY);
				send(INVALID_STATUS);
				send(PROCESSOR_ENDED);
				send(IN_SERVICE);
				send(BUSY_ENDED);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(ACK);
#endif
				send(DATA);
				signal(STOP);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				oldmsg = 0;
				cntmsg = 0;	/* force display OUT_OF_SERVICE */
				send(OUT_OF_SERVICE);
				send(PROCESSOR_OUTAGE);
				send(BUSY);
				send(INVALID_STATUS);
				send(IN_SERVICE);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(ACK);
#endif
				send(DATA);
				signal(CEASE);
				signal(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				oldmsg = 0;
				cntmsg = 0;	/* force display ALIGNMENT */
				send(ALIGNMENT);
				send(PROCESSOR_OUTAGE);
				send(BUSY);
				send(INVALID_STATUS);
				send(IN_SERVICE);
				send(PROCESSOR_ENDED);
				send(BUSY_ENDED);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(ACK);
#endif
				send(DATA);
				signal(CEASE);
				signal(START);
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				send(PROCESSOR_ENDED);
				send(PROCESSOR_OUTAGE);
				send(BUSY_ENDED);
				send(BUSY);
				send(INVALID_STATUS);
				send(IN_SERVICE);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				send(ACK);
#endif
				send(DATA);
				signal(CEASE);
				signal(START);
				state = 3;
				expand = 0;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(BUSY);
				send(INVALID_STATUS);
				signal(EMERG);
				signal(CEASE);
				signal(LPR);
				signal(START);
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				send(BUSY);
				send(INVALID_STATUS);
				signal(EMERG);
				signal(CEASE);
				// signal(LPR); // spec says LPR, but it doesn't make sense...
				signal(LPO);
				signal(START);
				send(IN_SERVICE);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
			case IUT_RPR:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			send(INVALID_STATUS);
			signal(EMERG);
			signal(CEASE);
			signal(LPR);
			signal(START);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case PROCESSOR_OUTAGE:
				send(BUSY);
				send(INVALID_STATUS);
				signal(EMERG);
				signal(CEASE);
				signal(START);
				send(IN_SERVICE);
				send(PROCESSOR_ENDED);
				send(BUSY_ENDED);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OK_ACK:
				break;
			case IUT_OUT_OF_SERVICE:	/* FIXME */
				send(TX_MAKE);
				return SUCCESS;
			default:
				send(TX_MAKE);
				return FAILURE;
			}
			break;
		default:
			send(TX_MAKE);
			return SCRIPTERROR;
		}
	}
}

#define desc_case_3_2 "\
Transmission Failure\n\
Link aligned ready (Corrupt FIBs - Basic)\
"
static int
test_case_3_2(void)
{
#if defined M2PA_VERSION_DRAFT11
	return NOTAPPLICABLE;
#endif
	for (;;) {
		switch (state) {
		case 0:
			send(BAD_ACK);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(BAD_ACK);
				start_tt(1000);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case OK_ACK:
				break;
			case IUT_OUT_OF_SERVICE:	/* FIXME */
				send(TX_MAKE);
				return SUCCESS;
			default:
				send(TX_MAKE);
				return FAILURE;
			}
			break;
		default:
			send(TX_MAKE);
			return SCRIPTERROR;
		}
	}
}

#define desc_case_3_4 "\
Transmission Failure\n\
Link aligned not ready (Corrupt FIBs - Basic)\
"
static int
test_case_3_4(void)
{
#if defined M2PA_VERSION_DRAFT11
	return NOTAPPLICABLE;
#endif
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 20);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
#endif
			case PROCESSOR_OUTAGE:
				expand = 1;
				send(BAD_ACK);
				start_tt(1000);
				state = 4;
				break;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(BAD_ACK);
				start_tt(1000);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			if (send(TX_BREAK))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OK_ACK:
				break;
			case IUT_OUT_OF_SERVICE:	/* FIXME */
				send(TX_MAKE);
				return SUCCESS;
			default:
				send(TX_MAKE);
				return FAILURE;
			}
			break;
		default:
			send(TX_MAKE);
			return SCRIPTERROR;
		}
	}
}

#define desc_case_3_6 "\
Transmission Failure\n\
Link in service (Corrupt FIBs - Basic)\
"
static int
test_case_3_6(void)
{
#if defined M2PA_VERSION_DRAFT11
	return NOTAPPLICABLE;
#endif
	for (;;) {
		switch (state) {
		case 0:
			send(BAD_ACK);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(BAD_ACK);
				start_tt(1000);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case PROCESSOR_OUTAGE:
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OK_ACK:
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case IUT_OUT_OF_SERVICE:	/* FIXME */
				send(TX_MAKE);
				return SUCCESS;
			default:
				send(TX_MAKE);
				return FAILURE;
			}
			break;
		default:
			send(TX_MAKE);
			return SCRIPTERROR;
		}
	}
}

#define desc_case_3_8 "\
Transmission Failure\n\
Link in processor outage (Corrupt FIBs - Basic)\
"
static int
test_case_3_8(void)
{
#if defined M2PA_VERSION_DRAFT11
	return NOTAPPLICABLE;
#endif
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case PROCESSOR_OUTAGE:
				send(BAD_ACK);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(BAD_ACK);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			send(DATA);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				if (++msu == 1) {
					signal(LPO);
					send(DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
				pt_bsn = iut_fsn;
				send(ACK);
				break;
			case ACK:
				if (++dat <= 1)
					break;
				return FAILURE;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (++msu == 2) {
					pt_bsn++;
					send(ACK);
					state = 3;
					break;
				}
				break;
			case ACK:
				if (++dat <= 1)
					break;
				return FAILURE;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROCESSOR_OUTAGE:
				send(DATA);
				start_tt(1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_DATA:
				if (++ind <= dat)
					break;
				return (FAILURE);
			case TIMEOUT:
				signal(CLEARB);
				signal(LPR);
				signal(SEND_MSU);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROCESSOR_ENDED:
				send(DATA);
				pt_fsn = iut_bsn;
				send(SEQUENCE_SYNC);
				send(DATA);
				start_tt(1000);
				state = 6;
				break;
			default:
				return FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = iut_fsn;
				send(ACK);
				break;
			case IUT_DATA:
				break;
			case ACK:
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 9:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			signal(SEND_MSU);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case DATA:
				if (++count == 2) {
					signal(LPO);
					send(DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				break;
			case ACK:
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case ACK:
				break;
			case PROCESSOR_OUTAGE:
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;
			case TIMEOUT:
				signal(CLEARB);
#if !defined M2PA_VERSION_DRAFT11
				signal(SEND_MSU);
#endif
				start_tt(1000);
				signal(LPR);
#if defined M2PA_VERSION_DRAFT11
				signal(SEND_MSU);
				pt_bsn = iut_fsn;
				send(ACK);
				state = 5;
#else
				state = 4;
#endif
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;
			case DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROCESSOR_ENDED:
				send(SEQUENCE_SYNC);
#if defined M2PA_VERSION_DRAFT11
				send(DATA);
				signal(SEND_MSU);
				state = 6;
				break;
#endif
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
#if defined M2PA_VERSION_DRAFT11
		case 6:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = iut_fsn;
				send(ACK);
				state = 7;
				break;
			case IUT_DATA:
				state = 8;
				break;
			case ACK:
				break;
			default:
				return FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			case IUT_DATA:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = iut_fsn;
				send(ACK);
				return SUCCESS;
			case ACK:
				break;
			default:
				return FAILURE;
			}
			break;
#endif
		default:
			return SCRIPTERROR;
		}
	}
}
#endif

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
			signal(SEND_MSU);
			send(DATA);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				if (++msu == 2) {
					pt_bsn++;
					send(ACK);
					send(PROCESSOR_OUTAGE);
					send(DATA);
					signal(SEND_MSU);
					start_tt(iutconf.sl.t7 * 10);
					state = 2;
					break;
				}
				break;
			case ACK:
				if (++dat <= 1)
					break;
				return FAILURE;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case TIMEOUT:
				send(PROCESSOR_ENDED);
				send(DATA);
				start_tt(iutconf.sl.t7 * 10 / 2);
				state = 3;
				break;
			case ACK:
				break;
			case IUT_DATA:
				break;
			case IUT_RPO:
				break;
			case DATA:
				start_tt(iutconf.sl.t7 * 10);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			case DATA:
				pt_bsn = iut_fsn;
				send(ACK);
				break;
			case ACK:
				break;
			case IUT_DATA:
				break;
			case IUT_RPR:
				break;
			case IN_SERVICE:
				pt_fsn = iut_bsn;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 7:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 8:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		case 9:
			switch ((event = get_event())) {
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
			signal(SEND_MSU);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case DATA:
				if (++count == 2) {
					signal(LPO);
					send(DATA);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				break;
			case ACK:
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case ACK:
				break;
			case PROCESSOR_OUTAGE:
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;
			case TIMEOUT:
				signal(CLEARB);
				signal(SEND_MSU);
				start_tt(1000);
				signal(LPR);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;
			case DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROCESSOR_ENDED:
				send(SEQUENCE_SYNC);
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}
#endif

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
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPR:
				break;	/* stupid ITU-T SDLs */
			case PROCESSOR_OUTAGE:
				send(PROCESSOR_OUTAGE);
				send(PROCESSOR_ENDED);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case IUT_RPR:
			case IUT_RPO:
				break;
			case IN_SERVICE:
			case NO_MSG:
				send(LPR);
				send(PROCESSOR_ENDED);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IN_SERVICE:
			case IUT_RPO:
				break;
			case IUT_RPR:
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(LPO);
			send(LPO);
			send(PROCESSOR_OUTAGE);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case PROCESSOR_OUTAGE:
				signal(LPR);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPO:
				send(LPR);
				send(PROCESSOR_ENDED);
				state = 3;
				break;
			case PROCESSOR_ENDED:
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IN_SERVICE:
				break;
			case IUT_RPR:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_5_1 "\
SU delimitation, alignment, error detection and correction\n\
More than 7 ones between MSU opening and closing flags\
"
static int
test_case_5_1(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	unsigned char old_bsn = 0x7f;
	for (;;) {
		switch (state) {
		case 0:
			old_bsn = iut_bsn;
			if (send(MSU_SEVEN_ONES))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case FISU:
				if (iut_bsn != old_bsn)
					return FAILURE;
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			case IUT_DATA:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_5_2 "\
SU delimitation, alignment, error detection and correction\n\
Greater than maximum signal unit length\
"
static int
test_case_5_2(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(MSU_TOO_LONG);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case FISU:
				if (iut_bsn != 0x7f)
					return check_snibs(0x7f, 0xff);
				send(FISU);
				break;
			case TIMEOUT:
				return check_snibs(0x7f, 0xff);
			case IUT_DATA:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
			send(MSU_TOO_SHORT);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			case ACK:
			case IUT_DATA:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_5_4a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Forward direction\
"
static int
test_case_5_4a(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(FISU_FISU_1FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_5_4b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between FISUs\n\
Reverse direction\
"
static int
test_case_5_4b(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(FISU_FISU_2FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_5_5a "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Forward direction\
"
static int
test_case_5_5a(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(MSU_MSU_1FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_5_5b "\
SU delimitation, alignment, error detection and correction\n\
Reception of single and multiple flags between MSUs\n\
Reverse direction\
"
static int
test_case_5_5b(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(MSU_MSU_2FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_6_1 "\
SUERM check\n\
Error rate of 1 in 256 - Link remains in service\
"
static int
test_case_6_1(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(FISU_CORRUPT);
			stop_tt();
			count = 255;
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				if (tries < 8192) {
					int i;
					if (tries) {
						send(FISU_CORRUPT_S);
						for (i = 0; i < count; i++)
							send(FISU_S);
					} else {
						send(FISU_CORRUPT);
						send(FISU);
						for (i = 1; i < count; i++)
							send(FISU_S);
						send(COUNT);
						send(ETC);
					}
					tries++;
					break;
				}
				send(TRIES);
				return SUCCESS;
			default:
				send(TRIES);
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_6_2 "\
SUERM check\n\
Error rate of 1 in 254 - Link out of service\
"
static int
test_case_6_2(void)
{
	return NOTAPPLICABLE;	/* can't do this */
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
			case FISU:
			case NO_MSG:
				if (tries < 8192) {
					int i;
					if (tries) {
						send(FISU_CORRUPT_S);
						for (i = 0; i < count; i++)
							send(FISU_S);
					} else {
						send(FISU_CORRUPT);
						send(FISU);
						for (i = 1; i < count; i++)
							send(FISU_S);
						send(COUNT);
						send(ETC);
					}
					tries++;
					break;
				}
				send(TRIES);
				return FAILURE;
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				send(TRIES);
				return SUCCESS;
			default:
				send(TRIES);
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_6_3 "\
SUERM check\n\
Consequtive corrupt SUs\
"
static int
test_case_6_3(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			count++;
			send(FISU_CORRUPT);
			stop_tt();
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				count++;
				send(FISU_CORRUPT_S);
				if (count > 128) {
					send(COUNT);
					return FAILURE;
				}
				break;
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				if (count > 1)
					send(COUNT);
				if (count > 70)
					return FAILURE;
				else
					return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_6_4 "\
SUERM check\n\
Time controlled break of the link\
"
static int
test_case_6_4(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			if (send(TX_BREAK))
				return INCONCLUSIVE;
			start_tt(100);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case TIMEOUT:
				send(TX_MAKE);
				start_tt(1000);
				state = 2;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_IN_SERVICE:
				break;
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				send(TX_MAKE);
				return SUCCESS;
			default:
				send(TX_MAKE);
				return FAILURE;
			}
			break;
		default:
			send(TX_MAKE);
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_7_1 "\
AERM check\n\
Error rate below the normal threshold\
"
static int
test_case_7_1(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case ALIGNMENT:
				send(ALIGNMENT);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				break;
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				beg_time = milliseconds(t4);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case NO_MSG:
			case PROVING_NORMAL:
				if (count < iutconf.sdt.Tin - 1) {
					send(INVALID_STATUS);
					count++;
				} else {
					send(COUNT);
					send(PROVING_NORMAL);
					start_tt(iutconf.sl.t4n * 10 / 2 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case FISU:
				expand = 1;
				send(FISU);
				return SUCCESS;
			case TIMEOUT:
				expand = 1;
				return FAILURE;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_7_2 "\
AERM check\n\
Error rate at the normal threshold\
"
static int
test_case_7_2(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				break;
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((even = wait_event(0))) {
			case NO_MSG:
			case PROVING_NORMAL:
				if (count < iutconf.sdt.Tin) {
					send(INVALID_STATUS);
					count++;
				} else {
					send(COUNT);
					send(PROVING_NORMAL);
					beg_time = milliseconds(t4);
					start_tt(iutconf.sl.t4n * 10 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case IUT_IN_SERVICE:
				expand = 1;
				break;
			case FISU:
				expand = 1;
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_7_3 "\
AERM check\n\
Error rate above the normal threshold\
"
static int
test_case_7_3(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case ALIGNMENT:
				send(ALIGNMENT);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				break;
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				start_tt(iutconf.sl.t4n * 10 / 2);
				tries = 1;
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((even = wait_event(0))) {
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				send(COUNT);
				if (tries == iutconf.sl.M)
					return SUCCESS;
				return FAILURE;
			case NO_MSG:
			case PROVING_NORMAL:
				if (count <= iutconf.sdt.Tin) {
					send(INVALID_STATUS);
					count++;
				} else {
					send(COUNT);
					count = 0;
					send(PROVING_NORMAL);
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
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_NORMAL);
				expand = 0;
				break;
			case OUT_OF_SERVICE:
				expand = 1;
				break;
			case OUT_OF_SERVICE:
				expand = 1;
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_7_4 "\
AERM check\n\
Error rate at the emergency threshold\
"
static int
test_case_7_4(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(OUT_OF_SERVICE);
				break;
			case ALIGNMENT:
				send(ALIGNMENT);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(ALIGNMENT);
				break;
			case PROVING_NORMAL:
				send(PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 10 / 2);
				expand = 0;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_EMERG);
				expand = 0;
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = wait_event(0))) {
			case NO_MSG:
			case PROVING_NORMAL:
				if (count < iutconf.sdt.Tie) {
					send(INVALID_STATUS);
					count++;
				} else {
					send(COUNT);
					send(PROVING_EMERG);
					beg_time = milliseconds(t4e);
					start_tt(iutconf.sl.t4e * 10 + 200);
					expand = 0;
					state = 5;
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_EMERG);
				expand = 0;
				break;
			case IUT_IN_SERVICE:
				expand = 1;
				break;
			case FISU:
				expand = 1;
				return SUCCESS;
			default:
				expand = 1;
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
			send(DATA);
			start_tt(5000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case ACK:
				break;
			case IUT_DATA:
				signal(SEND_MSU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ACK:
				break;
			case DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_8_2 "\
Transmission and reception control (Basic)\n\
Negative acknowledgement of an MSU\
"
static int
test_case_8_2(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				if (check_snibs(0xff, 0x80))
					return FAILURE;
				state = 2;
			case FISU:
				pt_fsn = pt_bsn = 0x7f;
				pt_fib = pt_bib = 0x80;
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (check_snibs(0xff, 0x81))
					return FAILURE;
				pt_fsn = pt_bsn = 0x7f;
				pt_fib = 0x80;
				pt_bib = 0x00;
				send(FISU);
				state = 3;
				break;
			case FISU:
				pt_fsn = pt_bsn = 0x7f;
				pt_fib = pt_bib = 0x80;
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case DATA:
				if (check_snibs(0xff, 0x00))
					return FAILURE;
				state = 4;
			case FISU:
				pt_fsn = pt_bsn = 0x7f;
				pt_fib = 0x80;
				pt_bib = 0x00;
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case DATA:
				if (check_snibs(0xff, 0x01))
					return FAILURE;
				return SUCCESS;
			case FISU:
				pt_fsn = pt_bsn = 0x7f;
				pt_fib = 0x80;
				pt_bib = 0x00;
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
				signal(SEND_MSU);
			for (i = 0; i < n; i++)
				signal(SEND_MSU);
			count = 0;
			show_fisus = 0;
			show_timeout = 1;
			start_tt(iutconf.sl.t7 * 10 / 2 - 100);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(SHORT_WAIT))) {
			case DATA:
				if (++count == n) {
					nacks = n;
					signal(ETC);
					signal(COUNT);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
					pt_bsn += nacks;
#endif
					send(ACK);
					nacks = 1;
					count = 0;
					oldret = 0;
					cntret = 0;
					start_tt(iutconf.sl.t7 * 10 / 2 + 200);
					state = 2;
				}
				break;
			case TIMEOUT:
				expand = 1;
				signal(ETC);
				signal(COUNT);
				nacks = count;
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn += nacks;
#endif
				send(ACK);
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
				signal(ETC);
				signal(COUNT);
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (++count == n) {
					expand = 1;
					nacks = n;
					signal(ETC);
					signal(COUNT);
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
					pt_bsn += nacks;
#endif
					send(ACK);
					nacks = 1;
					return SUCCESS;
				}
				expand = 0;
				break;
			default:
				expand = 1;
				signal(ETC);
				signal(COUNT);
				return FAILURE;
			}
			break;
		default:
			expand = 1;
			return SCRIPTERROR;
		}
	}
}

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
			send(ACK);
			signal(SEND_MSU);
			start_tt(iutconf.sl.t7 * 10 + 200);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(ACK);
				send(DATA);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_DATA:
				state = 3;
				break;
			case ACK:
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_8_5 "\
Transmission and reception control (Basic)\n\
Duplicated FSN\
"
static int
test_case_8_5(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return NOTAPPLICABLE;	/* can't do this */
#else
	int inds = 0;
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			send(DATA);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_DATA:
				inds++;
				break;
			case ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0xffffff) {
					if (iut_bsn != 0)
						return FAILURE;
					pt_fsn--;
					pt_fsn &= 0xffffff;
					send(DATA);
					state = 2;
					break;
				}
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 0) {
					if (iut_bsn != 1)
						return FAILURE;
					pt_fsn--;
					pt_fsn &= 0xffffff
					send(DATA);
					state = 2;
					break;
				}
#endif
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(SHORT_WAIT))) {
			case IUT_DATA:
				if (inds++)
					return FAILURE;
				break;
			default:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0)
					return FAILURE;
				if (inds) {
					send(DATA);
					state = 3;
					break;
				}
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 1)
					return FAILURE;
				if (inds) {
					send(DATA);
					state = 3;
					break;
				}
#endif
				break;
			}
			break;
		case 3:
			switch ((event = wait_event(NORMAL_WAIT))) {
			case IUT_DATA:
				if (++inds > 2)
					return FAILURE;
				break;
			default:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0) {
					if (iut_bsn != 1)
						return FAILURE;
					if (inds == 2)
						return SUCCESS;
				}
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 1) {
					if (iut_bsn != 2)
						return FAILURE;
					if (inds == 2)
						return SUCCESS;
				}
#endif
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_8_6 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Single MSU\
"
static int
test_case_8_6(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			pt_fib = 0x00;
			send(DATA);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case IUT_DATA:
				break;
			case FISU:
			case NO_MSG:
				pt_fib = 0x80;
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x7f;
					oldmsg = 0;
					cntmsg = 0;
					send(DATA);
					state = 4;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_DATA:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0x7f || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x00, 0xff))
						return FAILURE;
					return SUCCESS;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_8_7 "\
Transmission and reception control (Basic)\n\
Erroneous retransmission - Multiple FISUs\
"
static int
test_case_8_7(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(FISU);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				pt_fib = 0x00;
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				pt_fib = 0x80;
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				pt_fib = 0x00;
				send(FISU);
				state = 4;
				start_tt(100);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case FISU:
				break;
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_8_8 "\
Transmission and reception control (Basic)\n\
Single FISU with corrupt FIB\
"
static int
test_case_8_8(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			send(FISU);
			state = 1;
			break;
		case 1:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				pt_fib = 0x00;
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(0))) {
			case FISU:
			case NO_MSG:
				pt_fib = 0x80;
				send(FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
			send(LPO);
			send(PROCESSOR_OUTAGE);
			state = 6;
			break;
		case 6:
			switch ((event = get_event())) {
			case IUT_RPO:
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			send(DATA);
			state = 3;
			break;
		case 3:
			start_tt(1000);
			state = 4;
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_RPR:
			case ACK:
				break;
			case IUT_DATA:
				send(DATA);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case IN_SERVICE:
				break;
			case IUT_RPR:
				break;
			case ACK:
				break;
			case IUT_DATA:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = iut_fsn;
				signal(LPO);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROCESSOR_OUTAGE:
				pt_bsn = iut_fsn;
				send(ACK);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			signal(LPR);
			state = 4;
			break;
		case 4:
			switch ((event = get_event())) {
			case PROCESSOR_ENDED:
				send(SEQUENCE_SYNC);
				signal(SEND_MSU);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = iut_fsn;
				send(ACK);
				start_tt(1000);
				state = 6;
				break;
			default:
				return FAILURE;
			}
			break;
		case 6:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_8_10 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - single MSU\
"
static int
test_case_8_10(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return NOTAPPLICABLE;	/* can't do this */
#else
	uint inds = 0;
	for (;;) {
		switch (state) {
		case 0:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
			pt_bsn = 0xffffff;
#else
			pt_bsn = 0x3fff;
#endif
			send(DATA);
			state = 3;
		case 3:
			switch ((event = get_event())) {
			case IUT_DATA:
				inds++;
				break;
			case ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0xffffff)
					if (iut_bsn != 0)
						return FAILURE;
				pt_bsn = 0xffffff;
				send(DATA);
				state = 4;
				break;
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 0)
					if (iut_bsn != 1)
						return FAILURE;
				pt_bsn = 0;
				send(DATA);
				state = 4;
				break;
#endif
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_DATA:
				if (inds++ > 1)
					return FAILURE;
				if (inds == 2)
					return SUCCESS;
				break;
			case ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0)
					if (iut_bsn != 1)
						return FAILURE;
				break;
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 1)
					if (iut_bsn != 2)
						return FAILURE;
				break;
#endif
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_8_11 "\
Transmission and reception control (Basic)\n\
Abnormal BSN - two consecutive FISUs\
"
static int
test_case_8_11(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return NOTAPPLICABLE;	/* can't do this */
#else
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
		case 1:
			switch ((event = wait_event(0))) {
			case ACK:
			case NO_MSG:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				pt_bsn = 0x7fffff;
#else
				pt_bsn = 0x3fff;
#endif
				send(ACK);
				oldmsg = 0;
				cntmsg = 0;
				send(ACK);
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				pt_bsn = 0;
#else
				pt_bsn = 0;
#endif
				send(ACK);
				state = 2;
				start_tt(1000);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case ACK:
				break;
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			case NO_MSG:
				signal(STOP);
				start_tt(1000);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#define desc_case_8_14 "\
Transmission and reception control (Basic)\n\
Abnormal FIBR\
"
static int
test_case_8_14(void)
{
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
	return NOTAPPLICABLE;	/* can't do this */
#else
	int nind = 0;
	for (;;) {
		switch (state) {
		case 0:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
			pt_bsn = 0xffffff;
#else
			pt_bsn = 0x3fff;
#endif
			send(DATA);
			state = 3;
		case 3:
			switch ((event = get_event())) {
			case IUT_DATA:
				if (nind < 1)
					nind++;
				else
					return FAILURE;
				break;
			case ACK:
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
				if (iut_fsn != 0xffffff)
					return FAILURE;
				if (iut_bsn != 0xffffff)
					if (iut_bsn != 0)
						return FAILURE;
				pt_fsn = 1;
				send(DATA);
				state = 4;
				break;
#else
				if (iut_fsn != 0)
					return FAILURE;
				if (iut_bsn != 0)
					if (iut_bsn != 1)
						return FAILURE;
				pt_fsn = 2;
				send(DATA);
				state = 4;
				break;
#endif
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case IUT_DATA:
				if (nind < 1)
					nind++;
				else
					return FAILURE;
				break;
#if defined M2PA_VERSION_DRAFT11
			case TIMEOUT:
				return SUCCESS;
#else
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return SUCCESS;
#endif
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_1 "\
Transmission and reception control (PCR)\n\
MSU transmission and reception\
"
static int
test_case_9_1(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case DATA:
				if (count < 4) {
					cntret = -1;
					pt_bsn = 0x7f;
					send(FISU);
					count++;
					break;
				}
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				send(FISU);
				break;
			case FISU:
				if (check_snibs(0xff, 0x80))
					return FAILURE;
				send(DATA);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_DATA:
				break;
			case FISU:
				if ((iut_bsn | iut_bib) != 0xff || (iut_fsn | iut_fib) != 0x80)
					return check_snibs(0x80, 0x80);
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_2 "\
Transmission and reception control (PCR)\n\
Priority control\
"
static int
test_case_9_2(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	int fsn_lo = 0, fsn_hi = 0, fsn_ex = 0;
	for (;;) {
		switch (state) {
		case 0:
			fsn_lo = 0;
			signal(SEND_MSU);
			signal(SEND_MSU);
			fsn_hi = 1;
			fsn_ex = fsn_lo;
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case DATA:
				if (iut_fsn < fsn_hi) {
					if (iut_fsn != fsn_ex)
						return FAILURE;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					pt_bsn = 0x7f;
					send(FISU);
					break;
				}
				if (iut_fsn != fsn_hi)
					return FAILURE;
				signal(ETC);
				signal(TRIES);
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				pt_bsn = 0x7f;
				send(FISU);
				signal(SEND_MSU);
				fsn_hi++;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn < fsn_hi) {
					if (iut_fsn != fsn_ex)
						return FAILURE;
					if (++fsn_ex == fsn_hi) {
						fsn_ex = fsn_lo;
						tries++;
					}
					oldisb = (oldisb & 0xff80) + fsn_ex;
					pt_bsn = 0x7f;
					send(FISU);
					break;
				}
				if (iut_fsn != fsn_hi)
					return FAILURE;
				signal(ETC);
				signal(TRIES);
				tries = 0;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case FISU:
				if (iut_fsn == 2)
					return SUCCESS;
				return FAILURE;
			case DATA:
				pt_bsn = fsn_lo;
				send(FISU);
				if (iut_fsn != fsn_ex) {
					if (iut_fsn == fsn_ex + 1) {
						if (fsn_lo < fsn_hi) {
							fsn_lo++;
							fsn_ex++;
							signal(ETC);
							signal(TRIES);
							tries = 0;
						}
					} else
						fsn_ex--;
				} else if (++tries > 100)
					return FAILURE;
				if (++fsn_ex > fsn_hi)
					fsn_ex = fsn_lo;
				oldisb = (oldisb & 0xff80) + fsn_ex;
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_3 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N1\
"
static int
test_case_9_3(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;
	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3)
		return INCONCLUSIVE;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n)
					return FAILURE;
				if (iut_fsn == n - 1)
					count++;
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 0 && count) {
					pt_bsn = 0x0;
					send(FISU);
					count = 1;
					state = 3;
					break;
				}
				pt_bsn = 0x7f;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n) {
					send(FISU);
					return SUCCESS;
				}
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				pt_bsn = 0;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_4 "\
Transmission and reception control (PCR)\n\
Forced retransmission with the value N2\
"
static int
test_case_9_4(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1 - 1;
	msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
	n = iutconf.sl.N2 / (msu_len + h) + 1;
	if (msu_len > iutconf.sdl.m)
		return INCONCLUSIVE;
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n)
					return FAILURE;
				if (iut_fsn == n - 1)
					count++;
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 0 && count) {
					pt_bsn = 0x0;
					send(FISU);
					count = 1;
					state = 3;
					break;
				}
				pt_bsn = 0x7f;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n) {
					send(FISU);
					return SUCCESS;
				}
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				pt_bsn = 0;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_5 "\
Transmission and reception control (PCR)\n\
Forced retransmission cancel\
"
static int
test_case_9_5(void)
{
	return NOTAPPLICABLE;	/* can't do this */
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
			return INCONCLUSIVE;
	}
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n)
					return FAILURE;
				if (iut_fsn == n - 1)
					count++;
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 3 && count) {
					pt_bsn = n - 1;
					send(FISU);
					count = 1;
					state = 3;
					break;
				}
				pt_bsn = 0x7f;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n - 3)
					return FAILURE;
				if (iut_fsn == n) {
					send(FISU);
					return SUCCESS;
				}
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				pt_bsn = n - 1;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_6 "\
Transmission and reception control (PCR)\n\
Reception of forced retransmission\
"
static int
test_case_9_6(void)
{
	return NOTAPPLICABLE;	/* can't do this */
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
			return INCONCLUSIVE;
	}
	printf("(N1=%ld, N2=%ld, n=%d, l=%d)\n", iutconf.sl.N1, iutconf.sl.N2, n, msu_len);
	fflush(stdout);
	for (;;) {
		switch (state) {
		case 0:
			start_tt(iutconf.sl.t7 * 10 + 1000);
			for (i = 0; i < n + 1; i++)
				signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				if (iut_fsn == n)
					return FAILURE;
				if (iut_fsn == n - 1)
					count++;
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 3 && count == 2) {
					pt_bsn = n - 1;
					send(FISU);
					return SUCCESS;
				}
				pt_bsn = 0x7f;
				send(FISU);
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_7 "\
Transmission and reception control (PCR)\n\
MSU transmission while RPO set\
"
static int
test_case_9_7(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			start_tt(5000);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(LPO);
				send(PROCESSOR_OUTAGE);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case IUT_RPO:
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(PROCESSOR_OUTAGE);
				break;
			case FISU:
				if (!count++)
					if (check_snibs(0xff, 0x80))
						return FAILURE;
				pt_bsn = 0x7f;
				send(PROCESSOR_OUTAGE);
				break;
			case TIMEOUT:
				signal(CLEARB);
				send(LPR);
				pt_bsn = 0x7f;
				send(DATA);
				start_tt(500);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case IUT_DATA:
			case IUT_RPR:
				break;
			case FISU:
				if (iut_bsn != 0) {
					pt_bsn = 0x7f;
					pt_fsn = 0x7f;
					send(DATA);
					break;
				}
				send(FISU);
				return SUCCESS;
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_8 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Single MSU\
"
static int
test_case_9_8(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			pt_bsn = 0x00;
			pt_fsn = 0x7f;
			send(DATA);
			pt_bsn = 0x7f;
			pt_fsn = 0x7f;
			send(DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_DATA:
				break;
			case FISU:
				if (iut_bsn == 0)
					return SUCCESS;
				pt_bsn = 0x7f;
				pt_fsn = 0x7f;
				send(DATA);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_9 "\
Transmission and reception control (PCR)\n\
Abnormal BSN - Two MSUs\
"
static int
test_case_9_9(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			pt_bsn = 0x7e;
			pt_fsn = 0x7f;
			send(DATA);
			pt_bsn = 0x7f;
			pt_fsn = 0x7f;
			send(DATA);
			pt_bsn = 0x7e;
			pt_fsn = 0x7f;
			send(DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_DATA:
				return FAILURE;
			case FISU:
				pt_bsn = 0x7f;
				pt_fsn = 0x7f;
				send(DATA);
				break;
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_10 "\
Transmission and reception control (PCR)\n\
Unexpected FSN\
"
static int
test_case_9_10(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			pt_fsn = 0x7f;
			send(DATA);
			pt_fsn = 0x01;
			send(DATA);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case IUT_DATA:
				if (++count == 2)
					return FAILURE;
				break;
			case FISU:
				if (iut_bsn != 0x7f) {
					if (check_snibs(0x80, 0xff))
						return FAILURE;
					return SUCCESS;
				}
				pt_fsn = 0x7f;
				send(DATA);
				break;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_11 "\
Transmission and reception control (PCR)\n\
Excessive delay of acknowledgement\
"
static int
test_case_9_11(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				beg_time = milliseconds(t7);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case DATA:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_12 "\
Transmission and reception control (PCR)\n\
FISU with FSN expected for MSU\
"
static int
test_case_9_12(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			pt_fsn = 0x00;
			send(FISU);
			pt_fsn = 0x7f;
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return check_snibs(0xff, 0xff);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

#define desc_case_9_13 "\
Transmission and reception control (PCR)\n\
Level 3 Stop command\
"
static int
test_case_9_13(void)
{
	return NOTAPPLICABLE;	/* can't do this */
#if 0
	for (;;) {
		switch (state) {
		case 0:
			signal(STOP);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case OUT_OF_SERVICE:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
#endif
}

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
			signal(CONG_D);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case BUSY:
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = wait_event(NORMAL_WAIT))) {
			case NO_MSG:
				signal(NO_CONG);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case BUSY_ENDED:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				send(BUSY);
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case TIMEOUT:
				start_tt((iutconf.sl.t6 - iutconf.sl.t7) * 10 - 300);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
#if defined(M2PA_VERSION_DRAFT6)||defined(M2PA_VERSION_DRAFT6_1)||defined(M2PA_VERSION_DRAFT6_9)||defined(M2PA_VERSION_DRAFT10)||defined(M2PA_VERSION_DRAFT11)
				pt_bsn = iut_fsn;
#endif
				send(BUSY_ENDED);
				send(ACK);
				state = 4;
				start_tt(1000);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				send(BUSY);
				start_tt(iutconf.sl.t6 * 10 - 300);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case TIMEOUT:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds(t7);
				send(BUSY_ENDED);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
				state = 4;
				start_tt(1000);
				break;
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return FAILURE;
			}
		case 4:
			switch ((event = get_event())) {
			case TIMEOUT:
				return FAILURE;
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				send(BUSY);
				start_tt(iutconf.sl.t6 * 10 - 300);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case TIMEOUT:
				start_tt(iutconf.sl.t7 * 10 - 300);
				beg_time = milliseconds(t7);
				send(BUSY_ENDED);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
			case TIMEOUT:
				pt_bsn = iut_fsn;
				send(ACK);
				state = 4;
				start_tt(1000);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch ((event = get_event())) {
			case TIMEOUT:
				return SUCCESS;
			default:
				return FAILURE;
			}
		default:
			return SCRIPTERROR;
		}
	}
}

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
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch ((event = get_event())) {
			case DATA:
				send(BUSY);
				start_tt(timer[t6].hi * 10 + 200);
				beg_time = milliseconds(t6);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				break;
			case IUT_OUT_OF_SERVICE:
				return check_time("T6  ", beg_time, timer[t6].lo, timer[t6].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
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
	if ((mgm_fd = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("  ****ERROR: open failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                :ioctl |                                            \n");
		FFLUSH(stdout);
	}
	if (ioctl(mgm_fd, I_SRDOPT, RMSGD) < 0) {
		printf("  ****ERROR: ioctl failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if ((ret = control(OPTMGMT_REQ)) != SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != OK_ACK)
		return FAILURE;
	if ((ret = control(BIND_REQ)) != SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != BIND_ACK)
		return FAILURE;
	return SUCCESS;
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
	if (close(mgm_fd) < 0) {
		printf("  ****ERROR: close failed\n");
		printf("             %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
	if ((pt_fd = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("****ERROR: open failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                 :ioctl                                             \n");
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_SRDOPT, RMSGD) < 0) {
		printf("****ERROR: ioctl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if ((ret = send(OPTMGMT_REQ)) != SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != OK_ACK)
		return FAILURE;
	if ((ret = send(BIND_REQ)) != SUCCESS)
		return ret;
	if ((event = wait_event(SHORT_WAIT)) != BIND_ACK)
		return FAILURE;
	return SUCCESS;
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
	if (close(pt_fd) < 0) {
		printf("****ERROR: close failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
	if ((iut_fd = open("/dev/sctp_n", O_NONBLOCK | O_RDWR)) < 0) {
		printf("                                   ****ERROR: open failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :ioctl                 \n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_SRDOPT, RMSGD) < 0) {
		printf("                                   ****ERROR: ioctl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if ((ret = signal(OPTMGMT_REQ)) != SUCCESS)
		return FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != OK_ACK)
		return FAILURE;
	if ((ret = signal(BIND_REQ)) != SUCCESS)
		return FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != BIND_ACK)
		return FAILURE;
	return SUCCESS;
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
	if ((ret = signal(CONN_REQ)) != SUCCESS)
		return ret;
	if ((event = wait_event(LONG_WAIT)) != CONN_IND)
		return FAILURE;
	if ((ret = control(CONN_RES)) != SUCCESS)
		return ret;
	ret = wait_event(LONG_WAIT);
	if (ret != OK_ACK && ret != CONN_CON)
		return FAILURE;
	ret = wait_event(LONG_WAIT);
	if (ret != OK_ACK && ret != CONN_CON)
		return FAILURE;
	return SUCCESS;
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
	if (ioctl(iut_fd, I_PUSH, "m2pa-sl") < 0) {
		printf("                                   ****ERROR: push m2pa-sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sdl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sdl            \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdl);
	ctl.ic_dp = (char *) &iutconf.sdl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sdl failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
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
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sdt failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sdt            \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdt);
	ctl.ic_dp = (char *) &iutconf.sdt;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sdt failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: options sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf("                                             :config sl             \n");
		FFLUSH(stdout);
	}
	ctl.ic_cmd = SL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sl);
	ctl.ic_dp = (char *) &iutconf.sl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
		printf("                                   ****ERROR: config sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}
static int
iut_enable(void)
{
	int ret;
	if ((ret = signal(ENABLE_REQ)) != SUCCESS)
		return FAILURE;
	if ((event = wait_event(SHORT_WAIT)) != ENABLE_CON)
		return FAILURE;
	return SUCCESS;
}
static int
iut_disable(void)
{
	int ret;
	if ((ret = signal(DISABLE_REQ)) != SUCCESS)
		return FAILURE;
	if (wait_event(SHORT_WAIT) != DISABLE_CON)
		return FAILURE;
	return SUCCESS;
}
static int
iut_pop(void)
{
	if (verbose > 1) {
		printf("                                             :pop                   \n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_POP, 0) < 0) {
		printf("                                   ****ERROR: pop failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}
static int
iut_disconnect(void)
{
	int ret;
	if ((ret = signal(DISCON_REQ)) != SUCCESS)
		return ret;
	if (wait_event(SHORT_WAIT) != OK_ACK)
		return FAILURE;
	if (wait_event(LONG_WAIT) != DISCON_IND)
		return FAILURE;
	return SUCCESS;
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
	if (close(iut_fd) < 0) {
		printf("                                   ****ERROR: close failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

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
	signal(STOP);
	stop_tt();
	show_msus = 0;
	show_fisus = 1;
	// while ( wait_event(0) != NO_MSG );
	iut_disable();
	ioctl(iut_fd, I_FLUSH, FLUSHRW);
	iut_pop();
	iut_disconnect();
	iut_close();
	ioctl(pt_fd, I_FLUSH, FLUSHRW);
	pt_close();
	ioctl(mgm_fd, I_FLUSH, FLUSHRW);
	mgm_close();
	return SUCCESS;
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
	pt_bib = pt_fib = iut_bib = iut_fib = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != SUCCESS)
		return INCONCLUSIVE;
	if (pt_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_connect() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_m2pa_push() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_enable() != SUCCESS)
		return INCONCLUSIVE;
	start_tt(MAXIMUM_WAIT);
	return SUCCESS;
}
static int
link_out_of_service(void)
{
	iut_options = 0;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	pt_bib = pt_fib = iut_bib = iut_fib = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != SUCCESS)
		return INCONCLUSIVE;
	if (pt_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_connect() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_m2pa_push() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_enable() != SUCCESS)
		return INCONCLUSIVE;
	if (signal(POWER_ON) != SUCCESS)
		return INCONCLUSIVE;
	start_tt(MAXIMUM_WAIT);
	return SUCCESS;
}
static int
link_out_of_service_np(void)
{
	iut_options = SS7_POPT_NOPR;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	pt_bib = pt_fib = iut_bib = iut_fib = 0x0;
#if defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 || defined M2PA_VERSION_DRAFT11
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0xffffff;
#else				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x0;
#endif				/* defined M2PA_VERSION_DRAFT9 || defined M2PA_VERSION_DRAFT10 ||
				   defined M2PA_VERSION_DRAFT11 */
	if (mgm_open() != SUCCESS)
		return INCONCLUSIVE;
	if (pt_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_open() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_connect() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_m2pa_push() != SUCCESS)
		return INCONCLUSIVE;
	if (iut_enable() != SUCCESS)
		return INCONCLUSIVE;
	if (signal(POWER_ON) != SUCCESS)
		return INCONCLUSIVE;
	start_tt(MAXIMUM_WAIT);
	return SUCCESS;
}
static int
link_in_service(void)
{
	if (link_out_of_service() != SUCCESS)
		return INCONCLUSIVE;
	for (;;) {
		switch (state) {
		case 0:
			switch ((event = get_event())) {
			case OUT_OF_SERVICE:
				send(POWER_ON);
				send(OUT_OF_SERVICE);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch ((event = get_event())) {
			case ALIGNMENT:
				send(START);
				send(ALIGNMENT);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch ((event = get_event())) {
			case PROVING_NORMAL:
				send(PROVING_EMERG);
				start_tt(iutconf.sl.t4e * 10 + 200);
				expand = 0;
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch ((event = get_event())) {
#if defined(M2PA_VERSION_DRAFT3)||defined(M2PA_VERSION_DRAFT3_1)
			case PROVING:
				send(PROVING);
				expand = 0;
				break;
#else
			case PROVING_NORMAL:
				send(PROVING_EMERG);
				expand = 0;
				break;
#endif
			case IN_SERVICE:
				expand = 1;
				send(IN_SERVICE);
				break;
			case IUT_IN_SERVICE:
				expand = 1;
				start_tt(MAXIMUM_WAIT);
				return SUCCESS;
			default:
				expand = 1;
				return INCONCLUSIVE;
			}
			break;
		default:
			return INCONCLUSIVE;
		}
	}
	return INCONCLUSIVE;
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
 *  =========================================================================
 *
 *  TEST CASE Lists
 *
 *  =========================================================================
 */
struct test_case {
	const char *numb;		/* test case number */
	const char *name;		/* test case name */
	int (*testcase) (void);		/* test case proper */
	int (*preamble) (void);		/* test case preamble */
	int (*postamble) (void);	/* test case postamble */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} test_suite[] = {
	{
	"1.1(a)", desc_case_1_1a, test_case_1_1a, link_power_off, iut_power_off, 0, 0}, {
	"1.1(b)", desc_case_1_1b, test_case_1_1b, link_out_of_service, iut_power_off, 0, 0}, {
	"1.2", desc_case_1_2, test_case_1_2, link_out_of_service, iut_power_off, 0, 0}, {
	"1.3", desc_case_1_3, test_case_1_3, link_out_of_service, iut_power_off, 0, 0}, {
	"1.4", desc_case_1_4, test_case_1_4, link_out_of_service, iut_power_off, 0, 0}, {
	"1.5(a)", desc_case_1_5a_p, test_case_1_5a_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.5(b)", desc_case_1_5b_p, test_case_1_5b_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.5(a)np", desc_case_1_5a_np, test_case_1_5a_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.5(b)np", desc_case_1_5b_np, test_case_1_5b_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.6", desc_case_1_6_p, test_case_1_6_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.6np", desc_case_1_6_np, test_case_1_6_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.7", desc_case_1_7, test_case_1_7, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.8(a)", desc_case_1_8a_p, test_case_1_8a_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.8(b)", desc_case_1_8b_p, test_case_1_8b_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.8(a)np", desc_case_1_8a_np, test_case_1_8a_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.8(b)np", desc_case_1_8b_np, test_case_1_8b_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.9(a)", desc_case_1_9a_p, test_case_1_9a_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.9(b)", desc_case_1_9b_p, test_case_1_9b_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.9(a)np", desc_case_1_9a_np, test_case_1_9a_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.9(b)np", desc_case_1_9b_np, test_case_1_9b_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.10", desc_case_1_10_p, test_case_1_10_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.10np", desc_case_1_10_np, test_case_1_10_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.11", desc_case_1_11_p, test_case_1_11_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.11np", desc_case_1_11_np, test_case_1_11_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.12(a)", desc_case_1_12a_p, test_case_1_12a_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.12(b)", desc_case_1_12b_p, test_case_1_12b_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.12(a)np", desc_case_1_12a_np, test_case_1_12a_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.12(b)np", desc_case_1_12b_np, test_case_1_12b_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.13", desc_case_1_13_p, test_case_1_13_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.13np", desc_case_1_13_np, test_case_1_13_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.14", desc_case_1_14, test_case_1_14, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.15", desc_case_1_15_p, test_case_1_15_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.15np", desc_case_1_15_np, test_case_1_15_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.16", desc_case_1_16_p, test_case_1_16_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.16np", desc_case_1_16_np, test_case_1_16_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.17", desc_case_1_17, test_case_1_17, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.18", desc_case_1_18, test_case_1_18, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.19", desc_case_1_19, test_case_1_19, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.20", desc_case_1_20, test_case_1_20, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.21", desc_case_1_21, test_case_1_21, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.22", desc_case_1_22, test_case_1_22, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.23", desc_case_1_23, test_case_1_23, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.24", desc_case_1_24, test_case_1_24, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.25", desc_case_1_25, test_case_1_25, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.26", desc_case_1_26, test_case_1_26, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.27", desc_case_1_27_p, test_case_1_27_p, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.27np", desc_case_1_27_np, test_case_1_27_np, link_out_of_service_np, iut_power_off, 0, 0,}, {
	"1.28", desc_case_1_28, test_case_1_28, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.29(a)", desc_case_1_29a, test_case_1_29a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.29(b)", desc_case_1_29b, test_case_1_29b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.30(a)", desc_case_1_30a, test_case_1_30a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.30(b)", desc_case_1_30b, test_case_1_30b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.31(a)", desc_case_1_31a, test_case_1_31a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.31(b)", desc_case_1_31b, test_case_1_31b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"1.32(a)", desc_case_1_32a, test_case_1_32a, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.32(b)", desc_case_1_32b, test_case_1_32b, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.33", desc_case_1_33, test_case_1_33, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.34", desc_case_1_34, test_case_1_34, link_out_of_service, iut_power_off, 0, 0,}, {
	"1.35", desc_case_1_35, test_case_1_35, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.1", desc_case_2_1, test_case_2_1, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.2", desc_case_2_2, test_case_2_2, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.3", desc_case_2_3, test_case_2_3, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.4", desc_case_2_4, test_case_2_4, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.5", desc_case_2_5, test_case_2_5, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.6", desc_case_2_6, test_case_2_6, link_out_of_service, iut_power_off, 0, 0,}, {
	"2.7", desc_case_2_7, test_case_2_7, link_in_service_basic, iut_power_off, 0, 0,}, {
	"2.8", desc_case_2_8, test_case_2_8, link_in_service_basic, iut_power_off, 0, 0,}, {
	"3.1", desc_case_3_1, test_case_3_1, link_out_of_service, iut_power_off, 0, 0,}, {
	"3.2", desc_case_3_2, test_case_3_2, link_in_service_basic, iut_power_off, 0, 0,}, {
	"3.3", desc_case_3_3, test_case_3_3, link_out_of_service, iut_power_off, 0, 0,}, {
	"3.4", desc_case_3_4, test_case_3_4, link_out_of_service, iut_power_off, 0, 0,}, {
	"3.5", desc_case_3_5, test_case_3_5, link_in_service_basic, iut_power_off, 0, 0,}, {
	"3.6", desc_case_3_6, test_case_3_6, link_in_service_basic, iut_power_off, 0, 0,}, {
	"3.7", desc_case_3_7, test_case_3_7, link_in_service_basic, iut_power_off, 0, 0,}, {
	"3.8", desc_case_3_8, test_case_3_8, link_in_service_basic, iut_power_off, 0, 0,}, {
	"4.1(a)", desc_case_4_1a, test_case_4_1a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"4.1(b)", desc_case_4_1b, test_case_4_1b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"4.2", desc_case_4_2, test_case_4_2, link_in_service_basic, iut_power_off, 0, 0,}, {
	"4.3", desc_case_4_3, test_case_4_3, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.1", desc_case_5_1, test_case_5_1, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.2", desc_case_5_2, test_case_5_2, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.3", desc_case_5_3, test_case_5_3, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.4(a)", desc_case_5_4a, test_case_5_4a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.4(b)", desc_case_5_4b, test_case_5_4b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.5(a)", desc_case_5_5a, test_case_5_5a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"5.5(b)", desc_case_5_5b, test_case_5_5b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"6.1", desc_case_6_1, test_case_6_1, link_in_service_basic, iut_power_off, 0, 0,}, {
	"6.2", desc_case_6_2, test_case_6_2, link_in_service_basic, iut_power_off, 0, 0,}, {
	"6.3", desc_case_6_3, test_case_6_3, link_in_service_basic, iut_power_off, 0, 0,}, {
	"6.4", desc_case_6_4, test_case_6_4, link_in_service_basic, iut_power_off, 0, 0,}, {
	"7.1", desc_case_7_1, test_case_7_1, link_out_of_service, iut_power_off, 0, 0,}, {
	"7.2", desc_case_7_2, test_case_7_2, link_out_of_service, iut_power_off, 0, 0,}, {
	"7.3", desc_case_7_3, test_case_7_3, link_out_of_service, iut_power_off, 0, 0,}, {
	"7.4", desc_case_7_4, test_case_7_4, link_out_of_service, iut_power_off, 0, 0,}, {
	"8.1", desc_case_8_1, test_case_8_1, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.2", desc_case_8_2, test_case_8_2, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.3", desc_case_8_3, test_case_8_3, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.4", desc_case_8_4, test_case_8_4, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.5", desc_case_8_5, test_case_8_5, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.6", desc_case_8_6, test_case_8_6, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.7", desc_case_8_7, test_case_8_7, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.8", desc_case_8_8, test_case_8_8, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.9(a)", desc_case_8_9a, test_case_8_9a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.9(b)", desc_case_8_9b, test_case_8_9b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.10", desc_case_8_10, test_case_8_10, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.11", desc_case_8_11, test_case_8_11, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.12(a)", desc_case_8_12a, test_case_8_12a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.12(b)", desc_case_8_12b, test_case_8_12b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.13", desc_case_8_13, test_case_8_13, link_in_service_basic, iut_power_off, 0, 0,}, {
	"8.14", desc_case_8_14, test_case_8_14, link_in_service_basic, iut_power_off, 0, 0,}, {
	"9.1", desc_case_9_1, test_case_9_1, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.2", desc_case_9_2, test_case_9_2, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.3", desc_case_9_3, test_case_9_3, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.4", desc_case_9_4, test_case_9_4, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.5", desc_case_9_5, test_case_9_5, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.6", desc_case_9_6, test_case_9_6, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.7", desc_case_9_7, test_case_9_7, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.8", desc_case_9_8, test_case_9_8, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.9", desc_case_9_9, test_case_9_9, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.10", desc_case_9_10, test_case_9_10, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.11", desc_case_9_11, test_case_9_11, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.12", desc_case_9_12, test_case_9_12, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"9.13", desc_case_9_13, test_case_9_13, link_in_service_pcr, iut_power_off, 0, 0,}, {
	"10.1", desc_case_10_1, test_case_10_1, link_in_service_basic, iut_power_off, 0, 0,}, {
	"10.2(a)", desc_case_10_2a, test_case_10_2a, link_in_service_basic, iut_power_off, 0, 0,}, {
	"10.2(b)", desc_case_10_2b, test_case_10_2b, link_in_service_basic, iut_power_off, 0, 0,}, {
	"10.2(c)", desc_case_10_2c, test_case_10_2c, link_in_service_basic, iut_power_off, 0, 0,}, {
	"10.3", desc_case_10_3, test_case_10_3, link_in_service_basic, iut_power_off, 0, 0,}, {
	NULL,}
};

static int
begin_test(void)
{
	// ioctl( pt_fd, I_FLUSH, FLUSHRW); /* flush Protocol Tester (PT) */
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
	return (SUCCESS);
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
	return (SUCCESS);
}
#if 0
static int
end_tests(void)
{
	return (SUCCESS);
}
#endif

int
do_tests(void)
{
	int i;
	int retry = 0;
	int result = INCONCLUSIVE;
	int failures = 0, successes = 0, inconclusive = 0, notapplicable = 0, scripterror = 0;
	expand = 1;
	show_msus = 1;
	show_fisus = 1;
	show_timeout = 1;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\n\nM2PA - OpenSS7 STREAMS M2PA - Conformance Test Program.\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	for (i = 0; i < sizeof(test_suite) / sizeof(struct test_case) && test_suite[i].numb; i++) {
		if (!retry && test_suite[i].result)
			continue;
		if (!test_suite[i].run) {
			test_suite[i].result = INCONCLUSIVE;
			continue;
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\nM2PA Test Case:Q.781/%s:\n%s\n", test_suite[i].numb, test_suite[i].name);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		fprintf(stdout, "\n");
		begin_test();
		fprintf(stdout, "                         =====Preamble=====                         \n");
		if (test_suite[i].preamble && test_suite[i].preamble() != SUCCESS) {
			fprintf(stdout, "                         ?? INCONCLUSIVE ??                         \n");
			result = INCONCLUSIVE;
		} else {
			begin_test_case();
			fprintf(stdout, "                         =======Test=======                         \n");
			result = INCONCLUSIVE;
			if (test_suite[i].testcase)
				switch ((result = test_suite[i].testcase())) {
				case FAILURE:
					fprintf(stdout, "                         XXXXX FAILED XXXXX                         \n");
					break;
				case SUCCESS:
					fprintf(stdout, "                         ***** PASSED *****                         \n");
					break;
				case NOTAPPLICABLE:
					fprintf(stdout, "                         x NOT APPLICABLE x                         \n");
					break;
				default:
					result = SCRIPTERROR;
				case SCRIPTERROR:
					fprintf(stdout, "                         @@@@@ ERROR @@@@@@                         \n");
					break;
				}
		}
		fprintf(stdout, "                         =====Postamble====                         \n");
		if (test_suite[i].postamble && test_suite[i].postamble() != SUCCESS) {
			fprintf(stdout, "                         ?? INCONCLUSIVE ??                         \n");
			if (result == SUCCESS)
				result = INCONCLUSIVE;
		}
		fprintf(stdout, "                         =======Done=======                         \n");
		switch (result) {
		case SUCCESS:
			fprintf(stdout, "*********\n");
			fprintf(stdout, "********* Test Case SUCCESSFUL\n");
			fprintf(stdout, "*********\n\n");
			break;
		case FAILURE:
			fprintf(stdout, "XXXXXXXXX\n");
			fprintf(stdout, "XXXXXXXXX Test Case FAILED(%d) - test case failed in state %d, with event %d.\n", state, state, event);
			fprintf(stdout, "XXXXXXXXX\n\n");
			if (!retry) {
				retry = 1;
				verbose += 4;
				i--;
				continue;
			}
			retry = 0;
			verbose -= 4;
			break;
		case INCONCLUSIVE:
			fprintf(stdout, "?????????\n");
			fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
			fprintf(stdout, "?????????\n\n");
			if (!retry) {
				retry = 1;
				verbose += 4;
				i--;
				continue;
			}
			retry = 0;
			verbose -= 4;
			break;
		case NOTAPPLICABLE:
			fprintf(stdout, "xxxxxxxxx\n");
			fprintf(stdout, "xxxxxxxxx Test Case NOT APPLICABLE - test case does not apply.\n");
			fprintf(stdout, "xxxxxxxxx\n\n");
			break;
		default:
		case SCRIPTERROR:
			fprintf(stdout, "@@@@@@@@@\n");
			fprintf(stdout, "@@@@@@@@@ ERROR(%d) = test case complete in error in state %d with event %d.\n", state, state, event);
			fprintf(stdout, "@@@@@@@@@\n\n");
			if (!retry) {
				retry = 1;
				verbose += 4;
				i--;
				continue;
			}
			retry = 0;
			verbose -= 4;
			break;
		}
		switch ((test_suite[i].result = result)) {
		case SUCCESS:
			successes++;
			break;
		case FAILURE:
			failures++;
			break;
		case INCONCLUSIVE:
			inconclusive++;
			break;
		case NOTAPPLICABLE:
			notapplicable++;
			break;
		default:
		case SCRIPTERROR:
			scripterror++;
			break;
		}
	}
	lockf(fileno(stdout), F_LOCK, 0);
	if (summary && verbose) {
		fprintf(stdout, "\n\n");
		for (i = 0; i < (sizeof(test_suite) / sizeof(struct test_case)) && test_suite[i].numb; i++) {
			if (test_suite[i].run) {
				fprintf(stdout, "Test Case M2PA:%-10s  ", test_suite[i].numb);
				switch (test_suite[i].result) {
				case SUCCESS:
					fprintf(stdout, "SUCCESS\n");
					break;
				case FAILURE:
					fprintf(stdout, "FAILURE\n");
					break;
				case INCONCLUSIVE:
					fprintf(stdout, "INCONCLUSIVE\n");
					break;
				case NOTAPPLICABLE:
					fprintf(stdout, "NOT APPLICABLE\n");
					break;
				default:
				case SCRIPTERROR:
					fprintf(stdout, "ERROR\n");
					break;
				}
			}
		}
		fflush(stdout);
	}
	fprintf(stdout, "Done.\n\n");
	fprintf(stdout, "========= %2d successes     \n", successes);
	fprintf(stdout, "========= %2d failures      \n", failures);
	fprintf(stdout, "========= %2d inconclusive  \n", inconclusive);
	fprintf(stdout, "========= %2d not applicable\n", notapplicable);
	fprintf(stdout, "========= %2d script error  \n", scripterror);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (0);
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
M2PA - OpenSS7 STREAMS M2PA - Conformance Test Suite\n\
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
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
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
	for (t = test_suite; t->numb; t++) {
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
			{NULL, }
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
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", (long)timer_scale);
			break;
		case 's':
			summary = 1;
			break;
		case 'o':
			if (!range) {
				for (t = test_suite; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = test_suite; t->numb; t++)
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
				for (t = test_suite; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = test_suite; t->numb; t++)
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
