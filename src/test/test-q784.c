/*****************************************************************************

 @(#) $RCSfile: test-q784.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:18 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2009-06-21 11:44:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-q784.c,v $
 Revision 1.1.2.1  2009-06-21 11:44:18  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: test-q784.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:18 $";

/*
 *  This is a ferry-clip Q.784 conformance test program for testing the
 *  OpenSS7 ISUP multiplexing driver.
 *
 *  GENERAL OPERATION:
 *
 *  The test program opens a management stream to the ISUP multiplexing
 *  driver, configures the driver for a single signalling point, single
 *  signalling relation, and single trunk group.  It then opens a pipe and
 *  links one end of the pipe underneath the ISUP driver as the MTP Level 3
 *  stream.
 *
 *  The test program opens an ISUP stream and feeds ISUP primitives to and
 *  from that stream.  On the open end of the pipe it feeds MTP Level 3
 *  primitives into the bottom of the ISUP multiplexing driver.  This is the
 *  ferry-clip test arrangement as follows:
 *
 *
 *                                  USER SPACE                              
 *                                 TEST PROGRAM                             
 *
 *             ___________________________________________________          
 *                    \   /  |                  \   /  |                    
 *                     \ /   |                   \ /   |                    
 *                      |   / \                   |    |                    
 *             _________|__/___\_______           |    |                    
 *            |                        |          |    |                    
 *            |                        |          |    |                    
 *            |                        |          |    |                    
 *            |          ISUP          |          |    |                    
 *            |                        |          |    |                    
 *            |                        |          |    |                    
 *            |________________________|          |    |                    
 *                    \   /  |                    |    |                    
 *                     \ /   |                    |    |                    
 *                      |    |                    |    |                    
 *                      |   / \                   |   / \                   
 *             _________|__/___\__________________|__/___\________          
 *            |                                                   |         
 *            |                                                   |         
 *            |                        PIPE                       |         
 *            |                                                   |         
 *            |___________________________________________________|         
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
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/cci.h>
#include <ss7/isupi.h>
#include <ss7/isupi_ioctl.h>

enum {
	__EVENT_TIMEOUT = -5, __EVENT_UNKNOWN = -4, __RESULT_DECODE_ERROR = -3, __RESULT_SCRIPT_ERROR = -2,
	__RESULT_INCONCLUSIVE = -1, __RESULT_SUCCESS = 0, __RESULT_FAILURE = 1,
};



/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

/* Q.784 ISUP Basic Call Test Specification - Conformance Test Program\n\ */

static const char *lpkgname = "SS7 ISDN User Part";

/* static const char *spkgname = "strss7"; */
static const char *lstdname = "Q.763/ANSI T1.113/JQ.763";
static const char *sstdname = "Q.784";
static const char *shortname = "ISUP";
static char devname[256] = "/dev/streams/clone/isup";

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

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
static int last_prio = 0;

#define TEST_PROTOCOL 132

#define CHILD_PTU   0
#define CHILD_IUT   1

static int iut_connects = 1;

struct mtp_addr loc_addr = {
	ni:1,
	pc:0x00112233,
	si:5
};
struct mtp_addr rem_addr = {
	ni:1,
	pc:0x00223344,
	si:6
};

struct isup_addr iut_cpc_addr = {
	scope:ISUP_SCOPE_TG,
	id:1,
	cic:0,
};
struct isup_addr iut_tst_addr = {
	scope:ISUP_SCOPE_SR,
	id:1,
	cic:0,
};
struct isup_addr iut_mgm_addr = {
	scope:ISUP_SCOPE_SR,
	id:1,
	cic:0,
};
struct isup_addr iut_mnt_addr = {
	scope:ISUP_SCOPE_SR,
	id:1,
	cic:0,
};

#undef HZ
#define HZ 1000

static struct isup_opt_conf_ct iut_ct_opt = {
};
static struct isup_opt_conf_cg iut_cg_opt = {
};
static struct isup_opt_conf_tg iut_tg_opt = {
	flags:ISUP_TGF_CONTROLLING_EXCHANGE,	/* configuration flags */
	type:ISUP_TG_TYPE_2W,		/* trunk group type */
	exchange_type:ISUP_XCHG_TYPE_B,	/* exchange type */
	select_type:ISUP_SELECTION_TYPE_DSEQ,	/* circuit selection type */
	t1:20 * HZ,			/* waiting for RLC retry */
	t2:3 * 60 * HZ,			/* waiting for RES */
	t3:2 * 60 * HZ,			/* waiting OVL timeout */
	t5:8 * 60 * HZ,			/* waiting for RLC maintenance */
	t6:20 * HZ,			/* waiting for RES */
	t7:25 * HZ,			/* waiting for ACM/ANM/CON */
	t8:12 * HZ,			/* waiting for COT */
	t9:3 * 60 * HZ,			/* waiting for ANM/CON */
	t10:5 * HZ,			/* waiting more information. Interworking */
	t11:17 * HZ,			/* waiting for ACM, Interworking */
	t12:55 * HZ,			/* waiting for BLA retry */
	t13:8 * 60 * HZ,		/* waiting for BLA maintenance */
	t14:55 * HZ,			/* waiting for UBA retry */
	t15:8 * 60 * HZ,		/* waiting for UBA maintenance */
	t16:55 * HZ,			/* waiting for RLC retry */
	t17:8 * 60 * HZ,		/* waiting for RLC maintenance */
	t24:1 * HZ,			/* waiting for continuity IAM */
	t25:5 * HZ,			/* waiting for continuity CCR retry */
	t26:2 * 60 * HZ,		/* waiting for continuity CCR maintenance */
	t27:4 * 60 * HZ,		/* waiting for COT reset */
	t31:7 * 60 * HZ,		/* call reference guard */
	t32:4 * HZ,			/* waiting to send E2E message */
	t33:13 * HZ,			/* waiting for INF */
	t34:13 * HZ,			/* waiting for SEG */
	t35:17 * HZ,			/* waiting more information. */
	t36:17 * HZ,			/* waiting for COT/REL */
	t37:3 * HZ,			/* waiting echo control device */
	t38:17 * HZ,			/* waiting for RES */
	tacc_r:0,			/* */
	tccr:0,				/* */
	tccr_r:0,			/* */
	tcra:0,				/* */
	tcrm:0,				/* */
	tcvt:0,				/* */
	texm_d:0,			/* */
};
static struct isup_opt_conf_sr iut_sr_opt = {
	t4:8 * 60 * HZ,			/* waiting for UPA/other */
	t18:55 * HZ,			/* waiting CGBA retry */
	t19:8 * 60 * HZ,		/* waiting CGBA maintenance */
	t20:55 * HZ,			/* waiting CGUA retry */
	t21:8 * 60 * HZ,		/* waiting CGUA maintenance */
	t22:55 * HZ,			/* waiting GRA retry */
	t23:8 * 60 * HZ,		/* waiting GRA maintenance */
	t28:10 * HZ,			/* waiting CQR */
	t29:450 * 1000 / HZ,		/* congestion attack timer */
	t30:7 * HZ,			/* congestion decay timer */
	tcgb:0,				/* */
	tgrs:0,				/* */
	thga:0,				/* */
	tscga:0,			/* */
	tscga_d:0,			/* */
};
static struct isup_opt_conf_sp iut_sp_opt = {
};
static struct isup_opt_conf_mtp iut_mtp_opt = {
};
static struct isup_opt_conf_df iut_df_opt = {
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Messages
 *
 *  -------------------------------------------------------------------------
 */

struct numb {
	char num[200];
	size_t len;
	ulong nai;
};

struct var {
	unsigned char buf[100];
	size_t len;
};

struct msg {
	ulong cic;
	struct numb cdpn;
	struct numb cgpn;
	struct numb subn;
	ulong nci;
	ulong fci;
	ulong cpc;
	ulong tmr;
	ulong inri;
	ulong infi;
	ulong coti;
	ulong bci;
	ulong sris;
	ulong cgi;
	ulong cmi;
	ulong faci;
	struct var pam;
	struct var rs;
	struct var csi;
	ulong evnt;
	struct var uui;
	struct var caus;
	ulong cvri;
	ulong cgri;
	ulong ton;
	ulong cri;
	struct var opt;
};

struct msg pmsg = {
	12,
	{"17804901140", 11, 0},
	{"17804901241", 11, 0},
	{"555", 3, 0},
	0,
	(ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8,
	(ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24,
	ISUP_CALL_TYPE_SPEECH,
	0,
	0,
	ISUP_COT_SUCCESS,
	0,
	0,
	0,
	0,
	0,
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0},
	0,
	{{0,}, 0},
	{{0x80, 0x90}, 0},
	0,
	0,
	0,
	0,
	{{0,}, 0}
};
struct msg imsg = {
	12,
	{{0,}, 24, 0},
	{{0,}, 24, 0},
	{{0,}, 24, 0},
	0,
	(ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8,
	(ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24,
	ISUP_CALL_TYPE_SPEECH,
	0,
	0,
	ISUP_COT_SUCCESS,
	0,
	0,
	0,
	0,
	0,
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0},
	0,
	{{0,}, 0},
	{{0x80, 0x90}, 0},
	0,
	0,
	0,
	0,
	{{0,}, 0}
};

struct iaddr {
	struct isup_addr add;
	size_t len;
};

static ulong user_ref = 0;

struct prim {
	ulong bind_flags;
	ulong setup_ind;
	ulong opt_flags;
	ulong call_type;
	ulong call_flags;
	ulong call_ref;
	ulong user_ref;
	ulong token_value;
	ulong result;
	ulong flags;
	ulong event;
	ulong cause;
	ulong reason;
	struct var subn;
	struct iaddr addr;
	struct var opt;
	struct var cdpn;
};

struct prim cprim = {
	CC_TOKEN_REQUEST,
	31,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0}
};

struct prim mprim = {
	CC_TOKEN_REQUEST | CC_MANAGEMENT,
	31,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0},
	{{0,}, 0}
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

enum {
	t1 = 0, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15,
	t16, t17, t18, t19, t20, t21, t22, t23, t24, t25, t26, t27, t28, t29,
	t30, t31, t32, t33, t34, t35, t36, t37, t38, tmax
};

#undef HZ
#define HZ 1000

/* *INDENT-OFF* */
static timer_range_t timer[tmax] = {
	{(15 * HZ),		(60 * HZ),		"T1"	},	/* T1 15-60 seconds */
	{(3 * 60 * HZ),		(3 * 60 * HZ),		"T2"	},	/* T2 3 minutes */
	{(2 * 60 * HZ),		(2 * 60 * HZ),		"T3"	},	/* T3 2 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T4"	},	/* T4 5-15 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T5"	},	/* T5 5-15 minutes */
	{(10 * HZ),		(32 * HZ),		"T6"	},	/* T6 10-32 seconds (specified in Q.118) */
	{(20 * HZ),		(30 * HZ),		"T7"	},	/* T7 20-30 seconds */
	{(10 * HZ),		(15 * HZ),		"T8"	},	/* T8 10-15 seconds */
	{(2 * 60 * HZ),		(4 * 60 * HZ),		"T9"	},	/* T9 2-4 minutes (specified in Q.118) */
	{(4 * HZ),		(6 * HZ),		"T10"	},	/* T10 4-6 seconds */
	{(15 * HZ),		(20 * HZ),		"T11"	},	/* T11 15-20 seconds */
	{(15 * HZ),		(60 * HZ),		"T12"	},	/* T12 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T13"	},	/* T13 5-15 minutes */
	{(15 * HZ),		(60 * HZ),		"T14"	},	/* T14 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T15"	},	/* T15 5-15 minutes */
	{(15 * HZ),		(60 * HZ),		"T16"	},	/* T16 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T17"	},	/* T17 5-15 minutes */
	{(15 * HZ),		(60 * HZ),		"T18"	},	/* T18 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T19"	},	/* T19 5-15 minutes */
	{(15 * HZ),		(60 * HZ),		"T20"	},	/* T20 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T21"	},	/* T21 5-15 minutes */
	{(15 * HZ),		(60 * HZ),		"T22"	},	/* T22 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ),		"T23"	},	/* T23 5-15 minutes */
	{(1 * HZ),		(2 * HZ),		"T24"	},	/* T24 < 2 seconds */
	{(1 * HZ),		(10 * HZ),		"T25"	},	/* T25 1-10 seconds */
	{(1 * 60 * HZ),		(3 * 60 * HZ),		"T26"	},	/* T26 1-3 minutes */
	{(4 * 60 * HZ),		(4 * 60 * HZ),		"T27"	},	/* T27 4 minutes */
	{(10 * HZ),		(10 * HZ),		"T28"	},	/* T28 10 seconds */
	{(300 * HZ / 1000),	(600 * HZ / 1000),	"T29"	},	/* T29 300-600 milliseconds */
	{(5 * HZ),		(10 * HZ),		"T30"	},	/* T30 5-10 seconds */
	{(6 * 60 * HZ),		(7 * 60 * HZ),		"T31"	},	/* T31 > 6 minutes */
	{(3 * HZ),		(5 * HZ),		"T32"	},	/* T32 3-5 seconds */
	{(12 * HZ),		(15 * HZ),		"T33"	},	/* T33 12-15 seconds */
	{(12 * HZ),		(15 * HZ),		"T34"	},	/* T34 12-15 seconds */
	{(15 * HZ),		(20 * HZ),		"T35"	},	/* T35 15-20 seconds */
	{(15 * HZ),		(20 * HZ),		"T36"	},	/* T36 15-20 seconds */
	{(2 * HZ),		(4 * HZ),		"T37"	},	/* T37 2-4 seconds */
	{(15 * HZ),		(20 * HZ),		"T38"	}	/* T38 15-20 seconds */
};
/* *INDENT-ON* */

long test_start = 0;

static int state = 0;
static const char *failure_string = NULL;

#define __stringify_1(x) #x
#define __stringify(x) __stringify_1(x)
#define FAILURE_STRING(string) "[" __stringify(__LINE__) "] " string

/* lockf does not work well on SMP for some reason */
#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

/*
 *  Return the current time in milliseconds.
 */
static long
now(void)
{
	long ret;
	struct timeval now;

	if (gettimeofday(&now, NULL)) {
		printf("***************ERROR: couldn't get time!\n");
		printf("                      %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return (0);
	}
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000L;
	ret += (now.tv_usec + 999L) / 1000L;
	return ret;
}

/*
 *  Return the current time in milliseconds.
 */
static long
milliseconds(char *t)
{
	printf("                    .  .              :                .                    \n");
	printf("                    .  .            %6s             .                    (%d)\n", t, state);
	printf("                    .  .              :                .                    \n");
	fflush(stdout);
	return now();
}
static long
milliseconds_2nd(char *t)
{
	printf("                    .  .              :   :            .                    \n");
	printf("                    .  .              : %6s         .                    (%d)\n", t, state);
	printf("                    .  .              :   :            .                    \n");
	fflush(stdout);
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
	printf("                    |  |(%7.3g <= %7.3g <= %7.3g)| %s             (%d)\n", dlo - tol, itv, dhi + tol, t, state);
	fflush(stdout);
	if (dlo - tol <= itv && itv <= dhi + tol)
		return __RESULT_SUCCESS;
	return __RESULT_FAILURE;
}

static int
time_event(int event)
{
	if (verbose) {
		float t, m;
		struct timeval now;

		gettimeofday(&now, NULL);
		if (!test_start)
			test_start = now.tv_sec;
		t = (now.tv_sec - test_start);
		m = now.tv_usec;
		m = m / 1000000;
		t += m;
		printf("                    |  | %11.6g                   |                    (%d)\n", t, state);
		fflush(stdout);
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
//      act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		goto failure;
	if (sigaction(SIGPOLL, &act, NULL))
		goto failure;
	if (sigaction(SIGURG, &act, NULL))
		goto failure;
	if (sigaction(SIGPIPE, &act, NULL))
		goto failure;
	if (sigaction(SIGHUP, &act, NULL))
		goto failure;
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
      failure:
	return __RESULT_FAILURE;
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
		goto failure;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		goto failure;
	timer_timeout = 0;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
static int
start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;

	return start_tt(sdur);
}

static int
stop_signals(void)
{
	int result = __RESULT_SUCCESS;
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
//	act.sa_restorer = NULL;
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

#define ISUP_MT_IAM	  1UL	/* 0x01 - 0b00000001 - Initial address */
#define ISUP_MT_SAM	  2UL	/* 0x02 - 0b00000010 - Subsequent address */
#define ISUP_MT_INR	  3UL	/* 0x03 - 0b00000011 - Information request */
#define ISUP_MT_INF	  4UL	/* 0x04 - 0b00000100 - Information */
#define ISUP_MT_COT	  5UL	/* 0x05 - 0b00000101 - Continuity */
#define ISUP_MT_ACM	  6UL	/* 0x06 - 0b00000110 - Address complete */
#define ISUP_MT_CON	  7UL	/* 0x07 - 0b00000111 - Connect (not old ANSI) */
#define ISUP_MT_FOT	  8UL	/* 0x08 - 0b00001000 - Forward transfer */
#define ISUP_MT_ANM	  9UL	/* 0x09 - 0b00001001 - Answer */
#define ISUP_MT_REL	 12UL	/* 0x0c - 0b00001100 - Release */
#define ISUP_MT_SUS	 13UL	/* 0x0d - 0b00001101 - Suspend */
#define ISUP_MT_RES	 14UL	/* 0x0e - 0b00001110 - Resume */
#define ISUP_MT_RLC	 16UL	/* 0x10 - 0b00010000 - Release complete */
#define ISUP_MT_CCR	 17UL	/* 0x11 - 0b00010001 - Continuity check request */
#define ISUP_MT_RSC	 18UL	/* 0x12 - 0b00010010 - Reset circuit */
#define ISUP_MT_BLO	 19UL	/* 0x13 - 0b00010011 - Blocking */
#define ISUP_MT_UBL	 20UL	/* 0x14 - 0b00010100 - Unblcoking */
#define ISUP_MT_BLA	 21UL	/* 0x15 - 0b00010101 - Blocking acknowledgement */
#define ISUP_MT_UBA	 22UL	/* 0x16 - 0b00010110 - Unblocking acknowledgement */
#define ISUP_MT_GRS	 23UL	/* 0x17 - 0b00010111 - Circuit group reset */
#define ISUP_MT_CGB	 24UL	/* 0x18 - 0b00011000 - Circuit group blocking */
#define ISUP_MT_CGU	 25UL	/* 0x19 - 0b00011001 - Circuit group unblocking */
#define ISUP_MT_CGBA	 26UL	/* 0x1a - 0b00011010 - Circuit group blocking acknowledgement */
#define ISUP_MT_CGUA	 27UL	/* 0x1b - 0b00011011 - Circuit group unblocking acknowledgement */
#define ISUP_MT_CMR	 28UL	/* 0x1c - 0b00011100 - Call Modification Request (not old ANSI) */
#define ISUP_MT_CMC	 29UL	/* 0x1d - 0b00011101 - Call Modification Completed (not old ANSI) */
#define ISUP_MT_CMRJ	 30UL	/* 0x1e - 0b00011110 - Call Modification Reject (not old ANSI) */
#define ISUP_MT_FAR	 31UL	/* 0x1f - 0b00011111 - Facility request */
#define ISUP_MT_FAA	 32UL	/* 0x20 - 0b00100000 - Facility accepted */
#define ISUP_MT_FRJ	 33UL	/* 0x21 - 0b00100001 - Facility reject */
#define ISUP_MT_FAD	 34UL	/* 0x22 - 0b00100010 - Facility Deactivated (old Bellcore only) */
#define ISUP_MT_FAI	 35UL	/* 0x23 - 0b00100011 - Facility Information (old Bellcore only) */
#define ISUP_MT_LPA	 36UL	/* 0x24 - 0b00100100 - Loop back acknowledgement */
#define ISUP_MT_DRS	 39UL	/* 0x27 - 0b00100111 - Delayed release (not old ANSI) */
#define ISUP_MT_PAM	 40UL	/* 0x28 - 0b00101000 - Pass along */
#define ISUP_MT_GRA	 41UL	/* 0x29 - 0b00101001 - Circuit group reset acknowledgement */
#define ISUP_MT_CQM	 42UL	/* 0x2a - 0b00101010 - Circuit group query */
#define ISUP_MT_CQR	 43UL	/* 0x2b - 0b00101011 - Circuit group query response */
#define ISUP_MT_CPG	 44UL	/* 0x2c - 0b00101100 - Call progress */
#define ISUP_MT_USR	 45UL	/* 0x2d - 0b00101101 - User-to-user information */
#define ISUP_MT_UCIC	 46UL	/* 0x2e - 0b00101110 - Unequipped circuit identification code */
#define ISUP_MT_CFN	 47UL	/* 0x2f - 0b00101111 - Confusion */
#define ISUP_MT_OLM	 48UL	/* 0x30 - 0b00110000 - Overload */
#define ISUP_MT_CRG	 49UL	/* 0x31 - 0b00110001 - Charge information */
#define ISUP_MT_NRM	 50UL	/* 0x32 - 0b00110010 - Network resource management */
#define ISUP_MT_FAC	 51UL	/* 0x33 - 0b00110011 - Facility */
#define ISUP_MT_UPT	 52UL	/* 0x34 - 0b00110100 - User part test */
#define ISUP_MT_UPA	 53UL	/* 0x35 - 0b00110101 - User part available */
#define ISUP_MT_IDR	 54UL	/* 0x36 - 0b00110110 - Identification request */
#define ISUP_MT_IRS	 55UL	/* 0x37 - 0b00110111 - Identification response */
#define ISUP_MT_SGM	 56UL	/* 0x38 - 0b00111000 - Segmentation */
#define ISUP_MT_CRA	233UL	/* 0xe9 - 0b11101001 - Circuit Reservation Ack (old Bellcore/ANSI
				   2000) */
#define ISUP_MT_CRM	234UL	/* 0xea - 0b11101010 - Circuit Reservation (old Bellcore/ANSI 2000) 
				 */
#define ISUP_MT_CVR	235UL	/* 0xeb - 0b11101011 - Circuit Validation Response (old
				   Bellcore/ANSI 2000) */
#define ISUP_MT_CVT	236UL	/* 0xec - 0b11101100 - Circuit Validation Test (old Bellcore/ANSI
				   2000) */
#define ISUP_MT_EXM	237UL	/* 0xed - 0b11101101 - Exit (old Bellcore/ANSI 2000) */
#define ISUP_MT_NON	248UL	/* 0xf8 - 0b11111000 - National Notification (Spain) */
#define ISUP_MT_LLM	252UL	/* 0xfc - 0b11111100 - National Malicious Call (Spain) */
#define ISUP_MT_CAK	253UL	/* 0xfd - 0b11111101 - Charge Acknowledgement (Singapore) */
#define ISUP_MT_TCM	254UL	/* 0xfe - 0b11111110 - Tariff Charge (Singapore) */
#define ISUP_MT_MCP	255UL	/* 0xff - 0b11111111 - Malicious Call Print (Singapore) */

#define ISUP_PT_CGPN	 10UL	/* 0x0a - 0b00001010 - Calling party number */

/* Events and Actions */
enum {
	IAM = 2, SAM, INR, INF, COT, ACM, CON, FOT, ANM, REL, SUS, RES, RLC,
	CCR, RSC, BLO, UBL, BLA, UBA, GRS, CGB, CGU, CGBA, CGUA, CMR, CMC,
	CMRJ, FAR, FAA, FRJ, FAD, FAI, LPA, DRS, PAM, GRA, CQM, CQR, CPG, USR,
	UCIC, CFN, OLM, CRG, NRM, FAC, UPT, UPA, IDR, IRS, SGM, CRA, CRM, CVR,
	CVT, EXM, NON, LLM, CAK, TCM, MCP, PAUSE, RESUME, RESTART_COMPLETE,
	USER_PART_UNKNOWN, USER_PART_UNEQUIPPED, USER_PART_UNAVAILABLE,
	CONGESTION,
	INFO_REQ, OPTMGMT_REQ, BIND_REQ, UNBIND_REQ, ADDR_REQ, SETUP_REQ,
	MORE_INFO_REQ, INFORMATION_REQ, CONT_CHECK_REQ, CONT_TEST_REQ,
	CONT_REPORT_REQ, SETUP_RES, PROCEEDING_REQ, ALERTING_REQ,
	PROGRESS_REQ, IBI_REQ, CONNECT_REQ, SETUP_COMPLETE_REQ, FORWXFER_REQ,
	SUSPEND_REQ, SUSPEND_RES, SUSPEND_REJECT_REQ, RESUME_REQ, RESUME_RES,
	RESUME_REJECT_REQ, REJECT_REQ, RELEASE_REQ, RELEASE_RES,
	RESET_REQ, RESET_RES, BLOCKING_REQ, BLOCKING_RES, UNBLOCKING_REQ,
	UNBLOCKING_RES, QUERY_REQ,
	QUERY_RES, STOP_REQ,
	CPC_OK_ACK, CPC_ERROR_ACK, CPC_INFO_ACK, CPC_BIND_ACK,
	CPC_OPTMGMT_ACK, CPC_ADDR_ACK, CPC_CALL_REATTEMPT_IND, CPC_SETUP_IND,
	CPC_MORE_INFO_IND, CPC_INFORMATION_IND, CPC_CONT_CHECK_IND,
	CPC_CONT_TEST_IND, CPC_CONT_REPORT_IND, CPC_SETUP_CON,
	CPC_PROCEEDING_IND, CPC_ALERTING_IND, CPC_PROGRESS_IND, CPC_IBI_IND,
	CPC_CONNECT_IND, CPC_SETUP_COMPLETE_IND, CPC_FORWXFER_IND,
	CPC_SUSPEND_IND, CPC_SUSPEND_CON, CPC_SUSPEND_REJECT_IND,
	CPC_RESUME_IND, CPC_RESUME_CON, CPC_RESUME_REJECT_IND, CPC_REJECT_IND,
	CPC_CALL_FAILURE_IND, CPC_RELEASE_IND, CPC_RELEASE_CON, CPC_RESET_IND,
	CPC_RESET_CON, CPC_BLOCKING_IND, CPC_BLOCKING_CON, CPC_UNBLOCKING_IND,
	CPC_UNBLOCKING_CON, CPC_QUERY_IND, CPC_QUERY_CON, CPC_STOP_IND,
	CPC_MAINT_IND, CPC_START_RESET_IND,
	MGM_OK_ACK, MGM_ERROR_ACK, MGM_INFO_ACK, MGM_BIND_ACK,
	MGM_OPTMGMT_ACK, MGM_ADDR_ACK, MGM_CALL_REATTEMPT_IND, MGM_SETUP_IND,
	MGM_MORE_INFO_IND, MGM_INFORMATION_IND, MGM_CONT_CHECK_IND,
	MGM_CONT_TEST_IND, MGM_CONT_REPORT_IND, MGM_SETUP_CON,
	MGM_PROCEEDING_IND, MGM_ALERTING_IND, MGM_PROGRESS_IND, MGM_IBI_IND,
	MGM_CONNECT_IND, MGM_SETUP_COMPLETE_IND, MGM_FORWXFER_IND,
	MGM_SUSPEND_IND, MGM_SUSPEND_CON, MGM_SUSPEND_REJECT_IND,
	MGM_RESUME_IND, MGM_RESUME_CON, MGM_RESUME_REJECT_IND, MGM_REJECT_IND,
	MGM_CALL_FAILURE_IND, MGM_RELEASE_IND, MGM_RELEASE_CON, MGM_RESET_IND,
	MGM_RESET_CON, MGM_BLOCKING_IND, MGM_BLOCKING_CON, MGM_UNBLOCKING_IND,
	MGM_UNBLOCKING_CON, MGM_QUERY_IND, MGM_QUERY_CON, MGM_STOP_IND,
	MGM_MAINT_IND, MGM_START_RESET_IND,
	IBI, RINGING, COMMUNICATION, TONE, LOOPBACK,
	NO_MSG
};

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
	case __TEST_CONTINUE:
		return ("CONTINUE");
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
	case I_GETMSG:
		return ("I_GETMSG");
	case I_PUTMSG:
		return ("I_PUTMSG");
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

void print_string(int child, const char *string);
void
print_ppa(int child, ppa_t * ppa)
{
	char buf[32];

	snprintf(buf, sizeof(buf), "%d:%d:%d", ((*ppa) >> 12) & 0x0f, ((*ppa) >> 8) & 0x0f, (*ppa) & 0x0ff);
	print_string(child, buf);
}
void print_string_val(int child, const char *string, ulong val);
void
print_sdl_stats(int child, sdl_stats_t * s)
{
	if (s->rx_octets)
		print_string_val(child, "rx_octets", s->rx_octets);
	if (s->tx_octets)
		print_string_val(child, "tx_octets", s->tx_octets);
	if (s->rx_overruns)
		print_string_val(child, "rx_overruns", s->rx_overruns);
	if (s->tx_underruns)
		print_string_val(child, "tx_underruns", s->tx_underruns);
	if (s->rx_buffer_overflows)
		print_string_val(child, "rx_buffer_overflows", s->rx_buffer_overflows);
	if (s->tx_buffer_overflows)
		print_string_val(child, "tx_buffer_overflows", s->tx_buffer_overflows);
	if (s->lead_cts_lost)
		print_string_val(child, "lead_cts_lost", s->lead_cts_lost);
	if (s->lead_dcd_lost)
		print_string_val(child, "lead_dcd_lost", s->lead_dcd_lost);
	if (s->carrier_lost)
		print_string_val(child, "carrier_lost", s->carrier_lost);
}

void
print_sdt_stats(int child, sdt_stats_t * s)
{
	if (s->tx_bytes)
		print_string_val(child, "tx_bytes", s->tx_bytes);
	if (s->tx_sus)
		print_string_val(child, "tx_sus", s->tx_sus);
	if (s->tx_sus_repeated)
		print_string_val(child, "tx_sus_repeated", s->tx_sus_repeated);
	if (s->tx_underruns)
		print_string_val(child, "tx_underruns", s->tx_underruns);
	if (s->tx_aborts)
		print_string_val(child, "tx_aborts", s->tx_aborts);
	if (s->tx_buffer_overflows)
		print_string_val(child, "tx_buffer_overflows", s->tx_buffer_overflows);
	if (s->tx_sus_in_error)
		print_string_val(child, "tx_sus_in_error", s->tx_sus_in_error);
	if (s->rx_bytes)
		print_string_val(child, "rx_bytes", s->rx_bytes);
	if (s->rx_sus)
		print_string_val(child, "rx_sus", s->rx_sus);
	if (s->rx_sus_compressed)
		print_string_val(child, "rx_sus_compressed", s->rx_sus_compressed);
	if (s->rx_overruns)
		print_string_val(child, "rx_overruns", s->rx_overruns);
	if (s->rx_aborts)
		print_string_val(child, "rx_aborts", s->rx_aborts);
	if (s->rx_buffer_overflows)
		print_string_val(child, "rx_buffer_overflows", s->rx_buffer_overflows);
	if (s->rx_sus_in_error)
		print_string_val(child, "rx_sus_in_error", s->rx_sus_in_error);
	if (s->rx_sync_transitions)
		print_string_val(child, "rx_sync_transitions", s->rx_sync_transitions);
	if (s->rx_bits_octet_counted)
		print_string_val(child, "rx_bits_octet_counted", s->rx_bits_octet_counted);
	if (s->rx_crc_errors)
		print_string_val(child, "rx_crc_errors", s->rx_crc_errors);
	if (s->rx_frame_errors)
		print_string_val(child, "rx_frame_errors", s->rx_frame_errors);
	if (s->rx_frame_overflows)
		print_string_val(child, "rx_frame_overflows", s->rx_frame_overflows);
	if (s->rx_frame_too_long)
		print_string_val(child, "rx_frame_too_long", s->rx_frame_too_long);
	if (s->rx_frame_too_short)
		print_string_val(child, "rx_frame_too_short", s->rx_frame_too_short);
	if (s->rx_residue_errors)
		print_string_val(child, "rx_residue_errors", s->rx_residue_errors);
	if (s->rx_length_error)
		print_string_val(child, "rx_length_error", s->rx_length_error);
	if (s->carrier_cts_lost)
		print_string_val(child, "carrier_cts_lost", s->carrier_cts_lost);
	if (s->carrier_dcd_lost)
		print_string_val(child, "carrier_dcd_lost", s->carrier_dcd_lost);
	if (s->carrier_lost)
		print_string_val(child, "carrier_lost", s->carrier_lost);
}
void
print_sl_stats(int child, sl_stats_t *s)
{
	if (s->sl_dur_in_service)
		print_string_val(child, "sl_dur_in_service", s->sl_dur_in_service);
	if (s->sl_fail_align_or_proving)
		print_string_val(child, "sl_fail_align_or_proving", s->sl_fail_align_or_proving);
	if (s->sl_nacks_received)
		print_string_val(child, "sl_nacks_received", s->sl_nacks_received);
	if (s->sl_dur_unavail)
		print_string_val(child, "sl_dur_unavail", s->sl_dur_unavail);
	if (s->sl_dur_unavail_failed)
		print_string_val(child, "sl_dur_unavail_failed", s->sl_dur_unavail_failed);
	if (s->sl_sibs_sent)
		print_string_val(child, "sl_sibs_sent", s->sl_sibs_sent);
	if (s->sl_tran_sio_sif_octets)
		print_string_val(child, "sl_tran_sio_sif_octets", s->sl_tran_sio_sif_octets);
	if (s->sl_tran_msus)
		print_string_val(child, "sl_tran_msus", s->sl_tran_msus);
	if (s->sl_recv_sio_sif_octets)
		print_string_val(child, "sl_recv_sio_sif_octets", s->sl_recv_sio_sif_octets);
	if (s->sl_recv_msus)
		print_string_val(child, "sl_recv_msus", s->sl_recv_msus);
	if (s->sl_cong_onset_ind[0])
		print_string_val(child, "sl_cong_onset_ind[0]", s->sl_cong_onset_ind[0]);
	if (s->sl_cong_onset_ind[1])
		print_string_val(child, "sl_cong_onset_ind[1]", s->sl_cong_onset_ind[1]);
	if (s->sl_cong_onset_ind[2])
		print_string_val(child, "sl_cong_onset_ind[2]", s->sl_cong_onset_ind[2]);
	if (s->sl_cong_onset_ind[3])
		print_string_val(child, "sl_cong_onset_ind[3]", s->sl_cong_onset_ind[3]);
	if (s->sl_dur_cong_status[0])
		print_string_val(child, "sl_dur_cong_status[0]", s->sl_dur_cong_status[0]);
	if (s->sl_dur_cong_status[1])
		print_string_val(child, "sl_dur_cong_status[1]", s->sl_dur_cong_status[1]);
	if (s->sl_dur_cong_status[2])
		print_string_val(child, "sl_dur_cong_status[2]", s->sl_dur_cong_status[2]);
	if (s->sl_dur_cong_status[3])
		print_string_val(child, "sl_dur_cong_status[3]", s->sl_dur_cong_status[3]);
	if (s->sl_cong_discd_ind[0])
		print_string_val(child, "sl_cong_discd_ind[0]", s->sl_cong_discd_ind[0]);
	if (s->sl_cong_discd_ind[1])
		print_string_val(child, "sl_cong_discd_ind[1]", s->sl_cong_discd_ind[1]);
	if (s->sl_cong_discd_ind[2])
		print_string_val(child, "sl_cong_discd_ind[2]", s->sl_cong_discd_ind[2]);
	if (s->sl_cong_discd_ind[3])
		print_string_val(child, "sl_cong_discd_ind[3]", s->sl_cong_discd_ind[3]);
}

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
		if (fsn[child] != 0x7f || bsn[child] != 0x7f || fib[child] != 0x80 || bib[child] != 0x80)
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

		if (fsn[other] != 0x7f || bsn[other] != 0x7f || fib[other] != 0x80 || bib[other] != 0x80)
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

	if (verbose > 0 && show)
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
 *  Old Stream Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int
pt_open(void)
{
	int pfd[2];

	if (verbose > 1) {
		printf("                    .  .                            X--X---pipe()           (%d)\n", state);
		fflush(stdout);
	}
	if ((pipe(pfd) < 0)) {
		printf("                                              ****ERROR: pipe failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return FAILURE;
	}
	pt_fd = pfd[0];
	pt_lnk_fd = pfd[1];
	if (verbose > 1) {
		printf("                    .  .                            |  |<--I_SRDOPT---------(%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(pt_fd, I_SRDOPT, RMSGD) < 0) {
		printf("                                              ****ERROR: ioctl failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                    .  .                            |  |<--I_PUSH-----------(%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(pt_fd, I_PUSH, "pipemod") < 0) {
		printf("                                              ****ERROR: push failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
pt_close(void)
{
	if (verbose > 1) {
		printf("                    .  .                            |  |<--I_POP------------(%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(pt_fd, I_POP, 0) < 0) {
		printf("                                              ****ERROR: pop failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("                    .  .                            X--X---close()          (%d)\n", state);
		fflush(stdout);
	}
	if (close(pt_fd) < 0) {
		printf("                                              ****ERROR: close pipe[0] failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (close(pt_lnk_fd) < 0) {
		printf("                                              ****ERROR: close pipe[1] failed\n");
		printf("                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
pt_start(void)
{
	if (pt_open() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int
pt_stop(void)
{
	if (pt_close() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  IUT Intitialization and Configuration
 */
static int
iut_open(void)
{
	if (verbose > 1) {
		printf("---open()-----------X  .                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if ((iut_cpc_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("***************ERROR: open failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---open()-tst-------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if ((iut_tst_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---open()-mgm-------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if ((iut_mgm_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---open()-mnt-------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if ((iut_mnt_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---I_SRDOPT-------->|  |                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_cpc_fd, I_SRDOPT, RMSGD) < 0) {
		printf("***************ERROR: ioctl failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---I_SRDOPT-tst-----+->|                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_tst_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---I_SRDOPT-mgm-----+->|                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---I_SRDOPT-mnt-----+->|                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mnt_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
iut_close(void)
{
	if (verbose > 1) {
		printf("---close()----------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (close(iut_mnt_fd) < 0) {
		printf("******************ERROR: close mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---close()----------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (close(iut_mgm_fd) < 0) {
		printf("******************ERROR: close mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---close()----------+--X                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (close(iut_tst_fd) < 0) {
		printf("******************ERROR: close tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	if (verbose > 1) {
		printf("---close()----------X  .                            |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (close(iut_cpc_fd) < 0) {
		printf("***************ERROR: close cpc failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
iut_config(void)
{
	int i;
	struct strioctl ioc;
	struct {
		struct isup_config conf;
		struct isup_conf_sp sp;
	} conf_sp;
	struct {
		struct isup_config conf;
		struct isup_conf_sr sr;
	} conf_sr;
	struct {
		struct isup_config conf;
		struct isup_conf_tg tg;
	} conf_tg;
	struct {
		struct isup_config conf;
		struct isup_conf_cg cg;
	} conf_cg;
	struct {
		struct isup_config conf;
		struct isup_conf_ct ct;
	} conf_ct;

	conf_sp.conf.type = ISUP_OBJ_TYPE_SP;
	conf_sp.conf.id = 1;
	conf_sp.conf.cmd = ISUP_ADD;
	conf_sp.sp.add = loc_addr;
	conf_sp.sp.proto.pvar = SS7_PVAR_ITUT_96;
	conf_sp.sp.proto.popt = SS7_POPT_UPT;
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf_sp);
	ioc.ic_dp = (char *) &conf_sp;
	if (verbose > 2) {
		printf("---ISUP_IOCSCONFIG--+->| (ISUP_ADD sp = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding sp=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf_sr.conf.type = ISUP_OBJ_TYPE_SR;
	conf_sr.conf.id = 1;
	conf_sr.conf.cmd = ISUP_ADD;
	conf_sr.sr.spid = 1;
	conf_sr.sr.add = rem_addr;
	conf_sr.sr.proto.pvar = SS7_PVAR_ITUT_96;
	conf_sr.sr.proto.popt = SS7_POPT_UPT;
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf_sr);
	ioc.ic_dp = (char *) &conf_sr;
	if (verbose > 2) {
		printf("---ISUP_IOCSCONFIG--+->| (ISUP_ADD sr = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding sr=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf_tg.conf.type = ISUP_OBJ_TYPE_TG;
	conf_tg.conf.id = 1;
	conf_tg.conf.cmd = ISUP_ADD;
	conf_tg.tg.srid = 1;
	conf_tg.tg.proto.pvar = SS7_PVAR_ITUT_96;
	conf_tg.tg.proto.popt = SS7_POPT_UPT;
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf_tg);
	ioc.ic_dp = (char *) &conf_tg;
	if (verbose > 2) {
		printf("---ISUP_IOCSCONFIG--+->| (ISUP_ADD tg = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding tg=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf_cg.conf.type = ISUP_OBJ_TYPE_CG;
	conf_cg.conf.id = 1;
	conf_cg.conf.cmd = ISUP_ADD;
	conf_cg.cg.srid = 1;
	conf_cg.cg.proto.pvar = SS7_PVAR_ITUT_96;
	conf_cg.cg.proto.popt = SS7_POPT_UPT;
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf_cg);
	ioc.ic_dp = (char *) &conf_cg;
	if (verbose > 2) {
		printf("---ISUP_IOCSCONFIG--+->| (ISUP_ADD cg = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding cg=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf_ct);
	ioc.ic_dp = (char *) &conf_ct;
	for (i = 1; i < 32; i++) {
		if (i == 16)
			continue;
		conf_ct.conf.type = ISUP_OBJ_TYPE_CT;
		conf_ct.conf.id = i;
		conf_ct.conf.cmd = ISUP_ADD;
		conf_ct.ct.cgid = 1;
		conf_ct.ct.tgid = 1;
		conf_ct.ct.cic = i;
		if (verbose > 2) {
			printf("---ISUP_IOCSCONFIG--+->| (ISUP_ADD ct = %2d)         |  |                    (%d)\n", state, i);
			fflush(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("******************ERROR: adding ct=%d\n", i);
			printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			return __RESULT_FAILURE;
		}
	}
	return __RESULT_SUCCESS;
}

static int
iut_unconfig(void)
{
	int i;
	struct isup_config conf;
	struct strioctl ioc = { ISUP_IOCCCONFIG, 0, sizeof(conf), (char *) &conf };

	for (i = 1; i < 32; i++) {
		if (i == 16)
			continue;
		conf.type = ISUP_OBJ_TYPE_CT;
		conf.id = i;
		conf.cmd = ISUP_DEL;
		if (verbose > 2) {
			printf("---ISUP_IOCCCONFIG--+->| (ISUP_DEL ct = %2d)         |  |                    (%d)\n", i, state);
			fflush(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("******************ERROR: deleting ct=%d\n", i);
			printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			return __RESULT_FAILURE;
		}
	}
	conf.type = ISUP_OBJ_TYPE_CG;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf("---ISUP_IOCCCONFIG--+->| (ISUP_DEL cg = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting cg=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_TG;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf("---ISUP_IOCCCONFIG--+->| (ISUP_DEL tg = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting tg=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_SR;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf("---ISUP_IOCCCONFIG--+->| (ISUP_DEL sr = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting sr=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_SP;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf("---ISUP_IOCCCONFIG--+->| (ISUP_DEL sp = 1)          |  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting sp=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return __RESULT_SUCCESS;
}

static int
iut_link(void)
{
	struct {
		struct isup_config config;
		struct isup_conf_mtp mtp;
	} conf;
	struct strioctl ioc;

	if (verbose > 1) {
		printf("---I_LINK-----------+->|<---------------------------X  |                    (%d)\n", state);
		fflush(stdout);
	}
	if ((pt_lnk_id = ioctl(iut_mgm_fd, I_LINK, pt_lnk_fd)) == -1) {
		printf("***************** ERROR: link failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	conf.config.type = ISUP_OBJ_TYPE_MT;
	conf.config.id = 1;
	conf.config.cmd = ISUP_ADD;
	conf.mtp.spid = 1;
	conf.mtp.srid = 1;
	conf.mtp.muxid = pt_lnk_id;
	conf.mtp.proto.pvar = SS7_PVAR_ITUT_96;
	conf.mtp.proto.popt = 0;
	ioc.ic_cmd = ISUP_IOCSCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf);
	ioc.ic_dp = (char *) &conf;
	if (verbose > 2) {
		printf("---ISUP_IOCSCONFIG--+->| ISUP_ADD (mtp = 1)            |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: config failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
iut_unlink(void)
{
#if 0
	struct isup_config conf;
	struct strioctl ioc;

	conf.type = ISUP_OBJ_TYPE_MT;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	ioc.ic_cmd = ISUP_IOCCCONFIG;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(conf);
	ioc.ic_dp = (char *) &conf;
	if (verbose > 2) {
		printf("---ISUP_IOCCCONFIG--+->| ISUP_DEL (mtp = 1)            |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: config failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
#endif
	if (verbose > 1) {
		printf("---I_UNLINK---------+->|<---------------------------X  |                    (%d)\n", state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_UNLINK, pt_lnk_id) < 0) {
		printf("******************ERROR: unlink failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
iut_option(void)
{
	int i;
	struct {
		struct isup_option hdr;
		union {
			struct isup_opt_conf_ct ct_opt;
			struct isup_opt_conf_cg cg_opt;
			struct isup_opt_conf_tg tg_opt;
			struct isup_opt_conf_sr sr_opt;
			struct isup_opt_conf_sp sp_opt;
			struct isup_opt_conf_mtp mtp_opt;
			struct isup_opt_conf_df df_opt;
		} opts;
	} opt;
	struct strioctl ioc;

	/* circuit options */
	opt.opts.ct_opt = iut_ct_opt;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.ct_opt);
	ioc.ic_dp = (char *) &opt;
	for (i = 1; i < 32; i++) {
		if (i == 16)
			continue;
		opt.hdr.type = ISUP_OBJ_TYPE_CT;
		opt.hdr.id = i;
		if (verbose > 2) {
			printf("---ISUP_IOCSOPTIONS-+->| ct id = %2d                    |                    (%d)\n", i, state);
			fflush(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("***************** ERROR: set option failed\n");
			printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			goto failure;
		}
	}
	/* circuit group options */
	opt.opts.cg_opt = iut_cg_opt;
	opt.hdr.type = ISUP_OBJ_TYPE_CG;
	opt.hdr.id = 1;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.cg_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| cg id = %2d                    |                    (%d)\n", 1, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	/* trunk group options */
	opt.opts.tg_opt = iut_tg_opt;
	if (timer_scale > 1) {
		/* scale timers down by scaling factor */
		opt.opts.tg_opt.t1 = iut_tg_opt.t1 / timer_scale;	/* waiting for RLC retry */
		opt.opts.tg_opt.t2 = iut_tg_opt.t2 / timer_scale;	/* waiting for RES */
		opt.opts.tg_opt.t3 = iut_tg_opt.t3 / timer_scale;	/* waiting OVL timeout */
		opt.opts.tg_opt.t5 = iut_tg_opt.t5 / timer_scale;	/* waiting for RLC
									   maintenance */
		opt.opts.tg_opt.t6 = iut_tg_opt.t6 / timer_scale;	/* waiting for RES */
		opt.opts.tg_opt.t7 = iut_tg_opt.t7 / timer_scale;	/* waiting for ACM/ANM/CON */
		opt.opts.tg_opt.t8 = iut_tg_opt.t8 / timer_scale;	/* waiting for COT */
		opt.opts.tg_opt.t9 = iut_tg_opt.t9 / timer_scale;	/* waiting for ANM/CON */
		opt.opts.tg_opt.t10 = iut_tg_opt.t10 / timer_scale;	/* waiting more
									   information.
									   Interworking */
		opt.opts.tg_opt.t11 = iut_tg_opt.t11 / timer_scale;	/* waiting for ACM,
									   Interworking */
		opt.opts.tg_opt.t12 = iut_tg_opt.t12 / timer_scale;	/* waiting for BLA retry */
		opt.opts.tg_opt.t13 = iut_tg_opt.t13 / timer_scale;	/* waiting for BLA
									   maintenance */
		opt.opts.tg_opt.t14 = iut_tg_opt.t14 / timer_scale;	/* waiting for UBA retry */
		opt.opts.tg_opt.t15 = iut_tg_opt.t15 / timer_scale;	/* waiting for UBA
									   maintenance */
		opt.opts.tg_opt.t16 = iut_tg_opt.t16 / timer_scale;	/* waiting for RLC retry */
		opt.opts.tg_opt.t17 = iut_tg_opt.t17 / timer_scale;	/* waiting for RLC
									   maintenance */
		opt.opts.tg_opt.t24 = iut_tg_opt.t24 / timer_scale;	/* waiting for continuity
									   IAM */
		opt.opts.tg_opt.t25 = iut_tg_opt.t25 / timer_scale;	/* waiting for continuity
									   CCR retry */
		opt.opts.tg_opt.t26 = iut_tg_opt.t26 / timer_scale;	/* waiting for continuity
									   CCR maintenance */
		opt.opts.tg_opt.t27 = iut_tg_opt.t27 / timer_scale;	/* waiting for COT reset */
		opt.opts.tg_opt.t31 = iut_tg_opt.t31 / timer_scale;	/* call reference guard */
		opt.opts.tg_opt.t32 = iut_tg_opt.t32 / timer_scale;	/* waiting to send E2E
									   message */
		opt.opts.tg_opt.t33 = iut_tg_opt.t33 / timer_scale;	/* waiting for INF */
		opt.opts.tg_opt.t34 = iut_tg_opt.t34 / timer_scale;	/* waiting for SEG */
		opt.opts.tg_opt.t35 = iut_tg_opt.t35 / timer_scale;	/* waiting more
									   information. */
		opt.opts.tg_opt.t36 = iut_tg_opt.t36 / timer_scale;	/* waiting for COT/REL */
		opt.opts.tg_opt.t37 = iut_tg_opt.t37 / timer_scale;	/* waiting echo control
									   device */
		opt.opts.tg_opt.t38 = iut_tg_opt.t38 / timer_scale;	/* waiting for RES */
		opt.opts.tg_opt.tacc_r = iut_tg_opt.tacc_r / timer_scale;	/* */
		opt.opts.tg_opt.tccr = iut_tg_opt.tccr / timer_scale;	/* */
		opt.opts.tg_opt.tccr_r = iut_tg_opt.tccr_r / timer_scale;	/* */
		opt.opts.tg_opt.tcra = iut_tg_opt.tcra / timer_scale;	/* */
		opt.opts.tg_opt.tcrm = iut_tg_opt.tcrm / timer_scale;	/* */
		opt.opts.tg_opt.tcvt = iut_tg_opt.tcvt / timer_scale;	/* */
		opt.opts.tg_opt.texm_d = iut_tg_opt.texm_d / timer_scale;	/* */
	}
	opt.hdr.type = ISUP_OBJ_TYPE_TG;
	opt.hdr.id = 1;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.tg_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| tg id = %2d                    |                    (%d)\n", 1, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	/* signalling relation options */
	opt.opts.sr_opt = iut_sr_opt;
	if (timer_scale > 1) {
		opt.opts.sr_opt.t4 = iut_sr_opt.t4 / timer_scale;	/* waiting for UPA/other */
		opt.opts.sr_opt.t18 = iut_sr_opt.t18 / timer_scale;	/* waiting CGBA retry */
		opt.opts.sr_opt.t19 = iut_sr_opt.t19 / timer_scale;	/* waiting CGBA maintenance 
									 */
		opt.opts.sr_opt.t20 = iut_sr_opt.t20 / timer_scale;	/* waiting CGUA retry */
		opt.opts.sr_opt.t21 = iut_sr_opt.t21 / timer_scale;	/* waiting CGUA maintenance 
									 */
		opt.opts.sr_opt.t22 = iut_sr_opt.t22 / timer_scale;	/* waiting GRA retry */
		opt.opts.sr_opt.t23 = iut_sr_opt.t23 / timer_scale;	/* waiting GRA maintenance */
		opt.opts.sr_opt.t28 = iut_sr_opt.t28 / timer_scale;	/* waiting CQR */
		opt.opts.sr_opt.t29 = iut_sr_opt.t29 / timer_scale;	/* congestion attack timer */
		opt.opts.sr_opt.t30 = iut_sr_opt.t30 / timer_scale;	/* congestion decay timer */
		opt.opts.sr_opt.tcgb = iut_sr_opt.tcgb / timer_scale;	/* */
		opt.opts.sr_opt.tgrs = iut_sr_opt.tgrs / timer_scale;	/* */
		opt.opts.sr_opt.thga = iut_sr_opt.thga / timer_scale;	/* */
		opt.opts.sr_opt.tscga = iut_sr_opt.tscga / timer_scale;	/* */
		opt.opts.sr_opt.tscga_d = iut_sr_opt.tscga_d / timer_scale;	/* */
	}
	opt.hdr.type = ISUP_OBJ_TYPE_SR;
	opt.hdr.id = 1;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.sr_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| sr id = %2d                    |                    (%d)\n", 1, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	/* signalling point options */
	opt.opts.sp_opt = iut_sp_opt;
	opt.hdr.type = ISUP_OBJ_TYPE_SP;
	opt.hdr.id = 1;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.sp_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| sp id = %2d                    |                    (%d)\n", 1, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	/* message transfer part options */
	opt.opts.mtp_opt = iut_mtp_opt;
	opt.hdr.type = ISUP_OBJ_TYPE_MT;
	opt.hdr.id = 1;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.mtp_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| mtp id = %2d                   |                    (%d)\n", 1, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	/* default options */
	opt.opts.df_opt = iut_df_opt;
	opt.hdr.type = ISUP_OBJ_TYPE_DF;
	opt.hdr.id = 0;
	ioc.ic_cmd = ISUP_IOCSOPTIONS;
	ioc.ic_timout = 0;
	ioc.ic_len = sizeof(opt.hdr) + sizeof(opt.opts.df_opt);
	ioc.ic_dp = (char *) &opt;
	if (verbose > 2) {
		printf("---ISUP_IOCSOPTIONS-+->| df id = %2d                    |                    (%d)\n", 0, state);
		fflush(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
};

static int
iut_bind(void)
{
	mprim.addr.add.scope = iut_tst_addr.scope;
	mprim.addr.add.id = iut_tst_addr.id;
	mprim.addr.add.cic = iut_tst_addr.cic;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.bind_flags = CC_TOKEN_REQUEST | CC_TEST;
	iut_tst_signal(BIND_REQ);
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) || mprim.addr.add.scope != iut_tst_addr.scope || mprim.addr.add.id != iut_tst_addr.id || mprim.addr.add.cic != iut_tst_addr.cic)
		goto failure;
	mprim.addr.add.scope = iut_mgm_addr.scope;
	mprim.addr.add.id = iut_mgm_addr.id;
	mprim.addr.add.cic = iut_mgm_addr.cic;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.bind_flags = CC_TOKEN_REQUEST | CC_MANAGEMENT;
	iut_mgm_signal(BIND_REQ);
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) || mprim.addr.add.scope != iut_mgm_addr.scope || mprim.addr.add.id != iut_mgm_addr.id || mprim.addr.add.cic != iut_mgm_addr.cic)
		goto failure;
	mprim.addr.add.scope = iut_mnt_addr.scope;
	mprim.addr.add.id = iut_mnt_addr.id;
	mprim.addr.add.cic = iut_mnt_addr.cic;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.bind_flags = CC_TOKEN_REQUEST | CC_MAINTENANCE;
	iut_mnt_signal(BIND_REQ);
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) || mprim.addr.add.scope != iut_mnt_addr.scope || mprim.addr.add.id != iut_mnt_addr.id || mprim.addr.add.cic != iut_mnt_addr.cic)
		goto failure;
	cprim.addr.add.scope = iut_cpc_addr.scope;
	cprim.addr.add.id = iut_cpc_addr.id;
	cprim.addr.add.cic = iut_cpc_addr.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.bind_flags = CC_TOKEN_REQUEST;
	iut_cpc_signal(BIND_REQ);
	if (any_event() != CPC_BIND_ACK || cprim.addr.len != sizeof(cprim.addr.add) || cprim.addr.add.scope != iut_cpc_addr.scope || cprim.addr.add.id != iut_cpc_addr.id || cprim.addr.add.cic != iut_cpc_addr.cic)
		goto failure;
#if 0
	iut_tst_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		goto failure;
	iut_mgm_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		goto failure;
	iut_mnt_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		goto failure;
	iut_cpc_signal(ADDR_REQ);
	if (any_event() != CPC_ADDR_ACK)
		goto failure;
#endif
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
iut_unbind(void)
{
	iut_cpc_signal(UNBIND_REQ);
	if (any_event() != CPC_OK_ACK)
		goto failure;
	iut_tst_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		goto failure;
	iut_mgm_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		goto failure;
	iut_mnt_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
iut_start(void)
{
	if (iut_open() != __RESULT_SUCCESS)
		goto failure;
	if (iut_config() != __RESULT_SUCCESS)
		goto failure;
	if (iut_link() != __RESULT_SUCCESS)
		goto failure;
	if (iut_option() != __RESULT_SUCCESS)
		goto failure;
	if (iut_bind() != __RESULT_SUCCESS)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
iut_stop(void)
{
	if (iut_unbind() != __RESULT_SUCCESS)
		goto failure;
	if (iut_unlink() != __RESULT_SUCCESS)
		goto failure;
	if (iut_unconfig() != __RESULT_SUCCESS)
		goto failure;
	if (iut_close() != __RESULT_SUCCESS)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Stream Initialization
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
		if (test_open(child, devname, O_RDWR) != __RESULT_SUCCESS)
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

static int
begin_tests(void)
{
	if (pt_start() != __RESULT_SUCCESS)
		goto failure;
	if (iut_start() != __RESULT_SUCCESS)
		goto failure;
	show_acks = 1;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
end_tests(void)
{
	show_acks = 0;
	if (iut_stop() != __RESULT_SUCCESS)
		goto failure;
	if (pt_stop() != __RESULT_SUCCESS)
		goto failure;
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

union primitives {
	lmi_ulong prim;
	union LMI_primitives lmi;
	union SDL_primitives sdl;
	union SDT_primitives sdt;
	union SL_primitives sl;
};

static int
do_signal(int child, int action)
{
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	union primitives *p = (typeof(p)) cbuf;
	struct strioctl ic;
	int err;

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
			print_tx_msg_sn(child, "FISU(bad fib)", bib[child] | bsn[child], (fib[child] | fsn[child]) ^ 0x80);
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
		if (do_signal(child, __TEST_SDT_CONFIG) != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		do_signal(child, __TEST_FISU_S);
		do_signal(child, __TEST_FISU_S);
		config->sdt.f = SDT_FLAGS_ONE;
		if (do_signal(child, __TEST_SDT_CONFIG) != __RESULT_SUCCESS)
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
		if (do_signal(child, __TEST_SDT_CONFIG) != __RESULT_SUCCESS)
			return __RESULT_INCONCLUSIVE;
		do_signal(child, __TEST_MSU_S);
		do_signal(child, __TEST_MSU_S);
		config->sdt.f = SDT_FLAGS_ONE;
		if (do_signal(child, __TEST_SDT_CONFIG) != __RESULT_SUCCESS)
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

/*
 *  The following group cannot really be performed on the PT (child == 1) and
 *  they are only used to print the messages that would appear if the PT were
 *  functioning similar to the IUT.
 */

	case __TEST_POWER_ON:
		if (child == CHILD_PTU) {
			ctrl->len = sizeof(p->sdt.daedt_start_req);
			p->sdt.daedt_start_req.sdt_primitive = SDT_DAEDT_START_REQ;
			data = NULL;
			test_putpmsg(child, ctrl, data, test_pband, test_pflags);
			ctrl->len = sizeof(p->sdt.daedr_start_req);
			p->sdt.daedr_start_req.sdt_primitive = SDT_DAEDR_START_REQ;
			data = NULL;
		} else {
			ctrl->len = sizeof(p->sl.power_on_req);
			p->sl.power_on_req.sl_primitive = SL_POWER_ON_REQ;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
		}
		print_command_state(child, ":power on");
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_START:
		ctrl->len = sizeof(p->sl.start_req);
		p->sl.start_req.sl_primitive = SL_START_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":start");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_STOP:
		ctrl->len = sizeof(p->sl.stop_req);
		p->sl.stop_req.sl_primitive = SL_STOP_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":stop");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_LPO:
		ctrl->len = sizeof(p->sl.local_proc_outage_req);
		p->sl.local_proc_outage_req.sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":set lpo");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_LPR:
		ctrl->len = sizeof(p->sl.resume_req);
		p->sl.resume_req.sl_primitive = SL_RESUME_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear lpo");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CONTINUE:
		if (((ss7_pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) || ((ss7_pvar & SS7_PVAR_YR) == SS7_PVAR_88)) {
			ctrl->len = sizeof(p->sl.continue_req);
			p->sl.continue_req.sl_primitive = SL_CONTINUE_REQ;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			print_command_state(child, ":continue");
			if (child != CHILD_PTU)
				return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		}
		return __RESULT_SUCCESS;
	case __TEST_CONG_A:
		ctrl->len = sizeof(p->sl.cong_accept_req);
		p->sl.cong_accept_req.sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":make congested");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CONG_D:
		ctrl->len = sizeof(p->sl.cong_discard_req);
		p->sl.cong_discard_req.sl_primitive = SL_CONGESTION_DISCARD_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":make congested");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_NO_CONG:
		ctrl->len = sizeof(p->sl.no_cong_req);
		p->sl.no_cong_req.sl_primitive = SL_NO_CONGESTION_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear congested");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_EMERG:
		ctrl->len = sizeof(p->sl.emergency_req);
		p->sl.emergency_req.sl_primitive = SL_EMERGENCY_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":set emerg");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CEASE:
		ctrl->len = sizeof(p->sl.emergency_ceases_req);
		p->sl.emergency_ceases_req.sl_primitive = SL_EMERGENCY_CEASES_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear emerg");
		if (child != CHILD_PTU)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return __RESULT_SUCCESS;
	case __TEST_CLEARB:
		ctrl->len = sizeof(p->sl.clear_buffers_req);
		p->sl.clear_buffers_req.sl_primitive = SL_CLEAR_BUFFERS_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_command_state(child, ":clear buffers");
		if (child != CHILD_PTU)
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
	case __TEST_TX_BREAK:
		print_command_state(child, ":break Tx");
		ic.ic_cmd = SDL_IOCCDISCTX;
		ic.ic_timout = 0;
		ic.ic_len = 0;
		ic.ic_dp = NULL;
		return test_ioctl(child, I_STR, (intptr_t) &ic);
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
	case __TEST_SDL_STATS:
		if (show && verbose > 1)
			print_command_state(child, ":stats sdl");
		ic.ic_cmd = SDL_IOCGSTATS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(stats->sdl);
		ic.ic_dp = (char *) &stats->sdl;
		if (!(err = test_ioctl(child, I_STR, (intptr_t) &ic)))
			if (show && verbose > 1)
				print_sdl_stats(child, &stats->sdl);
		return (err);
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
	case __TEST_SDT_STATS:
		if (show && verbose > 1)
			print_command_state(child, ":stats sdt");
		ic.ic_cmd = SDT_IOCGSTATS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(stats->sdt);
		ic.ic_dp = (char *) &stats->sdt;
		if (!(err = test_ioctl(child, I_STR, (intptr_t) &ic)))
			if (show && verbose > 1)
				print_sdt_stats(child, &stats->sdt);
		return (err);
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
	case __TEST_SL_STATS:
		if (show && verbose > 1)
			print_command_state(child, ":stats sl");
		ic.ic_cmd = SL_IOCGSTATS;
		ic.ic_timout = 0;
		ic.ic_len = sizeof(stats->sl);
		ic.ic_dp = (char *) &stats->sl;
		if ((err = test_ioctl(child, I_STR, (intptr_t) &ic)))
			if (show && verbose > 1)
				print_sl_stats(child, &stats->sl);
		return (err);

	case __TEST_ATTACH_REQ:
		ctrl->len = sizeof(p->lmi.attach_req) + (ADDR_buffer ? ADDR_length : 0);
		p->lmi.attach_req.lmi_primitive = LMI_ATTACH_REQ;
		p->lmi.attach_req.lmi_ppa_length = ADDR_length;
		p->lmi.attach_req.lmi_ppa_offset = sizeof(p->lmi.attach_req);
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + sizeof(p->lmi.attach_req), ADDR_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if (show && verbose > 1) {
			print_command_state(child, ":attach");
			print_ppa(child, (ppa_t *) p->lmi.attach_req.lmi_ppa);
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
		p->lmi.enable_req.lmi_rem_length = ADDR_length;
		p->lmi.enable_req.lmi_rem_offset = sizeof(p->lmi.enable_req);
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + sizeof(p->lmi.enable_req), ADDR_length);
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		if (show && verbose > 1) {
			print_command_state(child, ":enable");
			print_ppa(child, (ppa_t *) p->lmi.enable_req.lmi_rem);
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
	int other = (child + 1) % 2;

	if (data->len >= 0) {
		switch (child) {
		default:
			event = __TEST_DATA;
			print_rx_data(child, "M_DATA----------", data->len);
			break;
		case 0:
		{
			bib[other] = data->buf[0] & 0x80;
			bsn[other] = data->buf[0] & 0x7f;
			fib[other] = data->buf[1] & 0x80;
			fsn[other] = data->buf[1] & 0x7f;
			li[other] = data->buf[2] & 0x3f;
			sio[other] = data->buf[3] & 0x7;
			bsn[child] = fsn[other];
			switch (li[other]) {
			case 0:
				event = __TEST_FISU;
				break;
			case 1:
				event = sio[other] = data->buf[3] & 0x7;
				switch (sio[other]) {
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
				event = sio[other] = data->buf[4] & 0x7;
				switch (sio[other]) {
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
				if (event != oldret || oldisb != (((bib[other] | bsn[other]) << 8) | (fib[other] | fsn[other]))) {
					// if (oldisb == (((bib[other] | bsn[other]) << 8) |
					// (fib[other] |
					// fsn[other])) &&
					// ((event == __TEST_FISU && oldret == __TEST_MSU) ||
					// (event == __TEST_MSU && oldret == __TEST_FISU))) {
					// if (event == __TEST_MSU && !expand)
					// cntmsg++;
					// } else
					// cntret = 0;
					oldret = event;
					oldisb = ((bib[other] | bsn[other]) << 8) | (fib[other] | fsn[other]);
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
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIO");
					else
						snprintf(buf, sizeof(buf), "SIO[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIN:
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIN");
					else
						snprintf(buf, sizeof(buf), "SIN[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIE:
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIE");
					else
						snprintf(buf, sizeof(buf), "SIE[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIOS:
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIOS");
					else
						snprintf(buf, sizeof(buf), "SIOS[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIPO:
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIPO");
					else
						snprintf(buf, sizeof(buf), "SIPO[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIB:
					if (li[other] == 1)
						snprintf(buf, sizeof(buf), "SIB");
					else
						snprintf(buf, sizeof(buf), "SIB[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIX:
					snprintf(buf, sizeof(buf), "LSSU(invalid)[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_SIX2:
					snprintf(buf, sizeof(buf), "LSSU(invalid)[%d]", li[other]);
					print_rx_msg(child, buf);
					break;
				case __TEST_MSU:
					if (show_msus) {
						snprintf(buf, sizeof(buf), "MSU[%d]", li[other]);
						print_rx_msg(child, buf);
					}
					break;
				}
			}
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
		switch ((last_prim = p->prim)) {
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
		case SDT_RC_SIGNAL_UNIT_IND:
			if (verbose > 5 && show_msg)
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
			if (verbose > 5 && show_msg)
				print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
		case SDT_RC_CONGESTION_ACCEPT_IND:
		case SDT_RC_CONGESTION_DISCARD_IND:
		case SDT_RC_NO_CONGESTION_IND:
			print_command_state(child, prim_string(p->prim));
			event = __EVENT_UNKNOWN;
			break;
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
							fprintf(stdout, "%02X", (uint8_t) ctrl.buf[i]);
						fprintf(stdout, "]\n");
						fprintf(stdout, "[");
						for (i = 0; i < data.len; i++)
							fprintf(stdout, "%02X", (uint8_t) data.buf[i]);
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

static const char *
scope_string(int s)
{
	switch (s) {
	case ISUP_SCOPE_CT:
		return ("ct");
	case ISUP_SCOPE_CG:
		return ("cg");
	case ISUP_SCOPE_TG:
		return ("tg");
	case ISUP_SCOPE_SR:
		return ("sr");
	case ISUP_SCOPE_SP:
		return ("sp");
	case ISUP_SCOPE_DF:
		return ("df");
	case ISUP_SCOPE_CIC:
		return ("cic");
	default:
		return ("??");
	}
}

const char *
event_string(int e)
{
	switch (e) {
	case IAM:
		return ("IAM");
	case SAM:
		return ("SAM");
	case INR:
		return ("INR");
	case INF:
		return ("INF");
	case COT:
		return ("COT");
	case ACM:
		return ("ACM");
	case CON:
		return ("CON");
	case FOT:
		return ("FOT");
	case ANM:
		return ("ANM");
	case REL:
		return ("REL");
	case SUS:
		return ("SUS");
	case RES:
		return ("RES");
	case RLC:
		return ("RLC");
	case CCR:
		return ("CCR");
	case RSC:
		return ("RSC");
	case BLO:
		return ("BLO");
	case UBL:
		return ("UBL");
	case BLA:
		return ("BLA");
	case UBA:
		return ("UBA");
	case GRS:
		return ("GRS");
	case CGB:
		return ("CGB");
	case CGU:
		return ("CGU");
	case CGBA:
		return ("CGBA");
	case CGUA:
		return ("CGUA");
	case CMR:
		return ("CMR");
	case CMC:
		return ("CMC");
	case CMRJ:
		return ("CMRJ");
	case FAR:
		return ("FAR");
	case FAA:
		return ("FAA");
	case FRJ:
		return ("FRJ");
	case FAD:
		return ("FAD");
	case FAI:
		return ("FAI");
	case LPA:
		return ("LPA");
	case DRS:
		return ("DRS");
	case PAM:
		return ("PAM");
	case GRA:
		return ("GRA");
	case CQM:
		return ("CQM");
	case CQR:
		return ("CQR");
	case CPG:
		return ("CPC");
	case USR:
		return ("USR");
	case UCIC:
		return ("UCIC");
	case CFN:
		return ("CFN");
	case OLM:
		return ("OLN");
	case CRG:
		return ("CRG");
	case NRM:
		return ("NRM");
	case FAC:
		return ("FAC");
	case UPT:
		return ("UPT");
	case UPA:
		return ("UPA");
	case IDR:
		return ("IDR");
	case IRS:
		return ("IRS");
	case SGM:
		return ("SGM");
	case CRA:
		return ("CRA");
	case CRM:
		return ("CRM");
	case CVR:
		return ("CVR");
	case CVT:
		return ("CVT");
	case EXM:
		return ("EXM");
	case NON:
		return ("NON");
	case LLM:
		return ("LLM");
	case CAK:
		return ("CAK");
	case TCM:
		return ("TCM");
	case MCP:
		return ("MCP");
	case PAUSE:
		return ("MTP-PAUSE");
	case RESUME:
		return ("MTP-RESUME");
	case RESTART_COMPLETE:
		return ("MTP-RESTART-COMPLETE");
	case USER_PART_UNKNOWN:
		return ("MTP-STATUS(Unknown)");
	case USER_PART_UNEQUIPPED:
		return ("MTP-STATUS(Unequiped)");
	case USER_PART_UNAVAILABLE:
		return ("MTP-STATUS(Unavailable)");
	case CONGESTION:
		return ("MTP-STATUS(Congestion)");
	default:
		return ("???");
	}
}

#define BUFSIZE 300

static int state = 0;
int count = 0;
int tries = 0;
int expand = 0;
static long beg_time = 0;

int pt_fd = 0;
int pt_lnk_fd = 0;
int pt_lnk_id = 0;
unsigned char pt_dat[BUFSIZE];
unsigned char pt_ctl[BUFSIZE];

int iut_cpc_fd = 0;
int iut_mgm_fd = 0;
int iut_tst_fd = 0;
int iut_mnt_fd = 0;
unsigned char iut_dat[BUFSIZE];
unsigned char iut_ctl[BUFSIZE];

#define send pt_send

extern size_t strnlen(__const char *, size_t);

static int
send(int msg)
{
	int ret = __RESULT_SUCCESS;
	int i;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*pt_dat), 0, (char *) pt_dat };
	union MTP_primitives *m = (typeof(m)) cbuf;
	unsigned char *d = pt_dat;
	unsigned char *p = pt_dat;

	/* precharge buffer */
	*d++ = pmsg.cic;
	*d++ = pmsg.cic >> 8;
	switch (msg) {
	case IAM:
		printf("                    |<-+---------------------%3ld--IAM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_IAM;
		*d++ = pmsg.nci;
		*d++ = pmsg.fci;
		*d++ = pmsg.fci >> 8;
		*d++ = pmsg.cpc;
		*d++ = pmsg.tmr;
		p = d + 2;
		*d = p - d;
		d++;		/* pointer to cdpn */
		pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
		*p++ = 3 + ((pmsg.cdpn.len + 1) >> 1) + 1;	/* pointer to optional parameters */
		*p++ = 2 + ((pmsg.cdpn.len + 1) >> 1);	/* cdpn len */
		*p++ = pmsg.cdpn.nai | ((pmsg.cdpn.len & 0x1) ? 0x80 : 0);	/* nai and o/e */
		*p++ = 0x10;	/* inn allowed E.164 */
		for (i = 0; i < pmsg.cdpn.len; i += 2)
			*p++ = (pmsg.cdpn.num[i] & 0x0f) | ((pmsg.cdpn.num[i + 1] & 0x0f) << 4);
		/* optional parameters */
		*d = p - d;
		d++;
		/* O cgpn */
		*p++ = ISUP_PT_CGPN;
		pmsg.cdpn.len = strnlen(pmsg.cgpn.num, 24);
		*p++ = 2 + ((pmsg.cgpn.len + 1) >> 1);	/* cgpn len */
		*p++ = pmsg.cgpn.nai | ((pmsg.cgpn.len & 0x1) ? 0x80 : 0);	/* nai and o/e */
		*p++ = 0x13;	/* E.164, presentation allowed, user prov verified */
		for (i = 0; i < pmsg.cgpn.len; i += 2)
			*p++ = (pmsg.cgpn.num[i] & 0x0f) | ((pmsg.cgpn.num[i + 1] & 0x0f) << 4);
		if (pmsg.cgpn.len & 0x1) ;	/* filler correct */
		*p++ = 0;	/* end of optional parameters */
		goto send_isup;
	case SAM:
		printf("                    |<-+---------------------%3ld--SAM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_SAM;
		p = d + 2;
		*d = p - d;
		d++;		/* pointer to subn */
		pmsg.subn.len = strnlen(pmsg.subn.num, 100) + 1;
		*p++ = 1 + ((pmsg.subn.len + 1) >> 1);	/* subn len */
		*p++ = (pmsg.subn.len & 0x1) ? 0x80 : 0x00;
		for (i = 0; i < pmsg.subn.len; i += 2)
			*p++ = (pmsg.subn.num[i] & 0x0f) | ((pmsg.subn.num[i + 1] & 0x0f) << 4);
		if (pmsg.subn.len & 0x1) ;	/* filler correct */
		/* optional parameters */
		*d++ = 0;	/* eop */
		goto send_isup;
	case INR:
		printf("                    |<-+---------------------%3ld--INR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_INR;
		*d++ = pmsg.inri;
		*d++ = 0;	/* eop */
		goto send_isup;
	case INF:
		printf("                    |<-+---------------------%3ld--INF--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_INF;
		*d++ = pmsg.infi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case COT:
		*d++ = ISUP_MT_COT;
		*d++ = pmsg.coti;	/* success = 1, failure = 0 */
		if (pmsg.coti) {
			printf("                    |<-+---------------------%3ld--COT--| (success)          (%d)\n", pmsg.cic, state);
			fflush(stdout);
		} else {
			printf("                    |<-+---------------------%3ld--COT--| (failure)          (%d)\n", pmsg.cic, state);
			fflush(stdout);
		}
		goto send_isup;
	case ACM:
		printf("                    |<-+---------------------%3ld--ACM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_ACM;
		*d++ = pmsg.bci;
		*d++ = pmsg.bci >> 8;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CON:
		printf("                    |<-+---------------------%3ld--CON--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CON;
		*d++ = pmsg.bci;
		*d++ = pmsg.bci >> 8;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FOT:
		printf("                    |<-+---------------------%3ld--FOT--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FOT;
		*d++ = 0;	/* eop */
		goto send_isup;
	case ANM:
		printf("                    |<-+---------------------%3ld--ANM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_ANM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case REL:
		printf("                    |<-+---------------------%3ld--REL--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_REL;
		*d++ = 2;	/* causv pointer */
		*d++ = 0;	/* eop */
		*d++ = pmsg.caus.len;
		for (i = 0; i < pmsg.caus.len; i++)
			*d++ = pmsg.caus.buf[i];
		goto send_isup;
	case SUS:
		printf("                    |<-+---------------------%3ld--SUS--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_SUS;
		*d++ = pmsg.sris;
		*d++ = 0;	/* eop */
		goto send_isup;
	case RES:
		printf("                    |<-+---------------------%3ld--RES--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_RES;
		*d++ = pmsg.sris;
		*d++ = 0;	/* eop */
		goto send_isup;
	case RLC:
		printf("                    |<-+---------------------%3ld--RLC--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_RLC;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CCR:
		printf("                    |<-+---------------------%3ld--CCR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CCR;
		goto send_isup;
	case RSC:
		printf("                    |<-+---------------------%3ld--RSC--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_RSC;
		goto send_isup;
	case BLO:
		printf("                    |<-+---------------------%3ld--BLO--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_BLO;
		goto send_isup;
	case UBL:
		printf("                    |<-+---------------------%3ld--UBL--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_UBL;
		goto send_isup;
	case BLA:
		printf("                    |<-+---------------------%3ld--BLA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_BLA;
		goto send_isup;
	case UBA:
		printf("                    |<-+---------------------%3ld--UBA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_UBA;
		goto send_isup;
	case GRS:
		printf("                    |<-+---------------------%3ld--GRS--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_GRS;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGB:
		printf("                    |<-+---------------------%3ld--CGB--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CGB;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGU:
		printf("                    |<-+---------------------%3ld--CGU--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CGU;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGBA:
		printf("                    |<-+---------------------%3ld--CGBA-|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CGBA;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGUA:
		printf("                    |<-+---------------------%3ld--CGUA-|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CGUA;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CMR:
		printf("                    |<-+---------------------%3ld--CMR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CMR;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CMC:
		printf("                    |<-+---------------------%3ld--CMC--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CMC;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CMRJ:
		printf("                    |<-+---------------------%3ld--CMRJ-|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CMRJ;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAR:
		printf("                    |<-+---------------------%3ld--FAR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FAR;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAA:
		printf("                    |<-+---------------------%3ld--FAA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FAA;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FRJ:
		printf("                    |<-+---------------------%3ld--FRJ--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FRJ;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAD:
		printf("                    |<-+---------------------%3ld--FAD--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FAD;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAI:
		printf("                    |<-+---------------------%3ld--FAI--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FAI;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case LPA:
		printf("                    |<-+---------------------%3ld--LPA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_LPA;
		goto send_isup;
	case DRS:
		printf("                    |<-+---------------------%3ld--DRS--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_DRS;
		*d++ = 0;	/* eop */
		goto send_isup;
	case PAM:
		printf("                    |<-+---------------------%3ld--PAM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_PAM;
		for (i = 0; i < pmsg.pam.len; i++)
			*d++ = pmsg.pam.buf[i];
		goto send_isup;
	case GRA:
		printf("                    |<-+---------------------%3ld--GRA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_GRA;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CQM:
		printf("                    |<-+---------------------%3ld--CQM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CQM;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CQR:
		printf("                    |<-+---------------------%3ld--CQR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CQR;
		*d++ = 2;
		*d++ = 1 + pmsg.rs.len + 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		*d++ = pmsg.csi.len;
		for (i = 0; i < pmsg.csi.len; i++)
			*d++ = pmsg.csi.buf[i];
		goto send_isup;
	case CPG:
		printf("                    |<-+---------------------%3ld--CPG--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CPG;
		*d++ = pmsg.evnt;
		*d++ = 0;	/* eop */
		goto send_isup;
	case USR:
		printf("                    |<-+---------------------%3ld--USR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_USR;
		*d++ = 2;
		*d++ = 0;	/* eop */
		*d++ = pmsg.uui.len;
		for (i = 0; i < pmsg.uui.len; i++)
			*d++ = pmsg.uui.buf[i];
		goto send_isup;
	case UCIC:
		printf("                    |<-+---------------------%3ld--UCIC-|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_UCIC;
		goto send_isup;
	case CFN:
		printf("                    |<-+---------------------%3ld--CFN--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CFN;
		*d++ = 2;
		*d++ = 0;	/* eop */
		*d++ = pmsg.caus.len;
		for (i = 0; i < pmsg.caus.len; i++)
			*d++ = pmsg.caus.buf[i];
		goto send_isup;
	case OLM:
		printf("                    |<-+---------------------%3ld--OLM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_OLM;
		goto send_isup;
	case CRG:
		printf("                    |<-+---------------------%3ld--CRG--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CRG;
		goto send_isup;
	case NRM:
		printf("                    |<-+---------------------%3ld--NRM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_NRM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAC:
		printf("                    |<-+---------------------%3ld--FAC--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_FAC;
		*d++ = 0;	/* eop */
		goto send_isup;
	case UPT:
		printf("                    |<-+---------------------%3ld--UPT--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_UPT;
		*d++ = 0;	/* eop */
		goto send_isup;
	case UPA:
		printf("                    |<-+---------------------%3ld--UPA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_UPA;
		*d++ = 0;	/* eop */
		goto send_isup;
	case IDR:
		printf("                    |<-+---------------------%3ld--IDR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_IDR;
		*d++ = 0;	/* eop */
		goto send_isup;
	case IRS:
		printf("                    |<-+---------------------%3ld--IRS--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_IRS;
		*d++ = 0;	/* eop */
		goto send_isup;
	case SGM:
		printf("                    |<-+---------------------%3ld--SGM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_SGM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CRA:
		printf("                    |<-+---------------------%3ld--CRA--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CRA;
		goto send_isup;
	case CRM:
		printf("                    |<-+---------------------%3ld--CRM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CRM;
		*d++ = pmsg.nci;
		goto send_isup;
	case CVR:
		printf("                    |<-+---------------------%3ld--CVR--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CVR;
		*d++ = pmsg.cvri;
		*d++ = pmsg.cgri;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CVT:
		printf("                    |<-+---------------------%3ld--CVT--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CVT;
		goto send_isup;
	case EXM:
		printf("                    |<-+---------------------%3ld--EXM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_EXM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case NON:
		printf("                    |<-+---------------------%3ld--NON--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_NON;
		*d++ = pmsg.ton;
		*d++ = 0;	/* eop */
		goto send_isup;
	case LLM:
		printf("                    |<-+---------------------%3ld--LLM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_LLM;
		goto send_isup;
	case CAK:
		printf("                    |<-+---------------------%3ld--CAK--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_CAK;
		goto send_isup;
	case TCM:
		printf("                    |<-+---------------------%3ld--TCM--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_TCM;
		*d++ = pmsg.cri;
		goto send_isup;
	case MCP:
		printf("                    |<-+---------------------%3ld--MCP--|                    (%d)\n", pmsg.cic, state);
		fflush(stdout);
		*d++ = ISUP_MT_MCP;
		goto send_isup;
	case PAUSE:
		printf("                    |<-+--------------------------TFP--|                    (%d)\n", state);
		fflush(stdout);
		m->pause_ind.mtp_primitive = MTP_PAUSE_IND;
		m->pause_ind.mtp_addr_offset = sizeof(m->pause_ind);
		m->pause_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->pause_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->pause_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case RESUME:
		printf("                    |<-+--------------------------TFA--|                    (%d)\n", state);
		fflush(stdout);
		m->resume_ind.mtp_primitive = MTP_RESUME_IND;
		m->resume_ind.mtp_addr_offset = sizeof(m->resume_ind);
		m->resume_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->resume_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->resume_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case RESTART_COMPLETE:
		printf("                    |<-+--------------------------TRA--|                    (%d)\n", state);
		fflush(stdout);
		m->pause_ind.mtp_primitive = MTP_RESTART_COMPLETE_IND;
		ctrl.len = sizeof(m->restart_complete_ind);
		data.len = 0;
		goto send_mtp;
	case USER_PART_UNKNOWN:
		printf("                    |<-+--------------------------UPU--| (unknown)          (%d)\n", state);
		fflush(stdout);
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		m->status_ind.mtp_primitive = MTP_STATUS_IND;
		m->status_ind.mtp_type = MTP_STATUS_TYPE_UPU;
		m->status_ind.mtp_status = MTP_STATUS_UPU_UNKNOWN;
		m->status_ind.mtp_addr_offset = sizeof(m->status_ind);
		m->status_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->status_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case USER_PART_UNEQUIPPED:
		printf("                    |<-+--------------------------UPU--| (unequipped)       (%d)\n", state);
		fflush(stdout);
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		m->status_ind.mtp_primitive = MTP_STATUS_IND;
		m->status_ind.mtp_type = MTP_STATUS_TYPE_UPU;
		m->status_ind.mtp_status = MTP_STATUS_UPU_UNEQUIPPED;
		m->status_ind.mtp_addr_offset = sizeof(m->status_ind);
		m->status_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->status_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case USER_PART_UNAVAILABLE:
		printf("                    |<-+--------------------------UPU--| (unavailable)      (%d)\n", state);
		fflush(stdout);
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		m->status_ind.mtp_primitive = MTP_STATUS_IND;
		m->status_ind.mtp_type = MTP_STATUS_TYPE_UPU;
		m->status_ind.mtp_status = MTP_STATUS_UPU_INACCESSIBLE;
		m->status_ind.mtp_addr_offset = sizeof(m->status_ind);
		m->status_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->status_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case CONGESTION:
		printf("                    |<-+--------------------------TFC--|                    (%d)\n", state);
		fflush(stdout);
		m->status_ind.mtp_primitive = MTP_STATUS_IND;
		m->status_ind.mtp_type = MTP_STATUS_TYPE_CONG;
		m->status_ind.mtp_status = MTP_STATUS_CONGESTION;
		m->status_ind.mtp_addr_offset = sizeof(m->status_ind);
		m->status_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->status_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->status_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case IBI:
		printf("<<<<<<<<<<<<<<<<<<<<|<<|<<< IN-BAND INFORMATION <<<<<<<|<<<<<<<<<<<<<<<<<<<<(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case RINGING:
		printf("<<<<<<<<<<<<<<<<<<<<|<<|<<<<<<<<< RINGING <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<<<(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case COMMUNICATION:
		printf("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case TONE:
		printf("^<^<^<^<^<^<^<^<^<^<|<^|^<^<^< CONTINUITY TONE <^<^<^<^|^<^<^<^<^<^<^<^<^<^<(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case LOOPBACK:
		printf("                    |__|_______________________________|_                   \n");
		printf("                    |<<|_______________________________|_| LOOPBACK         (%d)\n", state);
		printf("                    |  |                               |                    \n");
		fflush(stdout);
		return (__RESULT_SUCCESS);
	default:
		printf("                    |<-+--------------------------\?\?\?--|                    (%d)\n", state);
		fflush(stdout);
		return (__RESULT_FAILURE);
	}
      send_isup:
	data.len = (d > p) ? (d - pt_dat) : (p - pt_dat);
	if (show_msg) {
		printf("Sent msg: ");
		for (i = 0; i < data.len; i++)
			printf("%02X ", (unsigned char) data.buf[i]);
		printf("\n");
		fflush(stdout);
	}
	ctrl.len = sizeof(m->transfer_ind) + sizeof(rem_addr);
	m->transfer_ind.mtp_primitive = MTP_TRANSFER_IND;
	m->transfer_ind.mtp_srce_offset = sizeof(m->transfer_ind);
	m->transfer_ind.mtp_srce_length = sizeof(rem_addr);
	m->transfer_ind.mtp_mp = 2;
	m->transfer_ind.mtp_sls = pmsg.cic & 0x0f;
	*(mtp_addr_t *) (&m->transfer_ind + 1) = rem_addr;
      send_mtp:
	ctrl.maxlen = BUFSIZE;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.buf = (char *) pt_dat;
	if (putmsg(pt_fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, 0) < 0) {
		if (errno == EAGAIN || errno == EINTR)
			return __RESULT_FAILURE;
		printf("                    |<-+--------------------******** ERROR: putmsg failed                                         \n");
		printf("                                                          : %-13s:%40s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}
	return time_event(ret);
}

static int
iut_cpc_signal(int action)
{
	int ret;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union CC_primitives *m = (union CC_primitives *) cbuf;
	unsigned char *c;

	ctrl.maxlen = BUFSIZE;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.buf = dbuf;
	switch (action) {
	case INFO_REQ:
		if (verbose) {
			printf("---INFO------------>|  |                               |                    (%d)\n", state);
			fflush(stdout);
		}
		m->cc_primitive = CC_INFO_REQ;
		ctrl.len = sizeof(m->info_req);
		data.len = 0;
		goto signal_msg;
	case OPTMGMT_REQ:
		if (verbose) {
			printf("---OPTMGMT--------->|  |                               |                    (%d)\n", state);
			fflush(stdout);
		}
		m->cc_primitive = CC_OPTMGMT_REQ;
		m->optmgmt_req.cc_opt_length = cprim.opt.len;
		m->optmgmt_req.cc_opt_offset = sizeof(m->optmgmt_req);
		m->optmgmt_req.cc_opt_flags = cprim.opt_flags;
		c = (unsigned char *) (&m->optmgmt_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->optmgmt_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case BIND_REQ:
		if (verbose > 1) {
			printf("---BIND------------>|  |                               |                    (%d)\n", state);
			fflush(stdout);
			if (verbose > 2) {
				if (cprim.addr.len) {
					printf(" addr scope =    %2s |  |                               |                    \n", scope_string(cprim.addr.add.scope));
					printf(" addr id    = %5ld |  |                               |                    \n", cprim.addr.add.id);
					printf(" addr cic   = %5ld |  |                               |                    \n", cprim.addr.add.cic);
				}
				printf(" setup inds = %5ld |  |                               |                    \n", cprim.setup_ind);
				printf(" flags      =  %4lx |  |                               |                    \n", cprim.bind_flags);
				fflush(stdout);
			}
		}
		m->cc_primitive = CC_BIND_REQ;
		m->bind_req.cc_addr_length = cprim.addr.len;
		m->bind_req.cc_addr_offset = cprim.addr.len ? sizeof(m->bind_req) : 0;
		m->bind_req.cc_setup_ind = cprim.setup_ind;
		m->bind_req.cc_bind_flags = cprim.bind_flags;
		if (cprim.addr.len)
			*(isup_addr_t *) (&m->bind_req + 1) = cprim.addr.add;
		ctrl.len = sizeof(m->bind_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBIND_REQ:
		if (verbose > 1)
			printf("---UNBIND---------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBIND_REQ;
		ctrl.len = sizeof(m->unbind_req);
		data.len = 0;
		goto signal_msg;
	case ADDR_REQ:
		if (verbose) {
			printf("---ADDR------------>|  |                               |                    (%d)\n", state);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_ADDR_REQ;
		m->addr_req.cc_call_ref = cprim.call_ref;
		ctrl.len = sizeof(m->addr_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_REQ:
		printf("---SETUP----------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_REQ;
		m->setup_req.cc_user_ref = cprim.user_ref;
		m->setup_req.cc_call_type = cprim.call_type;
		m->setup_req.cc_call_flags = cprim.call_flags;
		m->setup_req.cc_cdpn_length = cprim.cdpn.len;
		m->setup_req.cc_cdpn_offset = cprim.cdpn.len ? sizeof(m->setup_req) : 0;
		m->setup_req.cc_opt_length = cprim.opt.len;
		m->setup_req.cc_opt_offset = cprim.opt.len ? sizeof(m->setup_req) + cprim.cdpn.len : 0;
		m->setup_req.cc_addr_length = cprim.addr.len;
		m->setup_req.cc_addr_offset = cprim.addr.len ? sizeof(m->setup_req) + cprim.cdpn.len + cprim.opt.len : 0;
		c = (unsigned char *) (&m->setup_req + 1);
		bcopy(cprim.cdpn.buf, c, cprim.cdpn.len);
		c += cprim.cdpn.len;
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		c += cprim.addr.len;
		ctrl.len = sizeof(m->setup_req) + cprim.cdpn.len + cprim.opt.len + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case MORE_INFO_REQ:
		printf("---MORE-INFO------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_MORE_INFO_REQ;
		m->more_info_req.cc_call_ref = cprim.call_ref;
		m->more_info_req.cc_opt_length = cprim.opt.len;
		m->more_info_req.cc_opt_offset = cprim.opt.len ? sizeof(m->more_info_req) : 0;
		c = (unsigned char *) (&m->more_info_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->more_info_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case INFORMATION_REQ:
		printf("---INFORMATION----->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_INFORMATION_REQ;
		m->information_req.cc_user_ref = cprim.user_ref;
		m->information_req.cc_subn_length = cprim.subn.len;
		m->information_req.cc_subn_offset = cprim.subn.len ? sizeof(m->information_req) : 0;
		m->information_req.cc_opt_length = cprim.opt.len;
		m->information_req.cc_opt_offset = cprim.opt.len ? sizeof(m->information_req) + cprim.subn.len : 0;
		c = (unsigned char *) (&m->information_req + 1);
		bcopy(cprim.subn.buf, c, cprim.subn.len);
		c += cprim.subn.len;
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->information_req) + cprim.subn.len + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONT_CHECK_REQ:
		printf("---CONT CHECK------>|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_CONT_CHECK_REQ;
		m->cont_check_req.cc_addr_length = cprim.addr.len;
		m->cont_check_req.cc_addr_offset = sizeof(m->cont_check_req);
		c = (unsigned char *) (&m->cont_check_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->cont_check_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case CONT_TEST_REQ:
		printf("---CONT-TEST------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_CONT_TEST_REQ;
		m->cont_test_req.cc_token_value = cprim.token_value;
		m->cont_test_req.cc_call_ref = cprim.call_ref;
		ctrl.len = sizeof(m->cont_test_req);
		data.len = 0;
		goto signal_msg;
	case CONT_REPORT_REQ:
		printf("---CONT REPORT----->|  |                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_CONT_REPORT_REQ;
		m->cont_report_req.cc_user_ref = cprim.user_ref;
		m->cont_report_req.cc_call_ref = cprim.call_ref;
		m->cont_report_req.cc_result = cprim.result;
		ctrl.len = sizeof(m->cont_report_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_RES:
		printf("---SETUP OK-------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_RES;
		m->setup_res.cc_call_ref = cprim.call_ref;
		m->setup_res.cc_token_value = cprim.token_value;
		ctrl.len = sizeof(m->setup_res);
		data.len = 0;
		goto signal_msg;
	case PROCEEDING_REQ:
		printf("---PROCEEDING------>|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_PROCEEDING_REQ;
		m->proceeding_req.cc_call_ref = cprim.call_ref;
		m->proceeding_req.cc_flags = cprim.flags;;
		m->proceeding_req.cc_opt_length = cprim.opt.len;
		m->proceeding_req.cc_opt_offset = cprim.opt.len ? sizeof(m->proceeding_req) : 0;
		c = (unsigned char *) (&m->proceeding_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->proceeding_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case ALERTING_REQ:
		printf("---ALERTING-------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_ALERTING_REQ;
		m->alerting_req.cc_call_ref = cprim.call_ref;
		m->alerting_req.cc_flags = cprim.flags;;
		m->alerting_req.cc_opt_length = cprim.opt.len;
		m->alerting_req.cc_opt_offset = cprim.opt.len ? sizeof(m->alerting_req) : 0;
		c = (unsigned char *) (&m->alerting_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->alerting_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case PROGRESS_REQ:
		printf("---PROGRESS-------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_PROGRESS_REQ;
		m->progress_req.cc_call_ref = cprim.call_ref;
		m->progress_req.cc_event = cprim.event;;
		m->progress_req.cc_flags = cprim.flags;;
		m->progress_req.cc_opt_length = cprim.opt.len;
		m->progress_req.cc_opt_offset = cprim.opt.len ? sizeof(m->progress_req) : 0;
		c = (unsigned char *) (&m->progress_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->progress_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case IBI_REQ:
		printf("---IBI------------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_IBI_REQ;
		m->ibi_req.cc_call_ref = cprim.call_ref;
		m->ibi_req.cc_flags = cprim.flags;
		m->ibi_req.cc_opt_length = cprim.opt.len;
		m->ibi_req.cc_opt_offset = cprim.opt.len ? sizeof(m->ibi_req) : 0;
		c = (unsigned char *) (&m->ibi_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->ibi_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONNECT_REQ:
		printf("---CONNECT--------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_CONNECT_REQ;
		m->connect_req.cc_call_ref = cprim.call_ref;
		m->connect_req.cc_flags = cprim.flags;;
		m->connect_req.cc_opt_length = cprim.opt.len;
		m->connect_req.cc_opt_offset = cprim.opt.len ? sizeof(m->connect_req) : 0;
		c = (unsigned char *) (&m->connect_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->connect_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SETUP_COMPLETE_REQ:
		printf("---SETUP-COMPLETE-->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_COMPLETE_REQ;
		m->setup_complete_req.cc_call_ref = cprim.call_ref;
		m->setup_complete_req.cc_opt_length = cprim.opt.len;
		m->setup_complete_req.cc_opt_offset = cprim.opt.len ? sizeof(m->setup_complete_req) : 0;
		c = (unsigned char *) (&m->setup_complete_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->setup_complete_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case FORWXFER_REQ:
		printf("---FORWXFER-------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_FORWXFER_REQ;
		m->forwxfer_req.cc_call_ref = cprim.call_ref;
		m->forwxfer_req.cc_opt_length = cprim.opt.len;
		m->forwxfer_req.cc_opt_offset = cprim.opt.len ? sizeof(m->forwxfer_req) : 0;
		c = (unsigned char *) (&m->forwxfer_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->forwxfer_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_REQ:
		printf("---SUSPEND--------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_REQ;
		m->suspend_req.cc_call_ref = cprim.call_ref;
		m->suspend_req.cc_flags = cprim.flags;;
		m->suspend_req.cc_opt_length = cprim.opt.len;
		m->suspend_req.cc_opt_offset = cprim.opt.len ? sizeof(m->suspend_req) : 0;
		c = (unsigned char *) (&m->suspend_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->suspend_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_RES:
		printf("---SUSPEND-OK------>|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_RES;
		m->suspend_res.cc_call_ref = cprim.call_ref;
		m->suspend_res.cc_opt_length = cprim.opt.len;
		m->suspend_res.cc_opt_offset = cprim.opt.len ? sizeof(m->suspend_res) : 0;
		c = (unsigned char *) (&m->suspend_res + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->suspend_res) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_REJECT_REQ:
		printf("---SUSPEND-REJECT-->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_REJECT_REQ;
		m->suspend_reject_req.cc_call_ref = cprim.call_ref;
		m->suspend_reject_req.cc_cause = cprim.cause;
		m->suspend_reject_req.cc_opt_length = cprim.opt.len;
		m->suspend_reject_req.cc_opt_offset = cprim.opt.len ? sizeof(m->suspend_reject_req) : 0;
		c = (unsigned char *) (&m->suspend_reject_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->suspend_reject_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REQ:
		printf("---RESUME---------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_REQ;
		m->resume_req.cc_call_ref = cprim.call_ref;
		m->resume_req.cc_flags = cprim.flags;;
		m->resume_req.cc_opt_length = cprim.opt.len;
		m->resume_req.cc_opt_offset = cprim.opt.len ? sizeof(m->resume_req) : 0;
		c = (unsigned char *) (&m->resume_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->resume_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_RES:
		printf("---RESUME-OK------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_RES;
		m->resume_res.cc_call_ref = cprim.call_ref;
		m->resume_res.cc_opt_length = cprim.opt.len;
		m->resume_res.cc_opt_offset = cprim.opt.len ? sizeof(m->resume_res) : 0;
		c = (unsigned char *) (&m->resume_res + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->resume_res) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REJECT_REQ:
		printf("---RESUME-REJECT--->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_REJECT_REQ;
		m->resume_reject_req.cc_call_ref = cprim.call_ref;
		m->resume_reject_req.cc_cause = cprim.cause;
		m->resume_reject_req.cc_opt_length = cprim.opt.len;
		m->resume_reject_req.cc_opt_offset = cprim.opt.len ? sizeof(m->resume_reject_req) : 0;
		c = (unsigned char *) (&m->resume_reject_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->resume_reject_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case REJECT_REQ:
		printf("---REJECT---------->|  |                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_REJECT_REQ;
		m->reject_req.cc_call_ref = cprim.call_ref;
		m->reject_req.cc_cause = cprim.cause;
		m->reject_req.cc_opt_length = cprim.opt.len;
		m->reject_req.cc_opt_offset = cprim.opt.len ? sizeof(m->reject_req) : 0;
		c = (unsigned char *) (&m->reject_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->reject_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RELEASE_REQ:
		printf("---RELEASE--------->|  |                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_RELEASE_REQ;
		m->release_req.cc_user_ref = cprim.user_ref;
		m->release_req.cc_call_ref = cprim.call_ref;
		m->release_req.cc_cause = cprim.cause;;
		m->release_req.cc_opt_length = cprim.opt.len;
		m->release_req.cc_opt_offset = cprim.opt.len ? sizeof(m->release_req) : 0;
		c = (unsigned char *) (&m->release_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->release_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RELEASE_RES:
		printf("---RELEASE OK------>|  |                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_RELEASE_RES;
		m->release_res.cc_user_ref = cprim.user_ref;
		m->release_res.cc_call_ref = cprim.call_ref;
		m->release_res.cc_opt_length = cprim.opt.len;
		m->release_res.cc_opt_offset = cprim.opt.len ? sizeof(m->release_res) : 0;
		c = (unsigned char *) (&m->release_res + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->release_res) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESET_REQ:
		printf("---RESET----------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_RESET_REQ;
		m->reset_req.cc_flags = cprim.flags;
		m->reset_req.cc_addr_length = cprim.addr.len;
		m->reset_req.cc_addr_offset = cprim.addr.len ? sizeof(m->reset_req) : 0;
		c = (unsigned char *) (&m->reset_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		c += cprim.addr.len;
		ctrl.len = sizeof(m->reset_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case RESET_RES:
		printf("---RESET OK-------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_RESET_RES;
		m->reset_res.cc_flags = cprim.flags;
		m->reset_res.cc_addr_length = cprim.addr.len;
		m->reset_res.cc_addr_offset = sizeof(m->reset_res);
		c = (unsigned char *) (&m->reset_res + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->reset_res) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case BLOCKING_REQ:
		printf("---BLOCKING-------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_BLOCKING_REQ;
		m->blocking_req.cc_flags = cprim.flags;
		m->blocking_req.cc_addr_length = cprim.addr.len;
		m->blocking_req.cc_addr_offset = sizeof(m->blocking_req);
		c = (unsigned char *) (&m->blocking_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->blocking_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case BLOCKING_RES:
		printf("---BLOCKING-OK----->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_BLOCKING_RES;
		m->blocking_res.cc_flags = cprim.flags;
		m->blocking_res.cc_addr_length = cprim.addr.len;
		m->blocking_res.cc_addr_offset = sizeof(m->blocking_res);
		c = (unsigned char *) (&m->blocking_res + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->blocking_res) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBLOCKING_REQ:
		printf("---UNBLOCKING------>|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBLOCKING_REQ;
		m->unblocking_req.cc_flags = cprim.flags;
		m->unblocking_req.cc_addr_length = cprim.addr.len;
		m->unblocking_req.cc_addr_offset = sizeof(m->unblocking_req);
		c = (unsigned char *) (&m->unblocking_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->unblocking_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBLOCKING_RES:
		printf("---UNBLOCKING-OK--->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBLOCKING_RES;
		m->unblocking_res.cc_flags = cprim.flags;
		m->unblocking_res.cc_addr_length = cprim.addr.len;
		m->unblocking_res.cc_addr_offset = sizeof(m->unblocking_res);
		c = (unsigned char *) (&m->unblocking_res + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->unblocking_res) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case QUERY_REQ:
		printf("---QUERY----------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_QUERY_REQ;
		m->query_req.cc_flags = cprim.flags;
		m->query_req.cc_addr_length = cprim.addr.len;
		m->query_req.cc_addr_offset = sizeof(m->query_req);
		c = (unsigned char *) (&m->query_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->query_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case QUERY_RES:
		printf("---QUERY OK-------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_QUERY_RES;
		m->query_res.cc_flags = cprim.flags;
		m->query_res.cc_addr_length = cprim.addr.len;
		m->query_res.cc_addr_offset = sizeof(m->query_res);
		c = (unsigned char *) (&m->query_res + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->query_res) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case IBI:
		printf(">>>>>>>>>>>>>>>>>>>>|>>|>>> IN-BAND INFORMATION >>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case RINGING:
		printf(">>>>>>>>>>>>>>>>>>>>|>>|>>>>>>>>> RINGING >>>>>>>>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case COMMUNICATION:
		printf("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case TONE:
		printf("^>^>^>^>^>^>^>^>^>^>|>^|^>^>^> CONTINUITY TONE >^>^>^>^|^>^>^>^>^>^>^>^>^>^>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case LOOPBACK:
		printf("                   _|__|_______________________________|                    \n");
		printf("         LOOPBACK |_|__|_____________________________>>|                    (%d)\n", state);
		printf("                    |  |                               |                    \n");
		fflush(stdout);
		return (__RESULT_SUCCESS);
	default:
		printf("\?\?\?\?--------------->|  |                               |                    (%d)\n", state);
		fflush(stdout);
		return __RESULT_FAILURE;
	}
      signal_msg:
	if ((ret = putmsg(iut_cpc_fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, RS_HIPRI)) < 0) {
		printf("************** ERROR: putmsg failed                                         \n");
		printf("                    : %-13s:%40s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return (__RESULT_FAILURE);
	}
	return time_event(__RESULT_SUCCESS);
}

static int
iut_signal(int action, int fd)
{
	int ret;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union CC_primitives *m = (union CC_primitives *) cbuf;
	unsigned char *c;

	ctrl.maxlen = BUFSIZE;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.buf = dbuf;
	switch (action) {
	case INFO_REQ:
		if (verbose)
			printf("---INFO-------------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_INFO_REQ;
		ctrl.len = sizeof(m->info_req);
		data.len = 0;
		goto signal_msg;
	case OPTMGMT_REQ:
		if (verbose)
			printf("---OPTMGMT----------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_OPTMGMT_REQ;
		m->optmgmt_req.cc_opt_length = mprim.opt.len;
		m->optmgmt_req.cc_opt_offset = mprim.opt.len ? sizeof(m->optmgmt_req) : 0;
		m->optmgmt_req.cc_opt_flags = mprim.opt_flags;
		c = (unsigned char *) (&m->optmgmt_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->optmgmt_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case BIND_REQ:
		if (verbose > 1) {
			printf("---BIND-------------+->|                               |                    (%d)\n", state);
			fflush(stdout);
			if (verbose > 2) {
				if (mprim.addr.len) {
					printf(" addr scope =    %2s |  |                               |                    \n", scope_string(mprim.addr.add.scope));
					printf(" addr id    = %5ld |  |                               |                    \n", mprim.addr.add.id);
					printf(" addr cic   = %5ld |  |                               |                    \n", mprim.addr.add.cic);
				}
				printf(" setup inds = %5ld |  |                               |                    \n", mprim.setup_ind);
				printf(" flags      =  %4lx |  |                               |                    \n", mprim.bind_flags);
				fflush(stdout);
			}
		}
		m->cc_primitive = CC_BIND_REQ;
		m->bind_req.cc_addr_length = mprim.addr.len;
		m->bind_req.cc_addr_offset = mprim.addr.len ? sizeof(m->bind_req) : 0;
		m->bind_req.cc_setup_ind = mprim.setup_ind;
		m->bind_req.cc_bind_flags = mprim.bind_flags;
		if (mprim.addr.len)
			*(isup_addr_t *) (&m->bind_req + 1) = mprim.addr.add;
		ctrl.len = sizeof(m->bind_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBIND_REQ:
		if (verbose > 1)
			printf("---UNBIND-----------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBIND_REQ;
		ctrl.len = sizeof(m->unbind_req);
		data.len = 0;
		goto signal_msg;
	case ADDR_REQ:
		if (verbose)
			printf("---ADDR-------------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_ADDR_REQ;
		ctrl.len = sizeof(m->addr_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_REQ:
		printf("---SETUP------------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_REQ;
		m->setup_req.cc_user_ref = mprim.user_ref;
		m->setup_req.cc_call_type = mprim.call_type;
		m->setup_req.cc_call_flags = mprim.call_flags;
		m->setup_req.cc_cdpn_length = mprim.cdpn.len;
		m->setup_req.cc_cdpn_offset = mprim.cdpn.len ? sizeof(m->setup_req) : 0;
		m->setup_req.cc_opt_length = mprim.opt.len;
		m->setup_req.cc_opt_offset = mprim.opt.len ? sizeof(m->setup_req) + mprim.cdpn.len : 0;
		m->setup_req.cc_addr_length = mprim.addr.len;
		m->setup_req.cc_addr_offset = mprim.addr.len ? sizeof(m->setup_req) + mprim.cdpn.len + mprim.opt.len : 0;
		c = (unsigned char *) (&m->setup_req + 1);
		bcopy(mprim.cdpn.buf, c, mprim.cdpn.len);
		c += mprim.cdpn.len;
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		c += mprim.addr.len;
		ctrl.len = sizeof(m->setup_req) + mprim.cdpn.len + mprim.opt.len + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case MORE_INFO_REQ:
		printf("---MORE-INFO--------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_MORE_INFO_REQ;
		m->more_info_req.cc_call_ref = mprim.call_ref;
		m->more_info_req.cc_opt_length = mprim.opt.len;
		m->more_info_req.cc_opt_offset = mprim.opt.len ? sizeof(m->more_info_req) : 0;
		c = (unsigned char *) (&m->more_info_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->more_info_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case INFORMATION_REQ:
		printf("---INFORMATION------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_INFORMATION_REQ;
		m->information_req.cc_user_ref = mprim.user_ref;
		m->information_req.cc_subn_length = mprim.subn.len;
		m->information_req.cc_subn_offset = mprim.subn.len ? sizeof(m->information_req) : 0;
		m->information_req.cc_opt_length = mprim.opt.len;
		m->information_req.cc_opt_offset = mprim.opt.len ? sizeof(m->information_req) + mprim.subn.len : 0;
		c = (unsigned char *) (&m->information_req + 1);
		bcopy(mprim.subn.buf, c, mprim.subn.len);
		c += mprim.subn.len;
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->information_req) + mprim.subn.len + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONT_CHECK_REQ:
		printf("---CONT CHECK-------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_CONT_CHECK_REQ;
		m->cont_check_req.cc_addr_length = mprim.addr.len;
		m->cont_check_req.cc_addr_offset = sizeof(m->cont_check_req);
		c = (unsigned char *) (&m->cont_check_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->cont_check_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case CONT_TEST_REQ:
		printf("---CONT-TEST--------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_CONT_TEST_REQ;
		m->cont_test_req.cc_call_ref = mprim.call_ref;
		m->cont_test_req.cc_token_value = mprim.token_value;
		ctrl.len = sizeof(m->cont_test_req);
		data.len = 0;
		goto signal_msg;
	case CONT_REPORT_REQ:
		printf("---CONT REPORT------+->|                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_CONT_REPORT_REQ;
		m->cont_report_req.cc_user_ref = mprim.user_ref;
		m->cont_report_req.cc_call_ref = mprim.call_ref;
		m->cont_report_req.cc_result = mprim.result;
		ctrl.len = sizeof(m->cont_report_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_RES:
		printf("---SETUP OK---------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_RES;
		m->setup_res.cc_call_ref = mprim.call_ref;
		m->setup_res.cc_token_value = mprim.token_value;
		ctrl.len = sizeof(m->setup_res);
		data.len = 0;
		goto signal_msg;
	case PROCEEDING_REQ:
		printf("---PROCEEDING-------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_PROCEEDING_REQ;
		m->proceeding_req.cc_call_ref = mprim.call_ref;
		m->proceeding_req.cc_flags = mprim.flags;;
		m->proceeding_req.cc_opt_length = mprim.opt.len;
		m->proceeding_req.cc_opt_offset = mprim.opt.len ? sizeof(m->proceeding_req) : 0;
		c = (unsigned char *) (&m->proceeding_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->proceeding_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case ALERTING_REQ:
		printf("---ALERTING---------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_ALERTING_REQ;
		m->alerting_req.cc_call_ref = mprim.call_ref;
		m->alerting_req.cc_flags = mprim.flags;;
		m->alerting_req.cc_opt_length = mprim.opt.len;
		m->alerting_req.cc_opt_offset = mprim.opt.len ? sizeof(m->alerting_req) : 0;
		c = (unsigned char *) (&m->alerting_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->alerting_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case PROGRESS_REQ:
		printf("---PROGRESS---------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_PROGRESS_REQ;
		m->progress_req.cc_call_ref = mprim.call_ref;
		m->progress_req.cc_event = mprim.event;;
		m->progress_req.cc_flags = mprim.flags;;
		m->progress_req.cc_opt_length = mprim.opt.len;
		m->progress_req.cc_opt_offset = mprim.opt.len ? sizeof(m->progress_req) : 0;
		c = (unsigned char *) (&m->progress_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->progress_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case IBI_REQ:
		printf("---IBI--------------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_IBI_REQ;
		m->ibi_req.cc_call_ref = mprim.call_ref;
		m->ibi_req.cc_call_ref = mprim.call_ref;
		m->ibi_req.cc_flags = mprim.flags;
		m->ibi_req.cc_opt_length = mprim.opt.len;
		m->ibi_req.cc_opt_offset = mprim.opt.len ? sizeof(m->ibi_req) : 0;
		c = (unsigned char *) (&m->ibi_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->ibi_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONNECT_REQ:
		printf("---CONNECT----------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_CONNECT_REQ;
		m->connect_req.cc_call_ref = mprim.call_ref;
		m->connect_req.cc_flags = mprim.flags;;
		m->connect_req.cc_opt_length = mprim.opt.len;
		m->connect_req.cc_opt_offset = mprim.opt.len ? sizeof(m->connect_req) : 0;
		c = (unsigned char *) (&m->connect_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->connect_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SETUP_COMPLETE_REQ:
		printf("---SETUP-COMPLETE---+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SETUP_COMPLETE_REQ;
		m->setup_complete_req.cc_call_ref = mprim.call_ref;
		m->setup_complete_req.cc_opt_length = mprim.opt.len;
		m->setup_complete_req.cc_opt_offset = mprim.opt.len ? sizeof(m->setup_complete_req) : 0;
		c = (unsigned char *) (&m->setup_complete_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->setup_complete_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case FORWXFER_REQ:
		printf("---FORWXFER---------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_FORWXFER_REQ;
		m->forwxfer_req.cc_call_ref = mprim.call_ref;
		m->forwxfer_req.cc_opt_length = mprim.opt.len;
		m->forwxfer_req.cc_opt_offset = mprim.opt.len ? sizeof(m->forwxfer_req) : 0;
		c = (unsigned char *) (&m->forwxfer_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->forwxfer_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_REQ:
		printf("---SUSPEND----------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_REQ;
		m->suspend_req.cc_call_ref = mprim.call_ref;
		m->suspend_req.cc_flags = mprim.flags;;
		m->suspend_req.cc_opt_length = mprim.opt.len;
		m->suspend_req.cc_opt_offset = mprim.opt.len ? sizeof(m->suspend_req) : 0;
		c = (unsigned char *) (&m->suspend_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->suspend_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_RES:
		printf("---SUSPEND-OK-------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_RES;
		m->suspend_res.cc_call_ref = mprim.call_ref;
		m->suspend_res.cc_opt_length = mprim.opt.len;
		m->suspend_res.cc_opt_offset = mprim.opt.len ? sizeof(m->suspend_res) : 0;
		c = (unsigned char *) (&m->suspend_res + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->suspend_res) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case SUSPEND_REJECT_REQ:
		printf("---SUSPEND-REJECT---+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_SUSPEND_REJECT_REQ;
		m->suspend_reject_req.cc_call_ref = mprim.call_ref;
		m->suspend_reject_req.cc_cause = mprim.cause;
		m->suspend_reject_req.cc_opt_length = mprim.opt.len;
		m->suspend_reject_req.cc_opt_offset = mprim.opt.len ? sizeof(m->suspend_reject_req) : 0;
		c = (unsigned char *) (&m->suspend_reject_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->suspend_reject_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REQ:
		printf("---RESUME-----------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_REQ;
		m->resume_req.cc_call_ref = mprim.call_ref;
		m->resume_req.cc_flags = mprim.flags;;
		m->resume_req.cc_opt_length = mprim.opt.len;
		m->resume_req.cc_opt_offset = mprim.opt.len ? sizeof(m->resume_req) : 0;
		c = (unsigned char *) (&m->resume_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->resume_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_RES:
		printf("---RESUME-OK--------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_RES;
		m->resume_res.cc_call_ref = mprim.call_ref;
		m->resume_res.cc_opt_length = mprim.opt.len;
		m->resume_res.cc_opt_offset = mprim.opt.len ? sizeof(m->resume_res) : 0;
		c = (unsigned char *) (&m->resume_res + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->resume_res) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REJECT_REQ:
		printf("---RESUME-REJECT----+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_RESUME_REJECT_REQ;
		m->resume_reject_req.cc_call_ref = mprim.call_ref;
		m->resume_reject_req.cc_cause = mprim.cause;
		m->resume_reject_req.cc_opt_length = mprim.opt.len;
		m->resume_reject_req.cc_opt_offset = mprim.opt.len ? sizeof(m->resume_reject_req) : 0;
		c = (unsigned char *) (&m->resume_reject_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->resume_reject_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RELEASE_REQ:
		printf("---RELEASE----------+->|                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_RELEASE_REQ;
		m->release_req.cc_user_ref = mprim.user_ref;
		m->release_req.cc_call_ref = mprim.call_ref;
		m->release_req.cc_cause = mprim.cause;;
		m->release_req.cc_opt_length = mprim.opt.len;
		m->release_req.cc_opt_offset = mprim.opt.len ? sizeof(m->release_req) : 0;
		c = (unsigned char *) (&m->release_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->release_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case REJECT_REQ:
		printf("---REJECT-----------+->|                               |                    (%d)\n", state);
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		fflush(stdout);
		m->cc_primitive = CC_REJECT_REQ;
		m->reject_req.cc_call_ref = mprim.call_ref;
		m->reject_req.cc_cause = mprim.cause;
		m->reject_req.cc_opt_length = mprim.opt.len;
		m->reject_req.cc_opt_offset = mprim.opt.len ? sizeof(m->reject_req) : 0;
		c = (unsigned char *) (&m->reject_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->reject_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RELEASE_RES:
		printf("---RELEASE OK-------+->|                               |                    (%d)\n", state);
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		fflush(stdout);
		m->cc_primitive = CC_RELEASE_RES;
		m->release_res.cc_user_ref = mprim.user_ref;
		m->release_res.cc_call_ref = mprim.call_ref;
		m->release_res.cc_opt_length = mprim.opt.len;
		m->release_res.cc_opt_offset = mprim.opt.len ? sizeof(m->release_res) : 0;
		c = (unsigned char *) (&m->release_res + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->release_res) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESET_REQ:
		printf("---RESET------------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_RESET_REQ;
		m->reset_req.cc_flags = mprim.flags;
		m->reset_req.cc_addr_length = mprim.addr.len;
		m->reset_req.cc_addr_offset = mprim.addr.len ? sizeof(m->reset_req) : 0;
		c = (unsigned char *) (&m->reset_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		c += mprim.addr.len;
		ctrl.len = sizeof(m->reset_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case RESET_RES:
		printf("---RESET OK---------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_RESET_RES;
		m->reset_res.cc_flags = mprim.flags;
		m->reset_res.cc_addr_length = mprim.addr.len;
		m->reset_res.cc_addr_offset = sizeof(m->reset_res);
		c = (unsigned char *) (&m->reset_res + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->reset_res) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case BLOCKING_REQ:
		printf("---BLOCKING---------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_BLOCKING_REQ;
		m->blocking_req.cc_flags = mprim.flags;
		m->blocking_req.cc_addr_length = mprim.addr.len;
		m->blocking_req.cc_addr_offset = mprim.addr.len ? sizeof(m->blocking_req) : 0;
		c = (unsigned char *) (&m->blocking_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->blocking_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case BLOCKING_RES:
		printf("---BLOCKING-OK------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_BLOCKING_RES;
		m->blocking_res.cc_flags = mprim.flags;
		m->blocking_res.cc_addr_length = mprim.addr.len;
		m->blocking_res.cc_addr_offset = sizeof(m->blocking_res);
		c = (unsigned char *) (&m->blocking_res + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->blocking_res) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBLOCKING_REQ:
		printf("---UNBLOCKING-------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBLOCKING_REQ;
		m->unblocking_req.cc_flags = mprim.flags;
		m->unblocking_req.cc_addr_length = mprim.addr.len;
		m->unblocking_req.cc_addr_offset = sizeof(m->unblocking_req);
		c = (unsigned char *) (&m->unblocking_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->unblocking_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case UNBLOCKING_RES:
		printf("---UNBLOCKING-OK----+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_UNBLOCKING_RES;
		m->unblocking_res.cc_flags = mprim.flags;
		m->unblocking_res.cc_addr_length = mprim.addr.len;
		m->unblocking_res.cc_addr_offset = sizeof(m->unblocking_res);
		c = (unsigned char *) (&m->unblocking_res + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->unblocking_res) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case QUERY_REQ:
		printf("---QUERY------------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_QUERY_REQ;
		m->query_req.cc_flags = mprim.flags;
		m->query_req.cc_addr_length = mprim.addr.len;
		m->query_req.cc_addr_offset = sizeof(m->query_req);
		c = (unsigned char *) (&m->query_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->query_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case QUERY_RES:
		printf("---QUERY OK---------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		m->cc_primitive = CC_QUERY_RES;
		m->query_res.cc_flags = mprim.flags;
		m->query_res.cc_addr_length = mprim.addr.len;
		m->query_res.cc_addr_offset = sizeof(m->query_res);
		c = (unsigned char *) (&m->query_res + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->query_res) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case IBI:
		printf(">>>>>>>>>>>>>>>>>>>>|>>|>>> IN-BAND INFORMATION >>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case RINGING:
		printf(">>>>>>>>>>>>>>>>>>>>|>>|>>>>>>>>> RINGING >>>>>>>>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case COMMUNICATION:
		printf("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case TONE:
		printf("^>^>^>^>^>^>^>^>^>^>|>^|^>^>^> CONTINUITY TONE >^>^>^>^|^>^>^>^>^>^>^>^>^>^>(%d)\n", state);
		fflush(stdout);
		return (__RESULT_SUCCESS);
	case LOOPBACK:
		printf("                   _|__|_______________________________|                    \n");
		printf("         LOOPBACK |_|__|_____________________________>>|                    (%d)\n", state);
		printf("                    |  |                               |                    \n");
		fflush(stdout);
		return (__RESULT_SUCCESS);
	default:
		printf("\?\?\?\?----------------+->|                               |                    (%d)\n", state);
		fflush(stdout);
		return __RESULT_FAILURE;
	}
      signal_msg:
	if ((ret = putmsg(fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, RS_HIPRI)) < 0) {
		printf("***************** ERROR: putmsg failed                                         \n");
		printf("                       : %-13s:%40s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return (__RESULT_FAILURE);
	}
	return time_event(__RESULT_SUCCESS);
}

static int
iut_mgm_signal(int action)
{
	return iut_signal(action, iut_mgm_fd);
}
static int
iut_tst_signal(int action)
{
	return iut_signal(action, iut_tst_fd);
}
static int
iut_mnt_signal(int action)
{
	return iut_signal(action, iut_mnt_fd);
}

static int
pt_decode_data(struct strbuf data)
{
	int i;
	int ret;
	ulong mt;
	ulong x, y;
	unsigned char *d = (unsigned char *) data.buf, *p = (unsigned char *) data.buf, *e = (unsigned char *) data.buf + data.len;

	x = *d++;
	y = *d++;
	imsg.cic = x | (y << 8);
	mt = *d++;
	switch (mt) {
	case ISUP_MT_IAM:
		printf("                    |--+-IAM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (IAM);
		imsg.nci = *d++;
		if (d > e)
			goto decode_error;
		x = *d++;
		y = *d++;
		imsg.fci = x | (y << 8);
		if (d > e)
			goto decode_error;
		imsg.cpc = *d++;
		if (d > e)
			goto decode_error;
		imsg.tmr = *d++;
		if (d > e)
			goto decode_error;
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.cdpn.len = (*p++ - 2) << 1;
		if (p > e)
			goto decode_error;
		if (*p & 0x80)
			imsg.cdpn.len--;
		imsg.cdpn.nai = *p++ & 0x7f;
		if (p > e)
			goto decode_error;
		for (i = 0; i < imsg.cdpn.len; i += 2, p++) {
			imsg.cdpn.num[i + 0] = ((*p & 0x0f) >> 0) | 0x30;
			imsg.cdpn.num[i + 1] = ((*p & 0xf0) >> 4) | 0x30;
			if (p > e)
				goto decode_error;
		}
		imsg.cdpn.num[imsg.cdpn.len] = '\0';
		goto decode_opt;
	case ISUP_MT_SAM:
		printf("                    |--+-SAM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (SAM);
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.subn.len = (*p++ - 2) << 1;
		if (p > e)
			goto decode_error;
		if (*p & 0x80)
			imsg.subn.len--;
		imsg.subn.nai = *p++ & 0x7f;
		if (p > e)
			goto decode_error;
		for (i = 0; i < imsg.subn.len; i += 2, p++) {
			imsg.subn.num[i + 0] = ((*p & 0x0f) >> 0) | 0x30;
			imsg.subn.num[i + 1] = ((*p & 0xf0) >> 4) | 0x30;
			if (p > e)
				goto decode_error;
		}
		imsg.subn.num[imsg.subn.len] = '\0';
		goto decode_opt;
	case ISUP_MT_INR:
		printf("                    |--+-INR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (INR);
		imsg.inri = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_INF:
		printf("                    |--+-INF--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (INF);
		imsg.infi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_COT:
		ret = (COT);
		imsg.coti = *d++;
		if (imsg.coti) {
			printf("          (success) |--+-COT--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
			fflush(stdout);
		} else {
			printf("          (failure) |--+-COT--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
			fflush(stdout);
		}
		goto decode_none;
	case ISUP_MT_ACM:
		printf("                    |--+-ACM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (ACM);
	      decode_bci:
		imsg.bci = ((ulong) (*d++));
		imsg.bci |= (((ulong) (*d++)) << 8);
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CON:
		printf("                    |--+-CON--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CON);
		goto decode_bci;
	case ISUP_MT_FOT:
		printf("                    |--+-FOT--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FOT);
		goto decode_opt;
	case ISUP_MT_ANM:
		printf("                    |--+-ANM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (ANM);
		goto decode_opt;
	case ISUP_MT_REL:
		printf("                    |--+-REL--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (REL);
	      decode_caus:
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.caus.len = *p++;
		if (p > e)
			goto decode_error;
		if (imsg.caus.len < 2)
			goto decode_error;
		for (i = 0; i < imsg.caus.len; i++) {
			imsg.caus.buf[i] = *p++;
			if (p > e)
				goto decode_error;
		}
		goto decode_opt;
	case ISUP_MT_SUS:
		printf("                    |--+-SUS--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (SUS);
		imsg.sris = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_RES:
		printf("                    |--+-RES--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (RES);
		imsg.sris = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_RLC:
		printf("                    |--+-RLC--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (RLC);
		goto decode_opt;
	case ISUP_MT_CCR:
		printf("                    |--+-CCR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CCR);
		goto decode_none;
	case ISUP_MT_RSC:
		printf("                    |--+-RSC--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (RSC);
		goto decode_none;
	case ISUP_MT_BLO:
		printf("                    |--+-BLO--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (BLO);
		goto decode_none;
	case ISUP_MT_UBL:
		printf("                    |--+-UBL--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (UBL);
		goto decode_none;
	case ISUP_MT_BLA:
		printf("                    |--+-BLA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (BLA);
		goto decode_none;
	case ISUP_MT_UBA:
		printf("                    |--+-UBA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (UBA);
		goto decode_none;
	case ISUP_MT_GRS:
		printf("                    |--+-GRS--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (GRS);
	      decode_rs:
		p = d + *d;
		d++;
		if (p > e) {
			printf("**** ERROR: pointer out of range p = %p, e = %p\n", p, e);
			fflush(stdout);
			goto decode_error;
		}
		imsg.rs.len = *p++;
		if (p > e) {
			printf("**** ERROR: length out of range p = %p, e = %p\n", p, e);
			fflush(stdout);
			goto decode_error;
		}
		if (imsg.rs.len > 33) {
			printf("**** ERROR: length too large len = %ld\n", (long) imsg.rs.len);
			fflush(stdout);
			goto decode_error;
		}
		for (i = 0; i < imsg.rs.len; i++) {
			imsg.rs.buf[i] = *p++;
			if (p > e) {
				printf("**** ERROR: buffer out of range p = %p, e = %p\n", p, e);
				fflush(stdout);
				goto decode_error;
			}
		}
		goto done;
	case ISUP_MT_CGB:
		printf("                    |--+-CGB--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CGB);
	      decode_cgi:
		imsg.cgi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_rs;
	case ISUP_MT_CGU:
		printf("                    |--+-CGU--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CGU);
		goto decode_cgi;
	case ISUP_MT_CGBA:
		printf("                    |--+-CGBA-%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CGBA);
		goto decode_cgi;
	case ISUP_MT_CGUA:
		printf("                    |--+-CGUA-%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CGUA);
		goto decode_cgi;
	case ISUP_MT_CMR:
		printf("                    |--+-CMR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CMR);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CMC:
		printf("                    |--+-CMC--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CMC);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CMRJ:
		printf("                    |--+-CMRJ-%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CMRJ);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_FAR:
		printf("                    |--+-FAR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FAR);
	      decode_faci:
		imsg.faci = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_FAA:
		printf("                    |--+-FAA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FAA);
		goto decode_faci;
	case ISUP_MT_FRJ:
		printf("                    |--+-FRJ--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FRJ);
		goto decode_faci;
	case ISUP_MT_FAD:
		printf("                    |--+-FAD--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FAD);
		goto decode_faci;
	case ISUP_MT_FAI:
		printf("                    |--+-FAI--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FAI);
		goto decode_faci;
	case ISUP_MT_LPA:
		printf("                    |--+-LPA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (LPA);
		goto decode_none;
	case ISUP_MT_DRS:
		printf("                    |--+-DRS--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (DRS);
		goto decode_opt;
	case ISUP_MT_PAM:
		printf("                    |--+-PAM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (PAM);
		pmsg.pam.len = data.len - (data.buf - (char *) d);
		bcopy(d, pmsg.pam.buf, pmsg.pam.len);
		d += pmsg.pam.len;
		break;
	case ISUP_MT_GRA:
		printf("                    |--+-GRA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (GRA);
		goto decode_rs;
	case ISUP_MT_CQM:
		printf("                    |--+-CQM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CQM);
		goto decode_rs;
	case ISUP_MT_CQR:
		printf("                    |--+-CQR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CQR);
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.rs.len = *p++;
		if (p > e)
			goto decode_error;
		if (imsg.rs.len > 33)
			goto decode_error;
		for (i = 0; i < imsg.rs.len; i++) {
			imsg.rs.buf[i] = *p++;
			if (p > e)
				goto decode_error;
		}
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.csi.len = *p++;
		if (p > e)
			goto decode_error;
		for (i = 0; i < imsg.csi.len; i++) {
			imsg.csi.buf[i] = *p++;
			if (p > e)
				goto decode_error;
		}
		goto done;
	case ISUP_MT_CPG:
		printf("                    |--+-CPG--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CPG);
		imsg.evnt = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_USR:
		printf("                    |--+-USR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (USR);
		p = d + *d;
		d++;
		if (p > e)
			goto decode_error;
		imsg.uui.len = *p++;
		if (p > e)
			goto decode_error;
		for (i = 0; i < imsg.uui.len; i++) {
			imsg.uui.buf[i] = *p++;
			if (p > e)
				goto decode_error;
		}
		goto decode_opt;
	case ISUP_MT_UCIC:
		printf("                    |--+-UCIC-%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (UCIC);
		goto decode_none;
	case ISUP_MT_CFN:
		printf("                    |--+-CFN--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CFN);
		goto decode_caus;
	case ISUP_MT_OLM:
		printf("                    |--+-OLM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (OLM);
		goto decode_none;
	case ISUP_MT_CRG:
		printf("                    |--+-CRG--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CRG);
		goto decode_none;
	case ISUP_MT_NRM:
		printf("                    |--+-NRM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (NRM);
		goto decode_opt;
	case ISUP_MT_FAC:
		printf("                    |--+-FAC--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (FAC);
		goto decode_opt;
	case ISUP_MT_UPT:
		printf("                    |--+-UPT--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (UPT);
		goto decode_opt;
	case ISUP_MT_UPA:
		printf("                    |--+-UPA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (UPA);
		goto decode_opt;
	case ISUP_MT_IDR:
		printf("                    |--+-IDR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (IDR);
		goto decode_opt;
	case ISUP_MT_IRS:
		printf("                    |--+-IRS--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (IRS);
		goto decode_opt;
	case ISUP_MT_SGM:
		printf("                    |--+-SGM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (SGM);
		goto decode_opt;
	case ISUP_MT_CRA:
		printf("                    |--+-CRA--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CRA);
		goto decode_none;
	case ISUP_MT_CRM:
		printf("                    |--+-CRM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CRM);
		imsg.nci = *d++;
		goto decode_none;
	case ISUP_MT_CVR:
		printf("                    |--+-CVR--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CVR);
		imsg.cvri = *d++;
		if (d > e)
			goto decode_error;
		imsg.cgri = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CVT:
		printf("                    |--+-CVT--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CVT);
		goto decode_none;
	case ISUP_MT_EXM:
		printf("                    |--+-EXM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (EXM);
		goto decode_opt;
	case ISUP_MT_NON:
		printf("                    |--+-NON--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (NON);
		imsg.ton = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_LLM:
		printf("                    |--+-LLM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (LLM);
		goto decode_none;
	case ISUP_MT_CAK:
		printf("                    |--+-CAK--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (CAK);
		goto decode_none;
	case ISUP_MT_TCM:
		printf("                    |--+-TCM--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (TCM);
		imsg.cri = *d++;
		goto decode_none;
	case ISUP_MT_MCP:
		printf("                    |--+-MCP--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (MCP);
		goto decode_none;
	default:
		printf("                    |--+-\?\?\?--%3ld--------------------->|                    (%d)\n", imsg.cic, state);
		fflush(stdout);
		ret = (__EVENT_UNKNOWN);
		break;
	}
      done:
	if (show_msg) {
		printf("Recv msg: ");
		for (i = 0; i < data.len; i++)
			printf("%02X ", (unsigned char) data.buf[i]);
		printf("\n");
		fflush(stdout);
	}
	return (ret);
      decode_none:
	if (d != e)
		goto decode_error;
	goto done;
      decode_opt:
	p = d + *d;
	d++;
	if (p > e)
		goto decode_error;
	if (*p++) ;		/* have optional parameters */
	goto done;
      decode_error:
	printf("                    |XX+XXXXXXX DECODE ERROR XXXXXXXXXX|                    (%d)\n", state);
	fflush(stdout);
	ret = __RESULT_DECODE_ERROR;
	goto done;
}

static int
pt_decode_msg(struct strbuf ctrl, struct strbuf data)
{
	union MTP_primitives *m = (union MTP_primitives *) ctrl.buf;

	switch (m->mtp_primitive) {
	case MTP_TRANSFER_REQ:
		return pt_decode_data(data);
	}
	return (__EVENT_UNKNOWN);
}

static int
iut_cpc_decode_data(struct strbuf data)
{
	return (__EVENT_UNKNOWN);
}

static int
iut_cpc_decode_msg(struct strbuf ctrl, struct strbuf data)
{
	int ret;
	union CC_primitives *m = (union CC_primitives *) ctrl.buf;
	struct isup_addr *a;
	char *s;

	switch (m->cc_primitive) {
	case CC_OK_ACK:
		if (show_acks || verbose > 1)
			printf("<--OK---------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_OK_ACK;
		break;
	case CC_ERROR_ACK:
		switch (m->error_ack.cc_error_type) {
		case CCSYSERR:
			s = strerror(m->error_ack.cc_unix_error);
			break;
		case CCOUTSTATE:
			s = "CCOUTSTATE";
			break;
		case CCBADADDR:
			s = "CCBADADDR";
			break;
		case CCBADDIGS:
			s = "CCBADDIGS";
			break;
		case CCBADOPT:
			s = "CCBADOPT";
			break;
		case CCNOADDR:
			s = "CCNOADDR";
			break;
		case CCADDRBUSY:
			s = "CCADDRBUSY";
			break;
		case CCBADCLR:
			s = "CCBADCLR";
			break;
		case CCBADTOK:
			s = "CCBADTOK";
			break;
		case CCBADFLAG:
			s = "CCBADFLAG";
			break;
		case CCNOTSUPP:
			s = "CCNOTSUPP";
			break;
		case CCBADPRIM:
			s = "CCBADPRIM";
			break;
		default:
			s = "????????";
			break;
		}
		printf("<--ERROR------------|  |(%29s)|                    (%d)\n", s, state);
		fflush(stdout);
		ret = CPC_ERROR_ACK;
		break;
	case CC_INFO_ACK:
		if (verbose)
			printf("<--INFO-------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_INFO_ACK;
		break;
	case CC_BIND_ACK:
		if (verbose > 1) {
			printf("<--BIND-------------|  |                               |                    (%d)\n", state);
			fflush(stdout);
		}
		cprim.addr.len = m->bind_ack.cc_addr_length;
		bcopy((char *) &m->bind_ack + m->bind_ack.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		cprim.setup_ind = m->bind_ack.cc_setup_ind;
		if (verbose > 2) {
			if (cprim.addr.len) {
				printf(" addr scope =    %2s |  |                               |                    \n", scope_string(cprim.addr.add.scope));
				printf(" addr id    = %5ld |  |                               |                    \n", cprim.addr.add.id);
				printf(" addr cic   = %5ld |  |                               |                    \n", cprim.addr.add.cic);
			}
			printf(" setup inds = %5ld |  |                               |                    \n", cprim.setup_ind);
			fflush(stdout);
		}
		ret = CPC_BIND_ACK;
		break;
	case CC_OPTMGMT_ACK:
		if (verbose)
			printf("<--OPTMGMT----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_OPTMGMT_ACK;
		break;
	case CC_ADDR_ACK:
		if (verbose)
			printf("<--ADDR-------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_ADDR_ACK;
		break;
	case CC_CALL_REATTEMPT_IND:
		ret = CPC_CALL_REATTEMPT_IND;
		cprim.user_ref = m->call_reattempt_ind.cc_user_ref;
		switch ((cprim.reason = m->call_reattempt_ind.cc_reason)) {
		case ISUP_REATTEMPT_DUAL_SIEZURE:
			printf("<--CALL-REATTEMPT---|  | (dual siezure)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_RESET:
			printf("<--CALL-REATTEMPT---|  | (reset)                       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_BLOCKING:
			printf("<--CALL-REATTEMPT---|  | (blocking)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_T24_TIMEOUT:
			printf("<--CALL-REATTEMPT---|  | (t24 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_UNEXPECTED:
			printf("<--CALL-REATTEMPT---|  | (unexpected message)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_COT_FAILURE:
			printf("<--CALL-REATTEMPT---|  | (continuity check failure)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_CIRCUIT_BUSY:
			printf("<--CALL-REATTEMPT---|  | (circuit busy)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--CALL-REATTEMPT---|  | (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
		break;
	case CC_SETUP_IND:
		printf("<--SETUP------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SETUP_IND;
		cprim.user_ref = 0;
		cprim.call_ref = m->setup_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.call_type = m->setup_ind.cc_call_type;
		cprim.call_flags = m->setup_ind.cc_call_flags;
		cprim.cdpn.len = m->setup_ind.cc_cdpn_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_cdpn_offset, &cprim.cdpn.buf, cprim.cdpn.len);
		cprim.addr.len = m->setup_ind.cc_addr_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		cprim.opt.len = m->setup_ind.cc_opt_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_MORE_INFO_IND:
		printf("<--MORE-INFO--------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_MORE_INFO_IND;
		cprim.user_ref = m->more_info_ind.cc_user_ref;
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
		cprim.opt.len = m->more_info_ind.cc_opt_length;
		bcopy((char *) &m->more_info_ind + m->more_info_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_INFORMATION_IND:
		printf("<--INFORMATION------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_INFORMATION_IND;
		cprim.call_ref = m->information_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.subn.len = m->information_ind.cc_subn_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_subn_offset, &cprim.subn.buf, cprim.subn.len);
		cprim.opt.len = m->information_ind.cc_opt_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_CONT_CHECK_IND:
		printf("<--CONT-CHECK-------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_CONT_CHECK_IND;
		cprim.call_ref = m->cont_check_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.addr.len = m->cont_check_ind.cc_addr_length;
		bcopy((char *) &m->cont_check_ind + m->cont_check_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_CONT_TEST_IND:
		printf("<--CONT-TEST--------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_CONT_TEST_IND;
		cprim.call_ref = m->cont_test_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.addr.len = m->cont_test_ind.cc_addr_length;
		bcopy((char *) &m->cont_test_ind + m->cont_test_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_CONT_REPORT_IND:
		printf("<--CONT-REPORT------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_CONT_REPORT_IND;
		cprim.call_ref = m->cont_report_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.result = m->cont_report_ind.cc_result;
		break;
	case CC_SETUP_CON:
		printf("<--SETUP-OK---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SETUP_CON;
		cprim.user_ref = m->setup_con.cc_user_ref;
		cprim.call_ref = m->setup_con.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		cprim.addr.len = m->setup_con.cc_addr_length;
		bcopy((char *) &m->setup_con + m->setup_con.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_PROCEEDING_IND:
		printf("<--PROCEEDING-------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_PROCEEDING_IND;
		cprim.call_ref = m->proceeding_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->proceeding_ind.cc_flags;
		cprim.opt.len = m->proceeding_ind.cc_opt_length;
		bcopy((char *) &m->proceeding_ind + m->proceeding_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_ALERTING_IND:
		printf("<--ALERTING---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_ALERTING_IND;
		cprim.call_ref = m->alerting_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->alerting_ind.cc_flags;
		cprim.opt.len = m->alerting_ind.cc_opt_length;
		bcopy((char *) &m->alerting_ind + m->alerting_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_PROGRESS_IND:
		ret = CPC_PROGRESS_IND;
		cprim.call_ref = m->progress_ind.cc_call_ref;
		cprim.event = m->progress_ind.cc_event;
		cprim.flags = m->progress_ind.cc_flags;
		cprim.opt.len = m->progress_ind.cc_opt_length;
		bcopy((char *) &m->progress_ind + m->progress_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		switch (m->progress_ind.cc_event & ISUP_EVNT_MASK) {
		case ISUP_EVNT_ALERTING:
			printf("<--PROGRESS---------|  | (alerting)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_PROGRESS:
			printf("<--PROGRESS---------|  | (progress)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_IBI:
			printf("<--PROGRESS---------|  | (in-band info avail)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFB:
			printf("<--PROGRESS---------|  | (call forwarded busy)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFNA:
			printf("<--PROGRESS---------|  | (call fowarded no reply)      |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFU:
			printf("<--PROGRESS---------|  | (call forwarded uncond)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--PROGRESS---------|  | (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		break;
	case CC_IBI_IND:
		printf("<--IBI--------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_IBI_IND;
		cprim.call_ref = m->ibi_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->ibi_ind.cc_flags;
		cprim.opt.len = m->ibi_ind.cc_opt_length;
		bcopy((char *) &m->ibi_ind + m->ibi_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_CONNECT_IND:
		printf("<--CONNECT----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_CONNECT_IND;
		cprim.call_ref = m->connect_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->connect_ind.cc_flags;
		cprim.opt.len = m->connect_ind.cc_opt_length;
		bcopy((char *) &m->connect_ind + m->connect_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_SETUP_COMPLETE_IND:
		printf("<--SETUP-COMPLETE---|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SETUP_COMPLETE_IND;
		cprim.call_ref = m->setup_complete_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.opt.len = m->setup_complete_ind.cc_opt_length;
		bcopy((char *) &m->setup_complete_ind + m->setup_complete_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_FORWXFER_IND:
		printf("<--FORWXFER---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_FORWXFER_IND;
		cprim.call_ref = m->forwxfer_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.opt.len = m->forwxfer_ind.cc_opt_length;
		bcopy((char *) &m->forwxfer_ind + m->forwxfer_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_SUSPEND_IND:
		printf("<--SUSPEND----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SUSPEND_IND;
		cprim.call_ref = m->suspend_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->suspend_ind.cc_flags;
		cprim.opt.len = m->suspend_ind.cc_opt_length;
		bcopy((char *) &m->suspend_ind + m->suspend_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_SUSPEND_CON:
		printf("<--SUSPEND-OK-------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SUSPEND_CON;
		cprim.call_ref = m->suspend_con.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.opt.len = m->suspend_con.cc_opt_length;
		bcopy((char *) &m->suspend_con + m->suspend_con.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_SUSPEND_REJECT_IND:
		printf("<--SUSPEND-REJECT---|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_SUSPEND_REJECT_IND;
		cprim.call_ref = m->suspend_reject_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.cause = m->suspend_reject_ind.cc_cause;
		cprim.opt.len = m->suspend_reject_ind.cc_opt_length;
		bcopy((char *) &m->suspend_reject_ind + m->suspend_reject_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RESUME_IND:
		printf("<--RESUME-----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RESUME_IND;
		cprim.call_ref = m->resume_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.flags = m->resume_ind.cc_flags;
		cprim.opt.len = m->resume_ind.cc_opt_length;
		bcopy((char *) &m->resume_ind + m->resume_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RESUME_CON:
		printf("<--RESUME-OK--------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RESUME_CON;
		cprim.call_ref = m->resume_con.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.opt.len = m->resume_con.cc_opt_length;
		bcopy((char *) &m->resume_con + m->resume_con.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RESUME_REJECT_IND:
		printf("<--RESUME-REJECT----|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RESUME_REJECT_IND;
		cprim.call_ref = m->resume_reject_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		cprim.cause = m->resume_reject_ind.cc_cause;
		cprim.opt.len = m->resume_reject_ind.cc_opt_length;
		bcopy((char *) &m->resume_reject_ind + m->resume_reject_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_REJECT_IND:
		printf("<--REJECT-----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_REJECT_IND;
		cprim.user_ref = m->reject_ind.cc_user_ref;
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
		cprim.cause = m->reject_ind.cc_cause;
		cprim.opt.len = m->reject_ind.cc_opt_length;
		bcopy((char *) &m->reject_ind + m->reject_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_CALL_FAILURE_IND:
		ret = CPC_CALL_FAILURE_IND;
		cprim.call_ref = m->call_failure_ind.cc_call_ref;
		cprim.reason = m->call_failure_ind.cc_reason;
		cprim.cause = m->call_failure_ind.cc_cause;
		switch (m->call_failure_ind.cc_reason) {
		case ISUP_CALL_FAILURE_COT_FAILURE:
			printf("<--CALL-FAILURE-----|  | (continuity check failure)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_RESET:
			printf("<--CALL-FAILURE-----|  | (reset)                       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_RECV_RLC:
			printf("<--CALL-FAILURE-----|  | (received RLC)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_BLOCKING:
			printf("<--CALL-FAILURE-----|  | (blocking)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T2_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t2 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T3_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t3 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T6_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t6 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T7_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t7 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T8_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t8 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T9_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t9 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T35_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t35 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T38_TIMEOUT:
			printf("<--CALL-FAILURE-----|  | (t38 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_CIRCUIT_BUSY:
			printf("<--CALL-FAILURE-----|  | (circuit busy)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--CALL-FAILURE-----|  | (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		break;
	case CC_RELEASE_IND:
		printf("<--RELEASE----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RELEASE_IND;
		cprim.user_ref = m->release_ind.cc_user_ref;
		cprim.call_ref = m->release_ind.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		cprim.cause = m->release_ind.cc_cause;
		cprim.opt.len = m->release_ind.cc_opt_length;
		bcopy((char *) &m->release_ind + m->release_ind.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RELEASE_CON:
		printf("<--RELEASE-OK-------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RELEASE_CON;
		cprim.user_ref = m->release_con.cc_user_ref;
		cprim.call_ref = m->release_con.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", cprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		}
		cprim.opt.len = m->release_con.cc_opt_length;
		bcopy((char *) &m->release_con + m->release_con.cc_opt_offset, &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RESET_IND:
		printf("<--RESET------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RESET_IND;
		cprim.flags = m->reset_ind.cc_flags;
		cprim.addr.len = m->reset_ind.cc_addr_length;
		bcopy((char *) &m->reset_ind + m->reset_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_RESET_CON:
		printf("<--RESET-OK---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_RESET_CON;
		cprim.flags = m->reset_con.cc_flags;
		cprim.addr.len = m->reset_con.cc_addr_length;
		bcopy((char *) &m->reset_con + m->reset_con.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_BLOCKING_IND:
		printf("<--BLOCK------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_BLOCKING_IND;
		cprim.flags = m->blocking_ind.cc_flags;
		cprim.addr.len = m->blocking_ind.cc_addr_length;
		bcopy((char *) &m->blocking_ind + m->blocking_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_BLOCKING_CON:
		printf("<--BLOCK-OK---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_BLOCKING_CON;
		cprim.flags = m->blocking_con.cc_flags;
		cprim.addr.len = m->blocking_con.cc_addr_length;
		bcopy((char *) &m->blocking_con + m->blocking_con.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_UNBLOCKING_IND:
		printf("<--UNBLOCK----------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_UNBLOCKING_IND;
		cprim.flags = m->unblocking_ind.cc_flags;
		cprim.addr.len = m->unblocking_ind.cc_addr_length;
		bcopy((char *) &m->unblocking_ind + m->unblocking_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_UNBLOCKING_CON:
		printf("<--UNBLOCK-OK-------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_UNBLOCKING_CON;
		cprim.flags = m->unblocking_con.cc_flags;
		cprim.addr.len = m->unblocking_con.cc_addr_length;
		bcopy((char *) &m->unblocking_con + m->unblocking_con.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_QUERY_IND:
		printf("<--QUERY------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_QUERY_IND;
		cprim.flags = m->query_ind.cc_flags;
		cprim.addr.len = m->query_ind.cc_addr_length;
		bcopy((char *) &m->query_ind + m->query_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_QUERY_CON:
		printf("<--QUERY-OK---------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_QUERY_CON;
		cprim.flags = m->query_con.cc_flags;
		cprim.addr.len = m->query_con.cc_addr_length;
		bcopy((char *) &m->query_con + m->query_con.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		break;
	case CC_STOP_IND:
		printf("<--STOP-------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_STOP_IND;
		break;
	case CC_MAINT_IND:
		ret = CPC_MAINT_IND;
		cprim.reason = m->maint_ind.cc_reason;
		cprim.call_ref = m->maint_ind.cc_call_ref;
		cprim.addr.len = m->maint_ind.cc_addr_length;
		bcopy((char *) &m->maint_ind + m->maint_ind.cc_addr_offset, &cprim.addr.add, cprim.addr.len);
		switch (m->maint_ind.cc_reason) {
		case ISUP_MAINT_T5_TIMEOUT:
			printf("<--MAINT------------|  | (t5 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T13_TIMEOUT:
			printf("<--MAINT------------|  | (t13 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T15_TIMEOUT:
			printf("<--MAINT------------|  | (t15 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T17_TIMEOUT:
			printf("<--MAINT------------|  | (t17 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T19_TIMEOUT:
			printf("<--MAINT------------|  | (t19 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T21_TIMEOUT:
			printf("<--MAINT------------|  | (t21 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T23_TIMEOUT:
			printf("<--MAINT------------|  | (t23 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T25_TIMEOUT:
			printf("<--MAINT------------|  | (t25 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T26_TIMEOUT:
			printf("<--MAINT------------|  | (t26 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T27_TIMEOUT:
			printf("<--MAINT------------|  | (t27 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T28_TIMEOUT:
			printf("<--MAINT------------|  | (t28 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T36_TIMEOUT:
			printf("<--MAINT------------|  | (t36 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGBA:
			printf("<--MAINT------------|  | (unexpected cgba)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGUA:
			printf("<--MAINT------------|  | (unexpected cgua)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_MESSAGE:
			printf("<--MAINT------------|  | (unexpected message)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEQUIPPED_CIC:
			a = (isup_addr_t *) (((unsigned char *) m) + m->maint_ind.cc_addr_offset);
			printf("<--MAINT------------|  | (unequipped cic = %5ld)      |                    (%d)\n", a->cic, state);
			fflush(stdout);
			break;
		case ISUP_MAINT_SEGMENTATION_DISCARDED:
			printf("<--MAINT------------|  | (segmentation discarded)      |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNEQUIPPED:
			printf("<--MAINT------------|  | (user part unequipped)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNAVAILABLE:
			printf("<--MAINT------------|  | (user part unavailable)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_AVAILABLE:
			printf("<--MAINT------------|  | (user part available)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_MAN_MADE_BUSY:
			printf("<--MAINT------------|  | (user part man made busy)     |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_CONGESTED:
			printf("<--MAINT------------|  | (user part congested)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNCONGESTED:
			printf("<--MAINT------------|  | (user part uncongested)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGBA:
			printf("<--MAINT------------|  | (missing ack in cgba)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGUA:
			printf("<--MAINT------------|  | (missing ack in cgua)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGBA:
			printf("<--MAINT------------|  | (abnormal ack in cgba)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGUA:
			printf("<--MAINT------------|  | (abnormal ack in cgua)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_BLA:
			printf("<--MAINT------------|  | (unexpected bla)              |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_UBA:
			printf("<--MAINT------------|  | (unexpected uba)              |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_RELEASE_UNREC_INFO:
			printf("<--MAINT------------|  | (unrecognized information)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_RELEASE_FAILURE:
			printf("<--MAINT------------|  | (release failure)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MESSAGE_FORMAT_ERROR:
			printf("<--MAINT------------|  | (message format error)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--MAINT------------|  | (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			return (__RESULT_FAILURE);
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", cprim.call_ref);
		break;
	case CC_START_RESET_IND:
		printf("<--START-RESET------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		ret = CPC_START_RESET_IND;
		break;
	default:
		printf("<--\?\?\?\?-------------|  |                               |                    (%d)\n", state);
		fflush(stdout);
		return (__EVENT_UNKNOWN);
	}
	return (ret);
}

static int
iut_mgm_decode_data(struct strbuf data)
{
	return (__EVENT_UNKNOWN);
}

static int
iut_mgm_decode_msg(struct strbuf ctrl, struct strbuf data)
{
	int ret;
	union CC_primitives *m = (union CC_primitives *) ctrl.buf;
	struct isup_addr *a;
	char *s;

	switch (m->cc_primitive) {
	case CC_OK_ACK:
		if (show_acks || verbose > 1)
			printf("<--OK---------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_OK_ACK;
		break;
	case CC_ERROR_ACK:
		switch (m->error_ack.cc_error_type) {
		case CCSYSERR:
			s = strerror(m->error_ack.cc_unix_error);
			break;
		case CCOUTSTATE:
			s = "CCOUTSTATE";
			break;
		case CCBADADDR:
			s = "CCBADADDR";
			break;
		case CCBADOPT:
			s = "CCBADOPT";
			break;
		case CCNOADDR:
			s = "CCNOADDR";
			break;
		case CCADDRBUSY:
			s = "CCADDRBUSY";
			break;
		case CCBADCLR:
			s = "CCBADCLR";
			break;
		case CCBADTOK:
			s = "CCBADTOK";
			break;
		case CCBADFLAG:
			s = "CCBADFLAG";
			break;
		case CCNOTSUPP:
			s = "CCNOTSUPP";
			break;
		case CCBADPRIM:
			s = "CCBADPRIM";
			break;
		default:
			s = "????????";
			break;
		}
		printf("<--ERROR------------+--|(%29s)|                    (%d)\n", s, state);
		fflush(stdout);
		ret = MGM_ERROR_ACK;
		break;
	case CC_INFO_ACK:
		if (verbose)
			printf("<--INFO-------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_INFO_ACK;
		break;
	case CC_BIND_ACK:
		if (verbose > 1) {
			printf("<--BIND-------------+--|                               |                    (%d)\n", state);
			fflush(stdout);
		}
		mprim.addr.len = m->bind_ack.cc_addr_length;
		bcopy((char *) &m->bind_ack + m->bind_ack.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		mprim.setup_ind = m->bind_ack.cc_setup_ind;
		if (verbose > 2) {
			if (mprim.addr.len) {
				printf(" addr scope =    %2s |  |                               |                    \n", scope_string(mprim.addr.add.scope));
				printf(" addr id    = %5ld |  |                               |                    \n", mprim.addr.add.id);
				printf(" addr cic   = %5ld |  |                               |                    \n", mprim.addr.add.cic);
			}
			printf(" setup inds = %5ld |  |                               |                    \n", mprim.setup_ind);
			fflush(stdout);
		}
		ret = MGM_BIND_ACK;
		break;
	case CC_OPTMGMT_ACK:
		if (verbose)
			printf("<--OPTMGMT----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_OPTMGMT_ACK;
		break;
	case CC_ADDR_ACK:
		if (verbose)
			printf("<--ADDR-------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_ADDR_ACK;
		break;
	case CC_CALL_REATTEMPT_IND:
		ret = MGM_CALL_REATTEMPT_IND;
		mprim.user_ref = m->call_reattempt_ind.cc_user_ref;
		switch ((mprim.reason = m->call_reattempt_ind.cc_reason)) {
		case ISUP_REATTEMPT_DUAL_SIEZURE:
			printf("<--CALL-REATTEMPT---+--| (dual siezure)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_RESET:
			printf("<--CALL-REATTEMPT---+--| (reset)                       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_BLOCKING:
			printf("<--CALL-REATTEMPT---+--| (blocking)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_T24_TIMEOUT:
			printf("<--CALL-REATTEMPT---+--| (t24 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_UNEXPECTED:
			printf("<--CALL-REATTEMPT---+--| (unexpected message)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_COT_FAILURE:
			printf("<--CALL-REATTEMPT---+--| (continuity check failure)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_REATTEMPT_CIRCUIT_BUSY:
			printf("<--CALL-REATTEMPT---+--| (circuit busy)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--CALL-REATTEMPT---+--| (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
		break;
	case CC_SETUP_IND:
		printf("<--SETUP------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SETUP_IND;
		mprim.user_ref = 0;
		mprim.call_ref = m->setup_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.call_type = m->setup_ind.cc_call_type;
		mprim.call_flags = m->setup_ind.cc_call_flags;
		mprim.cdpn.len = m->setup_ind.cc_cdpn_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_cdpn_offset, &mprim.cdpn.buf, mprim.cdpn.len);
		mprim.addr.len = m->setup_ind.cc_addr_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		mprim.opt.len = m->setup_ind.cc_opt_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_MORE_INFO_IND:
		printf("<--MORE-INFO--------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_MORE_INFO_IND;
		mprim.user_ref = m->more_info_ind.cc_user_ref;
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
		mprim.opt.len = m->more_info_ind.cc_opt_length;
		bcopy((char *) &m->more_info_ind + m->more_info_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_INFORMATION_IND:
		printf("<--INFORMATION------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_INFORMATION_IND;
		mprim.call_ref = m->information_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.subn.len = m->information_ind.cc_subn_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_subn_offset, &mprim.subn.buf, mprim.subn.len);
		mprim.opt.len = m->information_ind.cc_opt_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_CONT_CHECK_IND:
		printf("<--CONT-CHECK-------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_CONT_CHECK_IND;
		mprim.call_ref = m->cont_check_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.addr.len = m->cont_check_ind.cc_addr_length;
		bcopy((char *) &m->cont_check_ind + m->cont_check_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_CONT_TEST_IND:
		printf("<--CONT-TEST--------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_CONT_TEST_IND;
		mprim.call_ref = m->cont_test_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.addr.len = m->cont_test_ind.cc_addr_length;
		bcopy((char *) &m->cont_test_ind + m->cont_test_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_CONT_REPORT_IND:
		printf("<--CONT-REPORT------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_CONT_REPORT_IND;
		mprim.call_ref = m->cont_report_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.result = m->cont_report_ind.cc_result;
		break;
	case CC_SETUP_CON:
		printf("<--SETUP-OK---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SETUP_CON;
		mprim.user_ref = m->setup_con.cc_user_ref;
		mprim.call_ref = m->setup_con.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		mprim.addr.len = m->setup_con.cc_addr_length;
		bcopy((char *) &m->setup_con + m->setup_con.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_PROCEEDING_IND:
		printf("<--PROCEEDING-------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_PROCEEDING_IND;
		mprim.call_ref = m->proceeding_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->proceeding_ind.cc_flags;
		mprim.opt.len = m->proceeding_ind.cc_opt_length;
		bcopy((char *) &m->proceeding_ind + m->proceeding_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_ALERTING_IND:
		printf("<--ALERTING---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_ALERTING_IND;
		mprim.call_ref = m->alerting_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->alerting_ind.cc_flags;
		mprim.opt.len = m->alerting_ind.cc_opt_length;
		bcopy((char *) &m->alerting_ind + m->alerting_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_PROGRESS_IND:
		ret = MGM_PROGRESS_IND;
		mprim.call_ref = m->progress_ind.cc_call_ref;
		mprim.event = m->progress_ind.cc_event;
		mprim.flags = m->progress_ind.cc_flags;
		mprim.opt.len = m->progress_ind.cc_opt_length;
		bcopy((char *) &m->progress_ind + m->progress_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		switch (m->progress_ind.cc_event & ISUP_EVNT_MASK) {
		case ISUP_EVNT_ALERTING:
			printf("<--PROGRESS---------+--| (alerting)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_PROGRESS:
			printf("<--PROGRESS---------+--| (progress)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_IBI:
			printf("<--PROGRESS---------+--| (in-band info avail)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFB:
			printf("<--PROGRESS---------+--| (call forwarded busy)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFNA:
			printf("<--PROGRESS---------+--| (call fowarded no reply)      |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_EVNT_CFU:
			printf("<--PROGRESS---------+--| (call forwarded uncond)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--PROGRESS---------+--| (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		break;
	case CC_IBI_IND:
		printf("<--IBI--------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_IBI_IND;
		mprim.call_ref = m->ibi_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->ibi_ind.cc_flags;
		mprim.opt.len = m->ibi_ind.cc_opt_length;
		bcopy((char *) &m->ibi_ind + m->ibi_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_CONNECT_IND:
		printf("<--CONNECT----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_CONNECT_IND;
		mprim.call_ref = m->connect_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->connect_ind.cc_flags;
		mprim.opt.len = m->connect_ind.cc_opt_length;
		bcopy((char *) &m->connect_ind + m->connect_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_SETUP_COMPLETE_IND:
		printf("<--SETUP-COMPLETE---+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SETUP_COMPLETE_IND;
		mprim.call_ref = m->setup_complete_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.opt.len = m->setup_complete_ind.cc_opt_length;
		bcopy((char *) &m->setup_complete_ind + m->setup_complete_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_FORWXFER_IND:
		printf("<--FORWXFER---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_FORWXFER_IND;
		mprim.call_ref = m->forwxfer_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.opt.len = m->forwxfer_ind.cc_opt_length;
		bcopy((char *) &m->forwxfer_ind + m->forwxfer_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_SUSPEND_IND:
		printf("<--SUSPEND----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SUSPEND_IND;
		mprim.call_ref = m->suspend_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->suspend_ind.cc_flags;
		mprim.opt.len = m->suspend_ind.cc_opt_length;
		bcopy((char *) &m->suspend_ind + m->suspend_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_SUSPEND_CON:
		printf("<--SUSPEND-OK-------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SUSPEND_CON;
		mprim.call_ref = m->suspend_con.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.opt.len = m->suspend_con.cc_opt_length;
		bcopy((char *) &m->suspend_con + m->suspend_con.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_SUSPEND_REJECT_IND:
		printf("<--SUSPEND-REJECT---+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_SUSPEND_REJECT_IND;
		mprim.call_ref = m->suspend_reject_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.cause = m->suspend_reject_ind.cc_cause;
		mprim.opt.len = m->suspend_reject_ind.cc_opt_length;
		bcopy((char *) &m->suspend_reject_ind + m->suspend_reject_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RESUME_IND:
		printf("<--RESUME-----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RESUME_IND;
		mprim.call_ref = m->resume_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.flags = m->resume_ind.cc_flags;
		mprim.opt.len = m->resume_ind.cc_opt_length;
		bcopy((char *) &m->resume_ind + m->resume_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RESUME_CON:
		printf("<--RESUME-OK--------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RESUME_CON;
		mprim.call_ref = m->resume_con.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.opt.len = m->resume_con.cc_opt_length;
		bcopy((char *) &m->resume_con + m->resume_con.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RESUME_REJECT_IND:
		printf("<--RESUME-REJECT----+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RESUME_REJECT_IND;
		mprim.call_ref = m->resume_reject_ind.cc_call_ref;
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		mprim.cause = m->resume_reject_ind.cc_cause;
		mprim.opt.len = m->resume_reject_ind.cc_opt_length;
		bcopy((char *) &m->resume_reject_ind + m->resume_reject_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_REJECT_IND:
		printf("<--REJECT-----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_REJECT_IND;
		mprim.user_ref = m->reject_ind.cc_user_ref;
		if (verbose)
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
		mprim.cause = m->reject_ind.cc_cause;
		mprim.opt.len = m->reject_ind.cc_opt_length;
		bcopy((char *) &m->reject_ind + m->reject_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_CALL_FAILURE_IND:
		ret = MGM_CALL_FAILURE_IND;
		mprim.call_ref = m->call_failure_ind.cc_call_ref;
		mprim.reason = m->call_failure_ind.cc_reason;
		mprim.cause = m->call_failure_ind.cc_cause;
		switch (m->call_failure_ind.cc_reason) {
		case ISUP_CALL_FAILURE_COT_FAILURE:
			printf("<--CALL-FAILURE-----+--| (continuity check failure)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_RESET:
			printf("<--CALL-FAILURE-----+--| (reset)                       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_RECV_RLC:
			printf("<--CALL-FAILURE-----+--| (received RLC)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_BLOCKING:
			printf("<--CALL-FAILURE-----+--| (blocking)                    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T2_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t2 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T3_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t3 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T6_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t6 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T7_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t7 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T8_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t8 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T9_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t9 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T35_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t35 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_T38_TIMEOUT:
			printf("<--CALL-FAILURE-----+--| (t38 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_CALL_FAILURE_CIRCUIT_BUSY:
			printf("<--CALL-FAILURE-----+--| (circuit busy)                |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--CALL-FAILURE-----+--| (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			break;
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		break;
	case CC_RELEASE_IND:
		printf("<--RELEASE----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RELEASE_IND;
		mprim.user_ref = m->release_ind.cc_user_ref;
		mprim.call_ref = m->release_ind.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		mprim.cause = m->release_ind.cc_cause;
		mprim.opt.len = m->release_ind.cc_opt_length;
		bcopy((char *) &m->release_ind + m->release_ind.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RELEASE_CON:
		printf("<--RELEASE-OK-------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RELEASE_CON;
		mprim.user_ref = m->release_con.cc_user_ref;
		mprim.call_ref = m->release_con.cc_call_ref;
		if (verbose) {
			printf("user ref = %-4ld     |  |                               |                    \n", mprim.user_ref);
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		}
		mprim.opt.len = m->release_con.cc_opt_length;
		bcopy((char *) &m->release_con + m->release_con.cc_opt_offset, &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RESET_IND:
		printf("<--RESET------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RESET_IND;
		mprim.flags = m->reset_ind.cc_flags;
		mprim.addr.len = m->reset_ind.cc_addr_length;
		bcopy((char *) &m->reset_ind + m->reset_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_RESET_CON:
		printf("<--RESET-OK---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_RESET_CON;
		mprim.flags = m->reset_con.cc_flags;
		mprim.addr.len = m->reset_con.cc_addr_length;
		bcopy((char *) &m->reset_con + m->reset_con.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_BLOCKING_IND:
		printf("<--BLOCK------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_BLOCKING_IND;
		mprim.flags = m->blocking_ind.cc_flags;
		mprim.addr.len = m->blocking_ind.cc_addr_length;
		bcopy((char *) &m->blocking_ind + m->blocking_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_BLOCKING_CON:
		printf("<--BLOCK-OK---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_BLOCKING_CON;
		mprim.flags = m->blocking_con.cc_flags;
		mprim.addr.len = m->blocking_con.cc_addr_length;
		bcopy((char *) &m->blocking_con + m->blocking_con.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_UNBLOCKING_IND:
		printf("<--UNBLOCK----------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_UNBLOCKING_IND;
		mprim.flags = m->unblocking_ind.cc_flags;
		mprim.addr.len = m->unblocking_ind.cc_addr_length;
		bcopy((char *) &m->unblocking_ind + m->unblocking_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_UNBLOCKING_CON:
		printf("<--UNBLOCK-OK-------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_UNBLOCKING_CON;
		mprim.flags = m->unblocking_con.cc_flags;
		mprim.addr.len = m->unblocking_con.cc_addr_length;
		bcopy((char *) &m->unblocking_con + m->unblocking_con.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_QUERY_IND:
		printf("<--QUERY------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_QUERY_IND;
		mprim.flags = m->query_ind.cc_flags;
		mprim.addr.len = m->query_ind.cc_addr_length;
		bcopy((char *) &m->query_ind + m->query_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_QUERY_CON:
		printf("<--QUERY-OK---------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_QUERY_CON;
		mprim.flags = m->query_con.cc_flags;
		mprim.addr.len = m->query_con.cc_addr_length;
		bcopy((char *) &m->query_con + m->query_con.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		break;
	case CC_STOP_IND:
		printf("<--STOP-------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_STOP_IND;
		break;
	case CC_MAINT_IND:
		ret = MGM_MAINT_IND;
		mprim.reason = m->maint_ind.cc_reason;
		mprim.call_ref = m->maint_ind.cc_call_ref;
		mprim.addr.len = m->maint_ind.cc_addr_length;
		bcopy((char *) &m->maint_ind + m->maint_ind.cc_addr_offset, &mprim.addr.add, mprim.addr.len);
		switch (m->maint_ind.cc_reason) {
		case ISUP_MAINT_T5_TIMEOUT:
			printf("<--MAINT------------+--| (t5 timeout)                  |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T13_TIMEOUT:
			printf("<--MAINT------------+--| (t13 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T15_TIMEOUT:
			printf("<--MAINT------------+--| (t15 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T17_TIMEOUT:
			printf("<--MAINT------------+--| (t17 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T19_TIMEOUT:
			printf("<--MAINT------------+--| (t19 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T21_TIMEOUT:
			printf("<--MAINT------------+--| (t21 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T23_TIMEOUT:
			printf("<--MAINT------------+--| (t23 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T25_TIMEOUT:
			printf("<--MAINT------------+--| (t25 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T26_TIMEOUT:
			printf("<--MAINT------------+--| (t26 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T27_TIMEOUT:
			printf("<--MAINT------------+--| (t27 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T28_TIMEOUT:
			printf("<--MAINT------------+--| (t28 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_T36_TIMEOUT:
			printf("<--MAINT------------+--| (t36 timeout)                 |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGBA:
			printf("<--MAINT------------+--| (unexpected cgba)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGUA:
			printf("<--MAINT------------+--| (unexpected cgua)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_MESSAGE:
			printf("<--MAINT------------+--| (unexpected message)          |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEQUIPPED_CIC:
			a = (isup_addr_t *) (((unsigned char *) m) + m->maint_ind.cc_addr_offset);
			printf("<--MAINT------------|  | (unequipped cic = %5ld)      |                    (%d)\n", a->cic, state);
			fflush(stdout);
			break;
		case ISUP_MAINT_SEGMENTATION_DISCARDED:
			printf("<--MAINT------------+--| (segmentation discarded)      |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNEQUIPPED:
			printf("<--MAINT------------+--| (user part unequipped)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNAVAILABLE:
			printf("<--MAINT------------+--| (user part unavailable)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_AVAILABLE:
			printf("<--MAINT------------+--| (user part available)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_MAN_MADE_BUSY:
			printf("<--MAINT------------+--| (user part man made busy)     |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_CONGESTED:
			printf("<--MAINT------------+--| (user part congested)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNCONGESTED:
			printf("<--MAINT------------+--| (user part uncongested)       |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGBA:
			printf("<--MAINT------------+--| (missing ack in cgba)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGUA:
			printf("<--MAINT------------+--| (missing ack in cgua)         |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGBA:
			printf("<--MAINT------------+--| (abnormal ack in cgba)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGUA:
			printf("<--MAINT------------+--| (abnormal ack in cgua)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_BLA:
			printf("<--MAINT------------+--| (unexpected bla)              |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_UBA:
			printf("<--MAINT------------+--| (unexpected uba)              |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_RELEASE_UNREC_INFO:
			printf("<--MAINT------------+--| (unrecognized information)    |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_RELEASE_FAILURE:
			printf("<--MAINT------------+--| (release failure)             |                    (%d)\n", state);
			fflush(stdout);
			break;
		case ISUP_MAINT_MESSAGE_FORMAT_ERROR:
			printf("<--MAINT------------+--| (message format error)        |                    (%d)\n", state);
			fflush(stdout);
			break;
		default:
			printf("<--MAINT------------+--| (\?\?\?\?)                        |                    (%d)\n", state);
			fflush(stdout);
			return (__RESULT_FAILURE);
		}
		if (verbose)
			printf("call ref = %-4ld     |  |                               |                    \n", mprim.call_ref);
		break;
	case CC_START_RESET_IND:
		printf("<--START-RESET------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		ret = MGM_START_RESET_IND;
		break;
	default:
		printf("<--\?\?\?\?-------------+--|                               |                    (%d)\n", state);
		fflush(stdout);
		return (__EVENT_UNKNOWN);
	}
	return (ret);
}

static int show_timeout = 0;

#define CPC 0x00000001UL
#define MGM 0x00000002UL
#define MNT 0x00000004UL
#define TST 0x00000008UL
#define PT  0x00000010UL
#define ANY 0x0000001fUL

static int
wait_event(int wait, int source)
{
	while (1) {
		struct pollfd pfd[] = {
			{pt_fd, POLLIN | POLLPRI, 0},
			{iut_cpc_fd, POLLIN | POLLPRI, 0},
			{iut_tst_fd, POLLIN | POLLPRI, 0},
			{iut_mgm_fd, POLLIN | POLLPRI, 0},
			{iut_mnt_fd, POLLIN | POLLPRI, 0}
		};

		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose) {
				printf("++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++(%d)\n", state);
				fflush(stdout);
				show_timeout--;
			}
			last_event = __EVENT_TIMEOUT;
			return time_event(__EVENT_TIMEOUT);
		}
		// printf("polling:\n");
		// fflush(stdout);
		pfd[0].fd = pt_fd;
		pfd[0].events = (source & PT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[0].revents = 0;
		pfd[1].fd = iut_cpc_fd;
		pfd[1].events = (source & CPC) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[1].revents = 0;
		pfd[2].fd = iut_tst_fd;
		pfd[2].events = (source & TST) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[2].revents = 0;
		pfd[3].fd = iut_mgm_fd;
		pfd[3].events = (source & MGM) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[3].revents = 0;
		pfd[4].fd = iut_mnt_fd;
		pfd[4].events = (source & MNT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[4].revents = 0;
		switch (poll(pfd, 5, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			if (verbose) {
				printf("X-X-X-X-X-X-X-X-X-X-|-X|X-X-X-X-X- ERROR! -X-X-X-X-X-X-|-X-X-X-X-X-X-X-X-X-X(%d)\n", state);
				fflush(stdout);
			}
			break;
		case 0:
			if (verbose) {
				printf("- - - - - - - - - - | -|- - - - - nothing! - - - - - - | - - - - - - - - - -(%d)\n", state);
				fflush(stdout);
			}
			last_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			// printf("polled:\n");
			// fflush(stdout);
			if (pfd[4].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, (char *) iut_ctl };
				struct strbuf data = { BUFSIZE, 0, (char *) iut_dat };

				// printf("getmsg from iut_mgm:\n");
				// fflush(stdout);
				if (getmsg(iut_mnt_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = iut_mgm_decode_msg(ctrl, data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = iut_mgm_decode_data(data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			// printf("polled:\n");
			// fflush(stdout);
			if (pfd[3].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, (char *) iut_ctl };
				struct strbuf data = { BUFSIZE, 0, (char *) iut_dat };

				// printf("getmsg from iut_mgm:\n");
				// fflush(stdout);
				if (getmsg(iut_mgm_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = iut_mgm_decode_msg(ctrl, data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = iut_mgm_decode_data(data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			// printf("polled:\n");
			// fflush(stdout);
			if (pfd[2].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, (char *) iut_ctl };
				struct strbuf data = { BUFSIZE, 0, (char *) iut_dat };

				// printf("getmsg from iut_mgm:\n");
				// fflush(stdout);
				if (getmsg(iut_tst_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = iut_mgm_decode_msg(ctrl, data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = iut_mgm_decode_data(data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[1].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, (char *) iut_ctl };
				struct strbuf data = { BUFSIZE, 0, (char *) iut_dat };

				// printf("getmsg from iut:\n");
				// fflush(stdout);
				if (getmsg(iut_cpc_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = iut_cpc_decode_msg(ctrl, data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = iut_cpc_decode_data(data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[0].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, (char *) pt_ctl };
				struct strbuf data = { BUFSIZE, 0, (char *) pt_dat };

				// printf("getmsg from pt:\n");
				// fflush(stdout);
				if (getmsg(pt_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from pt [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = pt_decode_msg(ctrl, data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = pt_decode_data(data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
}

int
any_event(void)
{
	return wait_event(-1, ANY);
}

int
cpc_event(void)
{
	return wait_event(-1, CPC);
}

int
tst_event(void)
{
	return wait_event(-1, TST);
}

int
mgm_event(void)
{
	return wait_event(-1, MGM);
}

int
mnt_event(void)
{
	return wait_event(-1, MNT);
}

int
pt_event(void)
{
	return wait_event(-1, PT);
}

int
event(int source)
{
	return wait_event(-1, source);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_0(void)
{
	state = 0;
	start_tt(1000);
	// if (send(RESUME) != __RESULT_SUCCESS)
	// return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
};

static int
preamble_1(void)
{
	/* start with a blocked circuit at SP A */
	state = 0;
	start_tt(1000);
	// if (send(RESUME) != __RESULT_SUCCESS)
	// return __RESULT_FAILURE;
	state = 1;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 2;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 3;
	pmsg.cic = 12;
	send(BLA);
	state = 4;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int
postamble_0(void)
{
	while (wait_event(0, ANY) != NO_MSG) ;
	state = 0;
	stop_tt();
	if (send(PAUSE) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
};

static int
postamble_1(void)
{
	/* unblock blocked circuit at SP A */
	state = 0;
	while (wait_event(0, ANY) != NO_MSG) ;
	state = 1;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 2;
	if (pt_event() != UBL)
		goto failure;
	pmsg.cic = 12;
	send(UBA);
	if (mgm_event() != MGM_UNBLOCKING_CON)
		goto failure;
	stop_tt();
	send(PAUSE);
	return __RESULT_SUCCESS;
      failure:
	stop_tt();
	send(PAUSE);
	return __RESULT_FAILURE;
}

static int
postamble_2(void)
{
	/* unblock blocked circuit at SP B */
	state = 0;
	while (wait_event(0, ANY) != NO_MSG) ;
	state = 1;
	pmsg.cic = 12;
	send(UBL);
	state = 2;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		goto failure;
	state = 3;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 4;
	if (mgm_event() != MGM_OK_ACK)
		goto failure;
	state = 5;
	if (pt_event() != UBA)
		goto failure;
	state = 6;
	stop_tt();
	send(PAUSE);
	return __RESULT_SUCCESS;
      failure:
	stop_tt();
	send(PAUSE);
	return __RESULT_FAILURE;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  The Test Cases...
 *
 *  -------------------------------------------------------------------------
 */

#define tgrp_case_1_1 "Circuit supervision"
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Non-allocated circuits"
#define xtra_case_1_1 NULL
#define sref_case_1_1 NULL
#define desc_case_1_1 "\
Verify that on receipt of a CIC relating to a circuit which does not exist,\n\
SP A will discard the message and alert the maintenance system."

static int
test_case_1_1_func(void)
{
	state = 0;
	pmsg.cic = 16;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

struct test_stream test_case_1_1_all = { preamble_0, test_case_1_1_func, postamble_0 };

#define test_case_1_1 { &tst_case_1_1_all, NULL, NULL }

#define test_group_1_2 "Reset of circuits"

#define tgrp_case_1_2_1 test_group_1_2
#define numb_case_1_2_1 "1.2.1"
#define name_case_1_2_1 "RSC received on an idle circuit"
#define xtra_case_1_2_1 NULL
#define sref_case_1_2_1 NULL
#define desc_case_1_2_1 "\
Verify that on receipt of a reset circuit message, SP A will respond by sending\n\
a release complete message."
static int
test_case_1_2_1(void)
{
	state = 0;
	pmsg.cic = 12;
	send(RSC);
	state = 1;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(RESET_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_2 test_group_1_2
#define numb_case_1_2_2 "1.2.2"
#define desc_case_1_2_2 "\
Reset of circuits\n\
-- RSC sent on an idle circuit\n\
Verify that SP A is able to generate a reset-circuit message."
static int
test_case_1_2_2(void)
{
	state = 0;
	mprim.flags = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(isup_addr_t);
	mprim.opt.len = 0;
	iut_mgm_signal(RESET_REQ);
	state = 1;
	if (pt_event() != RSC)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = 12;
	send(RLC);
	state = 3;
	if (mgm_event() != MGM_RESET_CON)
		return __RESULT_FAILURE;
	state = 4;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_3 test_group_1_2
#define numb_case_1_2_3 "1.2.3"
#define desc_case_1_2_3 "\
Reset of circuits\n\
-- RSC received on a locally blocked circuit\n\
Verify that on receipt of a reset circuit message while in its locally blocked\n\
state, SP A will respond by sending blocking and release complete messages."
static int
test_case_1_2_3(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 1;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = 12;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 4;
	pmsg.cic = 12;
	send(RSC);
	state = 5;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 6;
	iut_mgm_signal(RESET_RES);
	state = 7;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 9;
	pmsg.cic = 12;
	send(BLA);
	state = 10;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_4 test_group_1_2
#define numb_case_1_2_4 "1.2.4"
#define desc_case_1_2_4 "\
Reset of circuits\n\
-- RSC received on a remotely blocked circuit\n\
Verify that SP A is able to react to a reset-circuit message for a remotely\n\
blocked circuit."
static int
test_case_1_2_4(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 5;
	pmsg.cic = 12;
	send(RSC);
	state = 6;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 7;
	iut_mgm_signal(RESET_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 11;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_5a test_group_1_2
#define numb_case_1_2_5a "1.2.5a"
#define desc_case_1_2_5a "\
Reset of circuits\n\
-- Circuit group reset received\n\
-- (normal GRS)\n\
Verify that on receipt of one circuit group reset message SP A will respond by\n\
sending a circuit group reset acknowledgement."
static int
test_case_1_2_5a(void)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.len = 1;
	send(GRS);
	state = 1;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(RESET_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != GRA)
		return __RESULT_FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 6;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_5b test_group_1_2
#define numb_case_1_2_5b "1.2.5b"
#define desc_case_1_2_5b "\
Reset of circuits\n\
-- Circuit group reset received\n\
-- (GRS with range = 0)\n\
Verify that on receipt of one circuit group reset message SP A will respond by\n\
sending a circuit group reset acknowledgement."
static int
test_case_1_2_5b(void)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 0;
	pmsg.rs.len = 1;
	send(GRS);
	state = 1;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_5c test_group_1_2
#define numb_case_1_2_5c "1.2.5c"
#define desc_case_1_2_5c "\
Reset of circuits\n\
-- Circuit group reset received\n\
-- (GRS with range = 32)\n\
Verify that on receipt of one circuit group reset message SP A will respond by\n\
sending a circuit group reset acknowledgement."
static int
test_case_1_2_5c(void)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 32;
	pmsg.rs.len = 1;
	send(GRS);
	state = 1;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_6 test_group_1_2
#define numb_case_1_2_6 "1.2.6"
#define desc_case_1_2_6 "\
Reset of circuits\n\
-- Circuit group reset sent\n\
Verify that SP A is able to generate a circuit group reset message."
static int
test_case_1_2_6(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CG;
	mprim.addr.add.id = 1;
	mprim.addr.add.cic = 1;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_GROUP;
	iut_mgm_signal(RESET_REQ);
	state = 1;
	if (pt_event() != GRS)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = imsg.rs.buf[0];
	pmsg.rs.buf[1] = 0x00;
	pmsg.rs.buf[2] = 0x00;
	pmsg.rs.buf[3] = 0x00;
	pmsg.rs.buf[4] = 0x00;
	pmsg.rs.len = 5;
	send(GRA);
	state = 3;
	if (mgm_event() != MGM_RESET_CON)
		return __RESULT_FAILURE;
	state = 4;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_2_7 test_group_1_2
#define numb_case_1_2_7 "1.2.7"
#define desc_case_1_2_7 "\
Reset of circuits\n\
-- Circuit group reset received on remotely blocked circuits\n\
Verify that SP A is able to react to a circuit group reset message correctly\n\
for remotely blocked circuits."
static int
test_case_1_2_7(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 5;
	pmsg.cic = 13;
	send(BLO);
	state = 6;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 7;
	iut_mgm_signal(BLOCKING_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 10;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.len = 1;
	send(GRS);
	state = 11;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 12;
	iut_mgm_signal(RESET_RES);
	state = 13;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 14;
	if (pt_event() != GRA)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define desc_case_1_3_1_1x "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- normal range\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1x(ulong cgi)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.buf[1] = 0xff;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0xff;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.len = 5;
	pmsg.cgi = cgi;
	send(CGB);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != CGBA)
		return __RESULT_FAILURE;
	state = 5;
	/* FIXME: In here we should really attempt to launch some calls to the blocked circuits */
	state = 6;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.buf[1] = 0xff;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0xff;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.len = 5;
	pmsg.cgi = cgi;
	send(CGU);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 10;
	if (pt_event() != CGUA)
		return __RESULT_FAILURE;
	/* FIXME: In here we should really attempt to launch some calls to the blocked circuits */
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define desc_case_1_3_1_1y "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- range = 0\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1y(ulong cgi)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 0;
	pmsg.rs.len = 1;
	pmsg.cgi = cgi;
	send(CGB);
	state = 1;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 0;
	pmsg.rs.len = 1;
	pmsg.cgi = cgi;
	send(CGU);
	state = 3;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define desc_case_1_3_1_1z "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- range > 32\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1z(ulong cgi)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 34;
	pmsg.rs.buf[1] = 0x7f;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0x7f;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.buf[5] = 0x07;
	pmsg.rs.len = 6;
	pmsg.cgi = cgi;
	send(CGB);
	state = 1;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 34;
	pmsg.rs.buf[1] = 0x7f;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0x7f;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.buf[5] = 0x07;
	pmsg.rs.len = 6;
	pmsg.cgi = cgi;
	send(CGU);
	state = 3;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_1_3 "Circuit group blocking/unblocking"
#define test_group_1_3_1_1 "CGB and CGU received"

#define tgrp_case_1_3_1_1a test_group_1_3
#define sgrp_case_1_3_1_1a test_group_1_3_1_1
#define numb_case_1_3_1_1a "1.3.1.1a"
#define name_case_1_3_1_1a "Maintenance Oriented"
#define xtra_case_1_3_1_1a "valid range"
#define sref_case_1_3_1_1a NULL
#define desc_case_1_3_1_1a "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1a(void)
{
	return test_case_1_3_1_1x(ISUP_MAINTENANCE_ORIENTED);
}

#define tgrp_case_1_3_1_1b test_group_1_3
#define sgrp_case_1_3_1_1b test_group_1_3_1_1
#define numb_case_1_3_1_1b "1.3.1.1b"
#define name_case_1_3_1_1b "Maintenance Oriented"
#define xtra_case_1_3_1_1b "range = 0"
#define sref_case_1_3_1_1b NULL
#define desc_case_1_3_1_1b "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1b(void)
{
	return test_case_1_3_1_1y(ISUP_MAINTENANCE_ORIENTED);
}

#define tgrp_case_1_3_1_1c test_group_1_3
#define sgrp_case_1_3_1_1c test_group_1_3_1_1
#define numb_case_1_3_1_1c "1.3.1.1c"
#define name_case_1_3_1_1c "Maintenance Oriented"
#define xtra_case_1_3_1_1c "range > 32"
#define sref_case_1_3_1_1c NULL
#define desc_case_1_3_1_1c "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1c(void)
{
	return test_case_1_3_1_1z(ISUP_MAINTENANCE_ORIENTED);
}

#define tgrp_case_1_3_1_1d test_group_1_3
#define sgrp_case_1_3_1_1d test_group_1_3_1_1
#define numb_case_1_3_1_1d "1.3.1.1d"
#define name_case_1_3_1_1d "Hardware Failure Oriented"
#define xtra_case_1_3_1_1d "valid range"
#define sref_case_1_3_1_1d NULL
#define desc_case_1_3_1_1d "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1d(void)
{
	return test_case_1_3_1_1x(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define tgrp_case_1_3_1_1e test_group_1_3
#define sgrp_case_1_3_1_1e test_group_1_3_1_1
#define numb_case_1_3_1_1e "1.3.1.1e"
#define name_case_1_3_1_1e "Hardware Failure Oriented"
#define xtra_case_1_3_1_1e "range = 0"
#define sref_case_1_3_1_1e NULL
#define desc_case_1_3_1_1e "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1e(void)
{
	return test_case_1_3_1_1y(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define tgrp_case_1_3_1_1f test_group_1_3
#define sgrp_case_1_3_1_1f test_group_1_3_1_1
#define numb_case_1_3_1_1f "1.3.1.1f"
#define name_case_1_3_1_1f "Hardware Failure Oriented"
#define xtra_case_1_3_1_1f "range > 32"
#define sref_case_1_3_1_1f NULL
#define desc_case_1_3_1_1f "\
Verify that the circuit group blocking feature can be correctly initiated."

static int
test_case_1_3_1_1f(void)
{
	return test_case_1_3_1_1z(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define test_group_1_3_1_2 "CGB anbd CGU sent"

#define desc_case_1_3_1_2x "\
Circuit group blocking/unblocking\n\
-- CGB and CGU sent\n\
Verify that SP A is able to generate one circuit group blocking message and\n\
one circuit group unblocking message."
static int
test_case_1_3_1_2x(ulong cgi)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CG;
	mprim.addr.add.id = 1;
	mprim.addr.add.cic = 1;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_GROUP | cgi;
	iut_mgm_signal(BLOCKING_REQ);
	state = 1;
	if (pt_event() != CGB)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGBA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 4;
	mprim.addr.add.scope = ISUP_SCOPE_CG;
	mprim.addr.add.id = 1;
	mprim.addr.add.cic = 1;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_GROUP | cgi;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 5;
	if (pt_event() != CGU)
		return __RESULT_FAILURE;
	state = 6;
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGUA);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return __RESULT_FAILURE;
	state = 8;
	/* make sure both SP's can initiate calls */
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_3_1_2a test_group_1_3
#define sgrp_case_1_3_1_2a test_group_1_3_1_2
#define numb_case_1_3_1_2a "1.3.1.2a"
#define name_case_1_3_1_2a "Maintenance oriented"
#define xtra_case_1_3_1_2a NULL
#define sref_case_1_3_1_2a NULL
#define desc_case_1_3_1_2a "\
Verify that SP A is able to generate one circuit group blocking message and\n\
one circuit group unblocking message."

static int
test_case_1_3_1_2a(void)
{
	return test_case_1_3_1_2x(ISUP_MAINTENANCE_ORIENTED);
}

#define tgrp_case_1_3_1_2b test_group_1_3
#define sgrp_case_1_3_1_2b test_group_1_3_1_2
#define numb_case_1_3_1_2b "1.3.1.2b"
#define name_case_1_3_1_2b "Hardware failure oriented"
#define xtra_case_1_3_1_2b NULL
#define sref_case_1_3_1_2b NULL
#define desc_case_1_3_1_2b "\
Verify that SP A is able to generate one circuit group blocking message and\n\
one circuit group unblocking message."
static int
test_case_1_3_1_2b(void)
{
	return test_case_1_3_1_2x(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define tgrp_case_1_3_2_1 test_group_1_3
#define sgrp_case_1_3_2_1 NULL
#define numb_case_1_3_2_1 "1.3.2.1"
#define name_case_1_3_2_1 "BLO received"
#define xtra_case_1_3_2_1 NULL
#define sref_case_1_3_2_1 NULL
#define desc_case_1_3_2_1 "\
Verify that the blocking/unblocking procedure can be correctly initiated."

static int
test_case_1_3_2_1(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 5;
	/* FIXME: Make sure that a call cannot be made */
	state = 6;
	pmsg.cic = 12;
	send(UBL);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 10;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 11;
	/* FIXME: Make sure that a call can be made */
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_3_2_2 test_group_1_3
#define sgrp_case_1_3_2_2 NULL
#define numb_case_1_3_2_2 "1.3.2.2"
#define name_case_1_3_2_2 "BLO sent"
#define xtra_case_1_3_2_2 NULL
#define sref_case_1_3_2_2 NULL
#define desc_case_1_3_2_2 "\
Verify that SP A is able to generate blocking messages."
static int
test_case_1_3_2_2(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 1;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 4;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 5;
	if (pt_event() != UBL)
		return __RESULT_FAILURE;
	state = 6;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return __RESULT_FAILURE;
	state = 8;
	/* make sure that both SP's can initiate calls */
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_3_2_3 test_group_1_3
#define sgrp_case_1_3_2_3 NULL
#define numb_case_1_3_2_3 "1.3.2.3"
#define name_case_1_3_2_3 "Blocking from both ends; removal of blocking from one end."
#define xtra_case_1_3_2_3 NULL
#define sref_case_1_3_2_3 NULL
#define desc_case_1_3_2_3 "\
Verify that the blocking/unblocking procedure can be correctly initiated."
static int
test_case_1_3_2_3(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 1;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 4;
	pmsg.cic = 12;
	send(BLO);
	state = 5;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 6;
	iut_mgm_signal(BLOCKING_RES);
	state = 7;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 9;
	/* FIXME: Make sure that a call cannot be made */
	state = 10;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 11;
	if (pt_event() != UBL)
		return __RESULT_FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 13;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return __RESULT_FAILURE;
	state = 14;
	/* FIXME: make sure that both SP's can initiate calls */
	state = 15;
	pmsg.cic = 12;
	send(UBL);
	state = 16;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 17;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 18;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 19;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 20;
	/* Make sure that a call can be made */
	state = 21;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_3_2_4 test_group_1_3
#define sgrp_case_1_3_2_4 NULL
#define numb_case_1_3_2_4 "1.3.2.4"
#define name_case_1_3_2_4 "IAM received on a remotely blocked circuit"
#define xtra_case_1_3_2_4 NULL
#define sref_case_1_3_2_4 NULL
#define desc_case_1_3_2_4 "\
Verify that an IAM will unblock a remotely blocked circuit."

static int
test_case_1_3_2_4(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 5;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 6;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(SETUP_RES);
	state = 11;
	if (cpc_event() != CPC_OK_ACK)
		return (__RESULT_FAILURE);
	state = 12;
	cprim.flags = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 13;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 14;
	iut_cpc_signal(RINGING);
	state = 15;
	iut_cpc_signal(CONNECT_REQ);
	state = 16;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 17;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 18;
	iut_cpc_signal(COMMUNICATION);
	state = 19;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 20;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 21;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 22;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 23;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_3_2_5 test_group_1_3
#define sgrp_case_1_3_2_5 NULL
#define numb_case_1_3_2_5 "1.3.2.5"
#define name_case_1_3_2_5 "Blocking with CGB, unblocking with UBL"
#define xtra_case_1_3_2_5 NULL
#define sref_case_1_3_2_5 NULL
#define desc_case_1_3_2_5 "\
Verify that a circuit blocked by a maintenance oriented group blocking message\n\
can be unblocked with an unblocking message."
static int
test_case_1_3_2_5(void)
{
	state = 0;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.buf[1] = 0xff;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0xff;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.len = 5;
	pmsg.cgi = ISUP_MAINTENANCE_ORIENTED;
	send(CGB);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != CGBA)
		return __RESULT_FAILURE;
	state = 5;
	pmsg.cic = 12;
	send(UBL);
	state = 6;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 7;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 10;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 11;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 13;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 14;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 15;
	send(RINGING);
	state = 16;
	send(ANM);
	state = 17;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 18;
	iut_cpc_signal(COMMUNICATION);
	state = 19;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 20;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 22;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 23;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.buf[1] = 0xff;
	pmsg.rs.buf[2] = 0xff;
	pmsg.rs.buf[3] = 0xff;
	pmsg.rs.buf[4] = 0xff;
	pmsg.rs.len = 5;
	pmsg.cgi = ISUP_MAINTENANCE_ORIENTED;
	send(CGU);
	state = 24;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 25;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 26;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 27;
	if (pt_event() != CGUA)
		return __RESULT_FAILURE;
	state = 28;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_1_4 "Continuity check procedure"

#define tgrp_case_1_4_1 test_group_1_4
#define sgrp_case_1_4_1 NULL
#define numb_case_1_4_1 "1.4.1"
#define name_case_1_4_1 "CCR received: successful"
#define xtra_case_1_4_1 NULL
#define sref_case_1_4_1 NULL
#define desc_case_1_4_1 "\
Verify that the continuity check procedure for the proper alignment of\n\
circuits can be correctly performed."
static int
test_case_1_4_1(void)
{
	state = 0;
	pmsg.cic = 12;
	send(CCR);
	state = 1;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return __RESULT_FAILURE;
	state = 2;
	mprim.token_value = 0;
	iut_tst_signal(LOOPBACK);
	state = 3;
	iut_tst_signal(CONT_TEST_REQ);
	state = 4;
	send(TONE);
	state = 5;
	if (wait_event(100, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 6;
	pmsg.cic = 12;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 7;
	if (tst_event() != MGM_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 8;
	mprim.addr.len = 0;
	mprim.opt.len = 0;
	mprim.user_ref = 0;
	mprim.user_ref = 0;
	iut_tst_signal(RELEASE_RES);
	if (tst_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_4_2 test_group_1_4
#define sgrp_case_1_4_2 NULL
#define numb_case_1_4_2 "1.4.2"
#define name_case_1_4_2 "CCR sent: successful"
#define xtra_case_1_4_2 NULL
#define sref_case_1_4_2 NULL
#define desc_case_1_4_2 "\
Verify that the continuity check procedure for the proper alignment of circuits\n\
can be correctly performed."
static int
test_case_1_4_2(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.opt.len = 0;
	iut_tst_signal(CONT_CHECK_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	start_st(timer[t24].lo / 2);
	iut_tst_signal(TONE);
	state = 5;
	if (any_event() != __EVENT_TIMEOUT)
		return __RESULT_FAILURE;
	state = 6;
	start_tt(100);
	mprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	mprim.user_ref = 0;
	iut_tst_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (tst_event() != MGM_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_4_3 test_group_1_4
#define sgrp_case_1_4_3 NULL
#define numb_case_1_4_3 "1.4.3"
#define name_case_1_4_3 "CCR received: unsuccessful"
#define xtra_case_1_4_3 NULL
#define sref_case_1_4_3 NULL
#define desc_case_1_4_3 "\
Verify that the messages associated with continuity check procedures for a\n\
proper alignment of circuits can be correctly received."

static int
test_case_1_4_3(void)
{
	state = 0;
	pmsg.cic = 12;
	send(CCR);
	state = 1;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return __RESULT_FAILURE;
	state = 2;
	mprim.token_value = 0;
	iut_tst_signal(LOOPBACK);
	state = 3;
	iut_tst_signal(CONT_TEST_REQ);
	state = 4;
	send(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	state = 6;
	start_st(timer[t24].lo);
	if (any_event() != __EVENT_TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return __RESULT_FAILURE;
	}
	start_tt(100);
	state = 7;
	pmsg.coti = ISUP_COT_FAILURE;
	send(COT);
	state = 8;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 9;
	beg_time = milliseconds(" T26  ");
	state = 10;
	start_st(timer[t26].lo);
	if (any_event() != __EVENT_TIMEOUT) {
		check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
		return __RESULT_FAILURE;
	}
	start_tt(100);
	state = 11;
	send(CCR);
	state = 12;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return __RESULT_FAILURE;
	state = 13;
	mprim.token_value = 0;
	iut_tst_signal(LOOPBACK);
	state = 14;
	iut_tst_signal(CONT_TEST_REQ);
	state = 15;
	send(TONE);
	state = 16;
	beg_time = milliseconds(" T24  ");
	state = 17;
	start_st(timer[t24].lo);
	if (any_event() != __EVENT_TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return __RESULT_FAILURE;
	}
	start_tt(100);
	state = 18;
	pmsg.coti = ISUP_COT_FAILURE;
	send(COT);
	state = 19;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 20;
	beg_time = milliseconds(" T26  ");
	state = 21;
	start_st(timer[t26].lo);
	if (any_event() != __EVENT_TIMEOUT) {
		check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
		return __RESULT_FAILURE;
	}
	start_tt(100);
	state = 22;
	send(CCR);
	state = 23;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return __RESULT_FAILURE;
	state = 24;
	mprim.token_value = 0;
	iut_tst_signal(LOOPBACK);
	state = 25;
	iut_tst_signal(CONT_TEST_REQ);
	state = 26;
	send(TONE);
	state = 27;
	beg_time = milliseconds(" T24  ");
	state = 28;
	start_st(timer[t24].lo);
	if (any_event() != __EVENT_TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return __RESULT_FAILURE;
	}
	start_tt(100);
	state = 29;
	pmsg.coti = ISUP_COT_SUCCESS;
	send(COT);
	state = 30;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 31;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_4_4 test_group_1_4
#define sgrp_case_1_4_4 NULL
#define numb_case_1_4_4 "1.4.4"
#define name_case_1_4_4 "CCR sent: unsuccessful"
#define xtra_case_1_4_4 NULL
#define sref_case_1_4_4 NULL
#define desc_case_1_4_4 "\
Verify that the continuity check procedure for the proper alignment of\n\
circuits can be correctly invoked."
static int
test_case_1_4_4(void)
{
	state = 0;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.opt.len = 0;
	iut_tst_signal(CONT_CHECK_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_tst_signal(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	state = 6;
	start_st(timer[t24].hi + 100);
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 7;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 8;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 9;
	beg_time = milliseconds(" T26  ");
	state = 10;
	start_st(timer[t26].hi + 100);
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 11;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 12;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 13;
	check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
	state = 14;
	send(LOOPBACK);
	state = 15;
	iut_tst_signal(TONE);
	state = 16;
	beg_time = milliseconds(" T24  ");
	state = 17;
	start_st(timer[t24].hi + 100);
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 18;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 19;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 20;
	beg_time = milliseconds(" T26  ");
	state = 21;
	start_st(timer[t26].hi + 100);
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 22;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 23;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 24;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.opt.len = 0;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 25;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 26;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_1_4_5 test_group_1_4
#define sgrp_case_1_4_5 NULL
#define numb_case_1_4_5 "1.4.5"
#define name_case_1_4_5 "CCR not received unsuccessful; verify T27 timer"
#define xtra_case_1_4_5 NULL
#define sref_case_1_4_5 NULL
#define desc_case_1_4_5 "\
Verify that the continuity check procedure for the proper alignment of\n\
circuits can be correctly received."
static int
test_case_1_4_5(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_REQUIRED;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	send(TONE);
	state = 2;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 3;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return __RESULT_FAILURE;
	state = 4;
	iut_tst_signal(LOOPBACK);
	state = 5;
	iut_tst_signal(CONT_TEST_REQ);
	state = 6;
	pmsg.coti = ISUP_COT_FAILURE;
	pmsg.opt.len = 0;
	send(COT);
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 8;
	beg_time = milliseconds(" T27  ");
	state = 9;
	start_st(timer[t27].hi + 100);
	if (pt_event() != RSC)
		return __RESULT_FAILURE;
	state = 10;
	if (check_time(" T27  ", now() - beg_time, timer[t27].lo, timer[t27].hi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	start_tt(1000);
	state = 11;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 12;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 13;
	send(RLC);
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_1_5 "Receipt of unreasonable signalling information messages"

#define tgrp_case_1_5_1 test_group_1_5
#define sgrp_case_1_5_1 NULL
#define numb_case_1_5_1 "1.5.1"
#define name_case_1_5_1 "Receipt of unexpected messages"
#define xtra_case_1_5_1 NULL
#define sref_case_1_5_1 NULL
#define desc_case_1_5_1 "\
Verify that SP A is able to handle unexpected messages."
static int
test_case_1_5_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_5_2 test_group_1_5
#define sgrp_case_1_5_2 NULL
#define numb_case_1_5_2 "1.5.2"
#define name_case_1_5_2 "Receipt of unexpected messages during call setup"
#define xtra_case_1_5_2 NULL
#define sref_case_1_5_2 NULL
#define desc_case_1_5_2 "\
Verify that SP A is able to handle unexpected emssages."

static int
test_case_1_5_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_5_3 test_group_1_5
#define sgrp_case_1_5_3 NULL
#define numb_case_1_5_3 "1.5.3"
#define name_case_1_5_3 "Receipt of unexpected messages during a call"
#define xtra_case_1_5_3 NULL
#define sref_case_1_5_3 NULL
#define desc_case_1_5_3 "\
Verify that SP A is able to handle unexpected emssages."
static int
test_case_1_5_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_6_1 "Receipt of unknown messages"

#define tgrp_case_1_6_1_1 test_group_1_6_1
#define sgrp_case_1_6_1_1 NULL
#define numb_case_1_6_1_1 "1.6.1.1"
#define name_case_1_6_1_1 "Receipt of unknown message in forward direction"
#define xtra_case_1_6_1_1 NULL
#define sref_case_1_6_1_1 NULL
#define desc_case_1_6_1_1 "\
Verify that SP A is able to discard an unknown message without disrupting\n\
normal call handling."
static int
test_case_1_6_1_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_6_1_2 test_group_1_6_1
#define sgrp_case_1_6_1_2 NULL
#define numb_case_1_6_1_2 "1.6.1.2"
#define name_case_1_6_1_2 "Receipt of unknown message in backward direction"
#define xtra_case_1_6_1_2 NULL
#define sref_case_1_6_1_2 NULL
#define desc_case_1_6_1_2 "\
Verify that SP A is able to discard an unknown message without disrupting\n\
normal call handling."
static int
test_case_1_6_1_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_6_2 "Receipt of unknown parameters"

#define tgrp_case_1_6_2_1 test_group_1_6_2
#define sgrp_case_1_6_2_1 NULL
#define numb_case_1_6_2_1 "1.6.2.1"
#define name_case_1_6_2_1 "Receipt of unknown parameters in forward direction"
#define xtra_case_1_6_2_1 NULL
#define sref_case_1_6_2_1 NULL
#define desc_case_1_6_2_1 "\
Verify that SP A is able to discard an unknown parameter without disrupting\n\
normal call handling."
static int
test_case_1_6_2_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_6_2_2 test_group_1_6_2
#define sgrp_case_1_6_2_2 NULL
#define numb_case_1_6_2_2 "1.6.2.2"
#define name_case_1_6_2_2 "Receipt of unknown parameters in backward direction"
#define xtra_case_1_6_2_2 NULL
#define sref_case_1_6_2_2 NULL
#define desc_case_1_6_2_2 "\
Verify that SP A is able to discard an unknown parameter without disrupting\n\
normal call handling."
static int
test_case_1_6_2_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_6_3 "Receipt of unknown parameter values"

#define tgrp_case_1_6_3_1 test_group_1_6_3
#define sgrp_case_1_6_3_1 NULL
#define numb_case_1_6_3_1 "1.6.3.1"
#define name_case_1_6_3_1 "Receipt of unknown parameter values in forward direction"
#define xtra_case_1_6_3_1 NULL
#define sref_case_1_6_3_1 NULL
#define desc_case_1_6_3_1 "\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_6_3_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_6_3_2 test_group_1_6_3
#define sgrp_case_1_6_3_2 NULL
#define numb_case_1_6_3_2 "1.6.3.2"
#define name_case_1_6_3_2 "Receipt of unknown parameter values in backward direction"
#define xtra_case_1_6_3_2 NULL
#define sref_case_1_6_3_2 NULL
#define desc_case_1_6_3_2 "\
Verify that SP A is able to handle unknown parameter values."

static int
test_case_1_6_3_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_7_1 "Receipt of unknown messages"
#define test_group_1_7_1_x "Message Compatibility Information"

#define tgrp_case_1_7_1_1 test_group_1_7_1
#define sgrp_case_1_7_1_1 test_group_1_7_1_x
#define numb_case_1_7_1_1 "1.7.1.1"
#define name_case_1_7_1_1 "Release call"
#define xtra_case_1_7_1_1 NULL
#define sref_case_1_7_1_1 NULL
#define desc_case_1_7_1_1 "\
Verify that SP A release the call, if indicated in the Message Compatibility\n\
Information."

static int
test_case_1_7_1_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_2 test_group_1_7_1
#define sgrp_case_1_7_1_2 test_group_1_7_1_x
#define numb_case_1_7_1_2 "1.7.1.2"
#define name_case_1_7_1_2 "Discard message"
#define xtra_case_1_7_1_2 NULL
#define sref_case_1_7_1_2 NULL
#define desc_case_1_7_1_2 "\
Verify that SP A is able to discard an unknown message, if indicated in the\n\
Message Compatibility Information."
static int
test_case_1_7_1_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_3 test_group_1_7_1
#define sgrp_case_1_7_1_3 test_group_1_7_1_x
#define numb_case_1_7_1_3 "1.7.1.3"
#define name_case_1_7_1_3 "Pass on"
#define xtra_case_1_7_1_3 NULL
#define sref_case_1_7_1_3 NULL
#define desc_case_1_7_1_3 "\
Verify that SP A is able to pass on an unknown message, without notification."
static int
test_case_1_7_1_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_4 test_group_1_7_1
#define sgrp_case_1_7_1_4 test_group_1_7_1_x
#define numb_case_1_7_1_4 "1.7.1.4"
#define name_case_1_7_1_4 "Pass on not possible, release call"
#define xtra_case_1_7_1_4 NULL
#define sref_case_1_7_1_4 NULL
#define desc_case_1_7_1_4 "\
Verify that SP A release the call if pass on not possible and if indicated in\n\
the Message Compatibility Information."
static int
test_case_1_7_1_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_5 test_group_1_7_1
#define sgrp_case_1_7_1_5 test_group_1_7_1_x
#define numb_case_1_7_1_5 "1.7.1.5"
#define name_case_1_7_1_5 "Pass on not possible, discard information"
#define xtra_case_1_7_1_5 NULL
#define sref_case_1_7_1_5 NULL
#define desc_case_1_7_1_5 "\
Verify that SP A is able to discard an unknown message if pass on not possible\n\
and if indicated in the Message Compatibility Information."
static int
test_case_1_7_1_5(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_6 test_group_1_7_1
#define sgrp_case_1_7_1_6 test_group_1_7_1_x
#define numb_case_1_7_1_6 "1.7.1.6"
#define name_case_1_7_1_6 "Transit interpretation"
#define xtra_case_1_7_1_6 NULL
#define sref_case_1_7_1_6 NULL
#define desc_case_1_7_1_6 "\
Verify that SP A (Type B exchange) is able to ignore the remaining part of the\n\
Instruction indicator, if A = 0."
static int
test_case_1_7_1_6(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_1_7 test_group_1_7_1
#define sgrp_case_1_7_1_7 test_group_1_7_1_x
#define numb_case_1_7_1_7 "1.7.1.7"
#define name_case_1_7_1_7 "not received"
#define xtra_case_1_7_1_7 NULL
#define sref_case_1_7_1_7 NULL
#define desc_case_1_7_1_7 "\
Verify that SP A is able to discard an unknown message without Message\n\
Compatibility Information."
static int
test_case_1_7_1_7(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_7_2 "Receipt of unknown parameters"
#define test_group_1_7_2_x "Parameter Compatibility Information"

#define tgrp_case_1_7_2_1 test_group_1_7_2
#define sgrp_case_1_7_2_1 test_group_1_7_2_x
#define numb_case_1_7_2_1 "1.7.2.1"
#define name_case_1_7_2_1 "Release call"
#define xtra_case_1_7_2_1 NULL
#define sref_case_1_7_2_1 NULL
#define desc_case_1_7_2_1 "\
Verify that SP A is able to release the call, if indicated in Parameter\n\
Compatibility Information."

static int
test_case_1_7_2_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_2 test_group_1_7_2
#define sgrp_case_1_7_2_2 test_group_1_7_2_x
#define numb_case_1_7_2_2 "1.7.2.2"
#define name_case_1_7_2_2 "Discard message"
#define xtra_case_1_7_2_2 NULL
#define sref_case_1_7_2_2 NULL
#define desc_case_1_7_2_2 "\
Verify that SP A is able to discard a message containing an unknown parameter,\n\
if indicated in the Parameter Compatibility Information."
static int
test_case_1_7_2_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_3 test_group_1_7_2
#define sgrp_case_1_7_2_3 test_group_1_7_2_x
#define numb_case_1_7_2_3 "1.7.2.3"
#define name_case_1_7_2_3 "Discard parameter"
#define xtra_case_1_7_2_3 NULL
#define sref_case_1_7_2_3 NULL
#define desc_case_1_7_2_3 "\
Verify that SP A is able to discard an unknown parameter, if indicated in the\n\
Parameter Compatibility Information."
static int
test_case_1_7_2_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_4 test_group_1_7_2
#define sgrp_case_1_7_2_4 test_group_1_7_2_x
#define numb_case_1_7_2_4 "1.7.2.4"
#define name_case_1_7_2_4 "Pass on"
#define xtra_case_1_7_2_4 NULL
#define sref_case_1_7_2_4 NULL
#define desc_case_1_7_2_4 "\
Verify that SP A is able to pass on an unknown parameter, without\n\
notification."
static int
test_case_1_7_2_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_5 test_group_1_7_2
#define sgrp_case_1_7_2_5 test_group_1_7_2_x
#define numb_case_1_7_2_5 "1.7.2.5"
#define name_case_1_7_2_5 "Pass on not possisble, release call"
#define xtra_case_1_7_2_5 NULL
#define sref_case_1_7_2_5 NULL
#define desc_case_1_7_2_5 "\
Verify that SP A releases call if pass on is not possible and if indicated in\n\
Parameter Compatibility Information."
static int
test_case_1_7_2_5(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_6 test_group_1_7_2
#define sgrp_case_1_7_2_6 test_group_1_7_2_x
#define numb_case_1_7_2_6 "1.7.2.6"
#define name_case_1_7_2_6 "Pass on not possisble, discard message"
#define xtra_case_1_7_2_6 NULL
#define sref_case_1_7_2_6 NULL
#define desc_case_1_7_2_6 "\
Verify that SP A is able to discard a message containing an unknown parameter\n\
if pass on is not possible and if indicated in Parameter Compatibility\n\
Information."
static int
test_case_1_7_2_6(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_7 test_group_1_7_2
#define sgrp_case_1_7_2_7 test_group_1_7_2_x
#define numb_case_1_7_2_7 "1.7.2.7"
#define name_case_1_7_2_7 "Pass on not possisble, discard parameter"
#define xtra_case_1_7_2_7 NULL
#define sref_case_1_7_2_7 NULL
#define desc_case_1_7_2_7 "\
Verify that SP A is able to discard an unknown parameter if pass on is not\n\
possible and if indicated in the Parameter Compatibility Information."
static int
test_case_1_7_2_7(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_8 test_group_1_7_2
#define sgrp_case_1_7_2_8 test_group_1_7_2_x
#define numb_case_1_7_2_8 "1.7.2.8"
#define name_case_1_7_2_8 "Transit interpretation"
#define xtra_case_1_7_2_8 NULL
#define sref_case_1_7_2_8 NULL
#define desc_case_1_7_2_8 "\
Verify that SP A (Type B exchange) is able to ignore the remaining part of the\n\
Instruction indicator, if A = 0."
static int
test_case_1_7_2_8(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_9 test_group_1_7_2
#define sgrp_case_1_7_2_9 test_group_1_7_2_x
#define numb_case_1_7_2_9 "1.7.2.9"
#define name_case_1_7_2_9 "not received"
#define xtra_case_1_7_2_9 NULL
#define sref_case_1_7_2_9 NULL
#define desc_case_1_7_2_9 "\
Verify that SP A is able to handle an unknown parameter without Parameter\n\
Compatibility Information."
static int
test_case_1_7_2_9(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_2_10 test_group_1_7_2
#define sgrp_case_1_7_2_10 test_group_1_7_2_x
#define numb_case_1_7_2_10 "1.7.2.10"
#define name_case_1_7_2_10 "not received in REL"
#define xtra_case_1_7_2_10 NULL
#define sref_case_1_7_2_10 NULL
#define desc_case_1_7_2_10 "\
Verify that SP A is able to discard an unknown parameter in a REL without\n\
Parameter Compatiblity Information."
static int
test_case_1_7_2_10(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_1_7_3 "Receipt of unknown parameter values"

#define tgrp_case_1_7_3_1 test_group_1_7_3
#define sgrp_case_1_7_3_1 NULL
#define numb_case_1_7_3_1 "1.7.3.1"
#define name_case_1_7_3_1 "Receipt of unknown parameter values in forward direction"
#define xtra_case_1_7_3_1 NULL
#define sref_case_1_7_3_1 NULL
#define desc_case_1_7_3_1 "\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_7_3_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_1_7_3_2 test_group_1_7_3
#define sgrp_case_1_7_3_2 NULL
#define numb_case_1_7_3_2 "1.7.3.2"
#define name_case_1_7_3_2 "Receipt of unknown parameter values in backward direction"
#define xtra_case_1_7_3_2 NULL
#define sref_case_1_7_3_2 NULL
#define desc_case_1_7_3_2 "\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_7_3_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_2_1 "Both way circuit selection"

#define tgrp_case_2_1_1 test_group_2_1
#define sgrp_case_2_1_1 NULL
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "IAM sent by controlling SP"
#define xtra_case_2_1_1 NULL
#define sref_case_2_1_1 NULL
#define desc_case_2_1_1 "\
Verify that SP A can initiate an outgoing call on a circuit capable of both\n\
way operation when the controlling SP is A."
static int
test_case_2_1_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x19;
	cprim.cdpn.buf[3] = 0xf1;
	cprim.cdpn.len = 4;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 4;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x19;
	cprim.opt.buf[5] = 0x01;
	cprim.opt.buf[6] = 0x00;
	cprim.opt.len = 7;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_1_2 test_group_2_1
#define sgrp_case_2_1_2 NULL
#define numb_case_2_1_2 "2.1.2"
#define name_case_2_1_2 "IAM sent by non-controlling SP"
#define xtra_case_2_1_2 NULL
#define sref_case_2_1_2 NULL
#define desc_case_2_1_2 "\
Verify that SP A can initiate an outgoing call on a circuit capable of both\n\
way operation when the non-controlling SP is A."
static int
test_case_2_1_2(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x19;
	cprim.cdpn.buf[3] = 0xf1;
	cprim.cdpn.len = 4;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 4;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x19;
	cprim.opt.buf[5] = 0x01;
	cprim.opt.buf[6] = 0x00;
	cprim.opt.len = 7;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 10;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 12;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_2 "Called address sending"
#define test_group_2_2_1 "\"en bloc\" operation"

#define tgrp_case_2_2_1a test_group_2_2
#define sgrp_case_2_2_1a test_group_2_2_1
#define numb_case_2_2_1a "2.2.1a"
#define name_case_2_2_1a "outgoing call"
#define xtra_case_2_2_1a NULL
#define sref_case_2_2_1a NULL
#define desc_case_2_2_1a "\
Verify that a call can be successfully establish (all digits included in the\n\
IAM)."
static int
test_case_2_2_1a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xF1;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_2_1b test_group_2_2
#define sgrp_case_2_2_1b test_group_2_2_1
#define numb_case_2_2_1b "2.2.1b"
#define name_case_2_2_1b "incoming call"
#define xtra_case_2_2_1b NULL
#define sref_case_2_2_1b NULL
#define desc_case_2_2_1b "\
Verify that a call can be successfully establish (all digits included in the\n\
IAM)."
static int
test_case_2_2_1b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_2_2 "Overlap operation (with SAM)"

#define tgrp_case_2_2_2a test_group_2_2
#define sgrp_case_2_2_2a test_group_2_2_2
#define numb_case_2_2_2a "2.2.2a"
#define name_case_2_2_2a "outgoing call"
#define xtra_case_2_2_2a NULL
#define sref_case_2_2_2a NULL
#define desc_case_2_2_2a "\
Verify that SP A can initiate a call using an IAM followed by a SAM."
static int
test_case_2_2_2a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.len = 4;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	if (cpc_event() != CPC_MORE_INFO_IND)
		return __RESULT_FAILURE;
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	cprim.subn.buf[0] = 0x00;
	cprim.subn.buf[1] = 0x94;
	cprim.subn.buf[2] = 0x10;
	cprim.subn.buf[3] = 0x41;
	cprim.subn.buf[4] = 0xf0;
	cprim.subn.len = 5;
	cprim.opt.len = 0;
	iut_cpc_signal(INFORMATION_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 3;
	if (pt_event() != SAM)
		return __RESULT_FAILURE;
	state = 4;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 5;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 6;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 7;
	send(RINGING);
	state = 8;
	send(ANM);
	state = 9;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 12;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 13;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 14;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_2_2b test_group_2_2
#define sgrp_case_2_2_2b test_group_2_2_2
#define numb_case_2_2_2b "2.2.2b"
#define name_case_2_2_2b "incoming call"
#define xtra_case_2_2_2b NULL
#define sref_case_2_2_2b NULL
#define desc_case_2_2_2b "\
Verify that SP A can initiate a call using an IAM followed by a SAM."
static int
test_case_2_2_2b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "1780", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	strncpy(pmsg.subn.num, "5551212/", 24);
	pmsg.subn.len = strnlen(pmsg.subn.num, 24);
	send(SAM);
	state = 5;
	if (cpc_event() != CPC_INFORMATION_IND)
		return __RESULT_FAILURE;
	state = 6;
	cprim.flags = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 7;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(RINGING);
	state = 9;
	iut_cpc_signal(CONNECT_REQ);
	state = 10;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 11;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 12;
	iut_cpc_signal(COMMUNICATION);
	state = 13;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 14;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 15;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_3 "Successful Call setup"
#define test_group_2_3_1 "Ordinary call (with various indications in the ACM)"

#define tgrp_case_2_3_1x test_group_2_3
#define sgrp_case_2_3_1x test_group_2_3_1
#define name_case_2_3_1x "outgoing call"
#define xtra_case_2_3_1x NULL
#define sref_case_2_3_1x NULL
#define desc_case_2_3_1x "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1x(ulong bci)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | bci;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	switch (cpc_event()) {
	case CPC_ALERTING_IND:
		state = 5;
		send(RINGING);
		state = 6;
		if (!(bci & ISUP_BCI_SUBSCRIBER_FREE))
			return __RESULT_FAILURE;
		state = 7;
		send(ANM);
		break;
	case CPC_PROCEEDING_IND:
		state = 6;
		if ((bci & ISUP_BCI_SUBSCRIBER_FREE))
			return __RESULT_FAILURE;
		state = 7;
		send(ANM);
		break;
	default:
		return __RESULT_FAILURE;
	}
	state = 8;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 9;
	iut_cpc_signal(COMMUNICATION);
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 11;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 13;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_1a test_group_2_3
#define sgrp_case_2_3_1a test_group_2_3_1
#define numb_case_2_3_1a "2.3.1a"
#define name_case_2_3_1a "outgoing call"
#define xtra_case_2_3_1a "\"subscriber free\" and \"terminating access isdn\""
#define sref_case_2_3_1a NULL
#define desc_case_2_3_1a "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1a(void)
{
	return test_case_2_3_1x(ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define tgrp_case_2_3_1b test_group_2_3
#define sgrp_case_2_3_1b test_group_2_3_1
#define numb_case_2_3_1b "2.3.1b"
#define name_case_2_3_1b "outgoing call"
#define xtra_case_2_3_1b "\"subscriber free\" and \"terminating access non-isdn\""
#define sref_case_2_3_1b NULL
#define desc_case_2_3_1b "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1b(void)
{
	return test_case_2_3_1x(ISUP_BCI_SUBSCRIBER_FREE);
}

#define tgrp_case_2_3_1c test_group_2_3
#define sgrp_case_2_3_1c test_group_2_3_1
#define numb_case_2_3_1c "2.3.1c"
#define name_case_2_3_1c "outgoing call"
#define xtra_case_2_3_1c "\"no indication\" and \"terminating access isdn\""
#define sref_case_2_3_1c NULL
#define desc_case_2_3_1c "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1c(void)
{
	return test_case_2_3_1x(ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define tgrp_case_2_3_1d test_group_2_3
#define sgrp_case_2_3_1d test_group_2_3_1
#define numb_case_2_3_1d "2.3.1d"
#define name_case_2_3_1d "outgoing call"
#define xtra_case_2_3_1d "\"no indication\" and \"terminating access non-isdn\""
#define sref_case_2_3_1d NULL
#define desc_case_2_3_1d "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1d(void)
{
	return test_case_2_3_1x(0);
}

#define desc_case_2_3_1y "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- incoming call\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1y(ulong bci)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = bci | ISUP_BCI_ORDINARY_SUBSCRIBER;
	cprim.opt.len = 0;
	if (bci & ISUP_BCI_SUBSCRIBER_FREE)
		iut_cpc_signal(ALERTING_REQ);
	else
		iut_cpc_signal(PROCEEDING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_1e test_group_2_3
#define sgrp_case_2_3_1e test_group_2_3_1
#define numb_case_2_3_1e "2.3.1e"
#define name_case_2_3_1e "incoming call"
#define xtra_case_2_3_1e "\"subscriber free\" and \"terminating access isdn\""
#define sref_case_2_3_1e NULL
#define desc_case_2_3_1e "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1e(void)
{
	return test_case_2_3_1y(ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define tgrp_case_2_3_1f test_group_2_3
#define sgrp_case_2_3_1f test_group_2_3_1
#define numb_case_2_3_1f "2.3.1f"
#define name_case_2_3_1f "incoming call"
#define xtra_case_2_3_1f "\"subscriber free\" and \"terminating access non-isdn\""
#define sref_case_2_3_1f NULL
#define desc_case_2_3_1f "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1f(void)
{
	return test_case_2_3_1y(ISUP_BCI_SUBSCRIBER_FREE);
}

#define tgrp_case_2_3_1g test_group_2_3
#define sgrp_case_2_3_1g test_group_2_3_1
#define numb_case_2_3_1g "2.3.1g"
#define name_case_2_3_1g "incoming call"
#define xtra_case_2_3_1g "\"no indication\" and \"terminating access isdn\""
#define sref_case_2_3_1g NULL
#define desc_case_2_3_1g "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1g(void)
{
	return test_case_2_3_1y(ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define tgrp_case_2_3_1h test_group_2_3
#define sgrp_case_2_3_1h test_group_2_3_1
#define numb_case_2_3_1h "2.3.1h"
#define name_case_2_3_1h "incoming call"
#define xtra_case_2_3_1h "\"no indication\" and \"terminating access non-isdn\""
#define sref_case_2_3_1h NULL
#define desc_case_2_3_1h "\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1h(void)
{
	return test_case_2_3_1y(0);
}

#define test_group_2_3_2 "Ordinary call (with ACM, CPG, and ANM)"

#define tgrp_case_2_3_2x test_group_2_3
#define sgrp_case_2_3_2x test_group_2_3_2
#define name_case_2_3_2x "outgoing call"
#define xtra_case_2_3_2x NULL
#define sref_case_2_3_2x NULL
#define desc_case_2_3_2x "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2x(ulong evnt)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ((evnt != ISUP_EVNT_PROGRESS) ? 0 : ISUP_BCI_SUBSCRIBER_FREE);
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	pmsg.cic = imsg.cic;
	pmsg.evnt = evnt;
	send(CPG);
	state = 5;
	for (;;) {
		switch (cpc_event()) {
		case CPC_PROCEEDING_IND:
			continue;
		case CPC_ALERTING_IND:
			send(RINGING);
			state = 6;
			if (evnt != ISUP_EVNT_ALERTING)
				continue;
		case CPC_PROGRESS_IND:
			break;
		case CPC_IBI_IND:
			state = 6;
			send(IBI);
			break;
		default:
			return __RESULT_FAILURE;
		}
		break;
	}
	state = 7;
	send(ANM);
	state = 8;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 9;
	iut_cpc_signal(COMMUNICATION);
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 11;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 13;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_2a test_group_2_3
#define sgrp_case_2_3_2a test_group_2_3_2
#define numb_case_2_3_2a "2.3.2a"
#define name_case_2_3_2a "outgoing call"
#define xtra_case_2_3_2a "\"alerting\""
#define sref_case_2_3_2a NULL
#define desc_case_2_3_2a "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2a(void)
{
	return test_case_2_3_2x(ISUP_EVNT_ALERTING);
}

#define tgrp_case_2_3_2b test_group_2_3
#define sgrp_case_2_3_2b test_group_2_3_2
#define numb_case_2_3_2b "2.3.2b"
#define name_case_2_3_2b "outgoing call"
#define xtra_case_2_3_2b "\"progress\""
#define sref_case_2_3_2b NULL
#define desc_case_2_3_2b "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2b(void)
{
	return test_case_2_3_2x(ISUP_EVNT_PROGRESS);
}

#define tgrp_case_2_3_2c test_group_2_3
#define sgrp_case_2_3_2c test_group_2_3_2
#define numb_case_2_3_2c "2.3.2c"
#define name_case_2_3_2c "outgoing call"
#define xtra_case_2_3_2c "\"in-band information or appropriate parttern available\""
#define sref_case_2_3_2c NULL
#define desc_case_2_3_2c "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2c(void)
{
	return test_case_2_3_2x(ISUP_EVNT_IBI);
}

#define desc_case_2_3_2y "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- incoming call\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2y(ulong evnt)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.opt.len = 0;
	switch ((cprim.event = evnt)) {
	case ISUP_EVNT_ALERTING:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER;
		iut_cpc_signal(PROCEEDING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return __RESULT_FAILURE;
		state = 6;
		iut_cpc_signal(ALERTING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 7;
		if (pt_event() != CPG)
			return __RESULT_FAILURE;
		state = 8;
		iut_cpc_signal(RINGING);
		break;
	case ISUP_EVNT_PROGRESS:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
		iut_cpc_signal(ALERTING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return __RESULT_FAILURE;
		state = 6;
		iut_cpc_signal(RINGING);
		state = 7;
		iut_cpc_signal(PROGRESS_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 8;
		if (pt_event() != CPG)
			return __RESULT_FAILURE;
		break;
	case ISUP_EVNT_IBI:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER;
		iut_cpc_signal(PROCEEDING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return __RESULT_FAILURE;
		state = 6;
		iut_cpc_signal(IBI_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return __RESULT_FAILURE;
		state = 7;
		if (pt_event() != CPG)
			return __RESULT_FAILURE;
		state = 8;
		iut_cpc_signal(RINGING);
		break;
	default:
		return __RESULT_SCRIPT_ERROR;
	}
	state = 9;
	iut_cpc_signal(CONNECT_REQ);
	state = 10;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 11;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 12;
	iut_cpc_signal(COMMUNICATION);
	state = 13;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 14;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 15;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_2d test_group_2_3
#define sgrp_case_2_3_2d test_group_2_3_2
#define numb_case_2_3_2d "2.3.2d"
#define name_case_2_3_2d "incoming call"
#define xtra_case_2_3_2d "\"alerting\""
#define sref_case_2_3_2d NULL
#define desc_case_2_3_2d "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2d(void)
{
	return test_case_2_3_2y(ISUP_EVNT_ALERTING);
}

#define tgrp_case_2_3_2e test_group_2_3
#define sgrp_case_2_3_2e test_group_2_3_2
#define numb_case_2_3_2e "2.3.2e"
#define name_case_2_3_2e "incoming call"
#define xtra_case_2_3_2e "\"progress\""
#define sref_case_2_3_2e NULL
#define desc_case_2_3_2e "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2e(void)
{
	return test_case_2_3_2y(ISUP_EVNT_PROGRESS);
}

#define tgrp_case_2_3_2f test_group_2_3
#define sgrp_case_2_3_2f test_group_2_3_2
#define numb_case_2_3_2f "2.3.2f"
#define name_case_2_3_2f "incoming call"
#define xtra_case_2_3_2f "\"in-band information or appropriate parttern available\""
#define sref_case_2_3_2f NULL
#define desc_case_2_3_2f "\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2f(void)
{
	return test_case_2_3_2y(ISUP_EVNT_IBI);
}

#define test_group_2_3_3 "Ordinary call with CON"

#define tgrp_case_2_3_3a test_group_2_3
#define sgrp_case_2_3_3a test_group_2_3_3
#define numb_case_2_3_3a "2.3.3a"
#define name_case_2_3_3a "outgoing call"
#define xtra_case_2_3_3a NULL
#define sref_case_2_3_3a NULL
#define desc_case_2_3_3a "\
Verify that a call can be successfully completed with a connect message."
static int
test_case_2_3_3a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(CON);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 5;
	iut_cpc_signal(COMMUNICATION);
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_3b test_group_2_3
#define sgrp_case_2_3_3b test_group_2_3_3
#define numb_case_2_3_3b "2.3.3b"
#define name_case_2_3_3b "incoming call"
#define xtra_case_2_3_3b NULL
#define sref_case_2_3_3b NULL
#define desc_case_2_3_3b "\
Verify that a call can be successfully completed with a connect message."
static int
test_case_2_3_3b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.opt.len = 0;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	iut_cpc_signal(CONNECT_REQ);
	state = 5;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 6;
	if (pt_event() != CON)
		return __RESULT_FAILURE;
	state = 7;
	iut_cpc_signal(COMMUNICATION);
	state = 8;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 9;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 11;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_3_4 "Call switched via satellite"

#define tgrp_case_2_3_4a test_group_2_3
#define sgrp_case_2_3_4a test_group_2_3_4
#define numb_case_2_3_4a "2.3.4a"
#define name_case_2_3_4a "outgoing call"
#define xtra_case_2_3_4a NULL
#define sref_case_2_3_4a NULL
#define desc_case_2_3_4a "\
Verify the satellite indicator in the initial address message is correctly\n\
set."
static int
test_case_2_3_4a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_NCI_ONE_SATELLITE_CCT | ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_4b test_group_2_3
#define sgrp_case_2_3_4b test_group_2_3_4
#define numb_case_2_3_4b "2.3.4b"
#define name_case_2_3_4b "incoming call"
#define xtra_case_2_3_4b NULL
#define sref_case_2_3_4b NULL
#define desc_case_2_3_4b "\
Verify the satellite indicator in the initial address message is correctly\n\
set."
static int
test_case_2_3_4b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_ONE_SATELLITE_CCT;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_3_5 "Blocking and unblocking during a call (initiated)"

#define tgrp_case_2_3_5a test_group_2_3
#define sgrp_case_2_3_5a test_group_2_3_5
#define numb_case_2_3_5a "2.3.5a"
#define name_case_2_3_5a "outgoing call"
#define xtra_case_2_3_5a NULL
#define sref_case_2_3_5a NULL
#define desc_case_2_3_5a "\
Verify that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_5a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 10;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 12;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 17;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 18;
	if (pt_event() != UBL)
		return __RESULT_FAILURE;
	state = 19;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 20;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return __RESULT_FAILURE;
	state = 21;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_5b test_group_2_3
#define sgrp_case_2_3_5b test_group_2_3_5
#define numb_case_2_3_5b "2.3.5b"
#define name_case_2_3_5b "incoming call"
#define xtra_case_2_3_5b NULL
#define sref_case_2_3_5b NULL
#define desc_case_2_3_5b "\
Verify that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_5b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	send(BLO);
	state = 12;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 13;
	iut_mgm_signal(BLOCKING_RES);
	state = 14;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 15;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 16;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 17;
	iut_cpc_signal(COMMUNICATION);
	state = 18;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 19;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 20;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 21;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 22;
	send(UBL);
	state = 23;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 24;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 25;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 26;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 27;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_3_6 "Blocking and unblocking during a call (received)"

#define tgrp_case_2_3_6a test_group_2_3
#define sgrp_case_2_3_6a test_group_2_3_6
#define numb_case_2_3_6a "2.3.6a"
#define name_case_2_3_6a "outgoing call"
#define xtra_case_2_3_6a NULL
#define sref_case_2_3_6a NULL
#define desc_case_2_3_6a "\
Verfiy that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_6a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.cic = 12;
	send(BLO);
	state = 9;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 10;
	iut_mgm_signal(BLOCKING_RES);
	state = 11;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 12;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 17;
	pmsg.cic = 12;
	send(UBL);
	state = 18;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 19;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 20;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 21;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 22;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_2_3_6b test_group_2_3
#define sgrp_case_2_3_6b test_group_2_3_6
#define numb_case_2_3_6b "2.3.6b"
#define name_case_2_3_6b "incoming call"
#define xtra_case_2_3_6b NULL
#define sref_case_2_3_6b NULL
#define desc_case_2_3_6b "\
Verfiy that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_6b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 12;
	if (pt_event() != BLO)
		return __RESULT_FAILURE;
	state = 13;
	send(BLA);
	state = 14;
	if (mgm_event() != MGM_BLOCKING_CON)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 16;
	iut_cpc_signal(COMMUNICATION);
	state = 17;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 18;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 19;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 20;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 21;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 22;
	if (pt_event() != UBL)
		return __RESULT_FAILURE;
	state = 23;
	send(UBA);
	state = 24;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return __RESULT_FAILURE;
	state = 25;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_2_4 "Propagation delay determination procedure"

#define tgrp_case_2_4_1 test_group_2_4
#define sgrp_case_2_4_1 NULL
#define numb_case_2_4_1 "2.4.1"
#define name_case_2_4_1 "IAM sent containing the PDC"
#define xtra_case_2_4_1 NULL
#define sref_case_2_4_1 NULL
#define desc_case_2_4_1 "\
Verify that SP A is able to increase the PDC by the delay value of the\n\
outgoing route (D ms)."
static int
test_case_2_4_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_2_4_2 test_group_2_4
#define sgrp_case_2_4_2 NULL
#define numb_case_2_4_2 "2.4.2"
#define name_case_2_4_2 "SP supporting the procedure to SP supporting the procedure"
#define xtra_case_2_4_2 NULL
#define sref_case_2_4_2 NULL
#define desc_case_2_4_2 "\
Verify that a call can be successfully completed and the value of the call\n\
history is higher as the value of the PDC."
static int
test_case_2_4_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_2_4_3 test_group_2_4
#define sgrp_case_2_4_3 NULL
#define numb_case_2_4_3 "2.4.3"
#define name_case_2_4_3 "Abnormal procedure, PDC is not recevied"
#define xtra_case_2_4_3 NULL
#define sref_case_2_4_3 NULL
#define desc_case_2_4_3 "\
Verify that a call can be successfully completed and the PDC is generated in\n\
SP A."
static int
test_case_2_4_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_2_4_4 test_group_2_4
#define sgrp_case_2_4_4 NULL
#define numb_case_2_4_4 "2.4.4"
#define name_case_2_4_4 "ISUP'92 supporting the procedure to Q.767"
#define xtra_case_2_4_4 NULL
#define sref_case_2_4_4 NULL
#define desc_case_2_4_4 "\
Verify that a call can be successfully completed and the PDC is discarded."
static int
test_case_2_4_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_2_4_5 test_group_2_4
#define sgrp_case_2_4_5 NULL
#define numb_case_2_4_5 "2.4.5"
#define name_case_2_4_5 "Q.767 to ISUP'92 supporting the procedure"
#define xtra_case_2_4_5 NULL
#define sref_case_2_4_5 NULL
#define desc_case_2_4_5 "\
Verify that a call can be successfully completed and CHI is discarded if\n\
received."
static int
test_case_2_4_5(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_3 "Normal call release"
#define test_group_3_1 "Calling party clears before address complete"

#define tgrp_case_3_1a test_group_3
#define sgrp_case_3_1a test_group_3_1
#define numb_case_3_1a "3.1a"
#define name_case_3_1a "outgoing call"
#define xtra_case_3_1a NULL
#define sref_case_3_1a NULL
#define desc_case_3_1a "\
Verify that the calling party can successfully release a call prior to receipt\n\
of any backward message."
static int
test_case_3_1a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 3;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 4;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 5;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 6;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 7;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_1b test_group_3
#define sgrp_case_3_1b test_group_3_1
#define numb_case_3_1b "3.1b"
#define name_case_3_1b "incoming call"
#define xtra_case_3_1b NULL
#define sref_case_3_1b NULL
#define desc_case_3_1b "\
Verify that the calling party can successfully release a call prior to receipt\n\
of any backward message."
static int
test_case_3_1b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 5;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 7;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_2 "Calling party clears before answer"

#define tgrp_case_3_2a test_group_3
#define sgrp_case_3_2a test_group_3_2
#define numb_case_3_2a "3.2a"
#define name_case_3_2a "outgoing call"
#define xtra_case_3_2a NULL
#define sref_case_3_2a NULL
#define desc_case_3_2a "\
Verify that the calling party can successfully release a call prior to receipt\n\
of answer."
static int
test_case_3_2a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_2b test_group_3
#define sgrp_case_3_2b test_group_3_2
#define numb_case_3_2b "3.2b"
#define name_case_3_2b "incoming call"
#define xtra_case_3_2b NULL
#define sref_case_3_2b NULL
#define desc_case_3_2b "\
Verify that the calling party can successfully release a call prior to receipt\n\
of answer."
static int
test_case_3_2b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 8;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 10;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_3 "Calling party clears after answer"

#define tgrp_case_3_3a test_group_3
#define sgrp_case_3_3a test_group_3_3
#define numb_case_3_3a "3.3a"
#define name_case_3_3a "outgoing call"
#define xtra_case_3_3a NULL
#define sref_case_3_3a NULL
#define desc_case_3_3a "\
Verify that the calling party can successfully release a call after answer."
static int
test_case_3_3a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	iut_cpc_signal(COMMUNICATION);
	state = 8;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 9;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 10;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 11;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_3b test_group_3
#define sgrp_case_3_3b test_group_3_3
#define numb_case_3_3b "3.3b"
#define name_case_3_3b "incoming call"
#define xtra_case_3_3b NULL
#define sref_case_3_3b NULL
#define desc_case_3_3b "\
Verify that the calling party can successfully release a call after answer."
static int
test_case_3_3b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_4 "Called party clears after answer"

#define tgrp_case_3_4a test_group_3
#define sgrp_case_3_4a test_group_3_4
#define numb_case_3_4a "3.4a"
#define name_case_3_4a "outgoing call"
#define xtra_case_3_4a NULL
#define sref_case_3_4a NULL
#define desc_case_3_4a "\
Verify that a call can be successfully released in the backward direction."
static int
test_case_3_4a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 10;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 12;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_4b test_group_3
#define sgrp_case_3_4b test_group_3_4
#define numb_case_3_4b "3.4b"
#define name_case_3_4b "incoming call"
#define xtra_case_3_4b NULL
#define sref_case_3_4b NULL
#define desc_case_3_4b "\
Verify that a call can be successfully released in the backward direction."
static int
test_case_3_4b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 12;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 13;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(RLC);
	state = 14;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_5 "Suspend initiated by the network"

#define tgrp_case_3_5a test_group_3
#define sgrp_case_3_5a test_group_3_5
#define numb_case_3_5a "3.5a"
#define name_case_3_5a "outgoing call"
#define xtra_case_3_5a NULL
#define sref_case_3_5a NULL
#define desc_case_3_5a "\
Verify that the called subscriber can suscessfully clear back and reanswer a\n\
call."
static int
test_case_3_5a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.sris = ISUP_SRIS_NETWORK_INITIATED;
	send(SUS);
	state = 9;
	if (cpc_event() != CPC_SUSPEND_IND)
		return __RESULT_FAILURE;
	state = 10;
	send(RES);
	state = 11;
	if (cpc_event() != CPC_RESUME_IND)
		return __RESULT_FAILURE;
	state = 12;
	iut_cpc_signal(COMMUNICATION);
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_5b test_group_3
#define sgrp_case_3_5b test_group_3_5
#define numb_case_3_5b "3.5b"
#define name_case_3_5b "incoming call"
#define xtra_case_3_5b NULL
#define sref_case_3_5b NULL
#define desc_case_3_5b "\
Verify that the called subscriber can suscessfully clear back and reanswer a\n\
call."
static int
test_case_3_5b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.flags = ISUP_SRIS_NETWORK_INITIATED;
	iut_cpc_signal(SUSPEND_REQ);
	state = 12;
	if (pt_event() != SUS)
		return (__RESULT_FAILURE);
	state = 13;
	iut_cpc_signal(RESUME_REQ);
	state = 14;
	if (pt_event() != RES)
		return (__RESULT_FAILURE);
	state = 15;
	iut_cpc_signal(COMMUNICATION);
	state = 16;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 17;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_6 "Suspend and resume initiated by a calling party"

#define tgrp_case_3_6a test_group_3
#define sgrp_case_3_6a test_group_3_6
#define numb_case_3_6a "3.6a"
#define name_case_3_6a "outgoing call"
#define xtra_case_3_6a NULL
#define sref_case_3_6a NULL
#define desc_case_3_6a "\
Verify that the calling subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_6a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.flags = ISUP_SRIS_USER_INITIATED;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(SUSPEND_REQ);
	state = 10;
	if (pt_event() != SUS)
		return __RESULT_FAILURE;
	state = 11;
	iut_cpc_signal(RESUME_REQ);
	state = 12;
	if (pt_event() != RES)
		return __RESULT_FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_6a test_group_3
#define sgrp_case_3_6a test_group_3_6
#define numb_case_3_6b "3.6b"
#define name_case_3_6b "incoming call"
#define xtra_case_3_6b NULL
#define sref_case_3_6b NULL
#define desc_case_3_6b "\
Verify that the calling subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_6b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	pmsg.opt.len = 0;
	send(SUS);
	state = 12;
	if (cpc_event() != CPC_SUSPEND_IND)
		return __RESULT_FAILURE;
	state = 13;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	pmsg.opt.len = 0;
	send(RES);
	state = 14;
	if (cpc_event() != CPC_RESUME_IND)
		return __RESULT_FAILURE;
	state = 15;
	iut_cpc_signal(COMMUNICATION);
	state = 16;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 17;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_7 "Suspend and resume initiated by a called party"

#define tgrp_case_3_7a test_group_3
#define sgrp_case_3_7a test_group_3_7
#define numb_case_3_7a "3.7a"
#define name_case_3_7a "outgoing call"
#define xtra_case_3_7a NULL
#define sref_case_3_7a NULL
#define desc_case_3_7a "\
Verify that the called subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_7a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	send(SUS);
	state = 10;
	if (cpc_event() != CPC_SUSPEND_IND)
		return __RESULT_FAILURE;
	state = 11;
	send(RES);
	state = 12;
	if (cpc_event() != CPC_RESUME_IND)
		return __RESULT_FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_7b test_group_3
#define sgrp_case_3_7b test_group_3_7
#define numb_case_3_7b "3.7b"
#define name_case_3_7b "incoming call"
#define xtra_case_3_7b NULL
#define sref_case_3_7b NULL
#define desc_case_3_7b "\
Verify that the called subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_7b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return __RESULT_FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return __RESULT_FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return __RESULT_FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.flags = ISUP_SRIS_USER_INITIATED;
	iut_cpc_signal(SUSPEND_REQ);
	state = 12;
	if (pt_event() != SUS)
		return (__RESULT_FAILURE);
	state = 13;
	iut_cpc_signal(RESUME_REQ);
	state = 14;
	if (pt_event() != RES)
		return (__RESULT_FAILURE);
	state = 15;
	iut_cpc_signal(COMMUNICATION);
	state = 16;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 17;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_3_8 "Collision of REL messages"

#define tgrp_case_3_8a test_group_3
#define sgrp_case_3_8a test_group_3_8
#define numb_case_3_8a "3.8a"
#define name_case_3_8a "REL sent first"
#define xtra_case_3_8a NULL
#define sref_case_3_8a NULL
#define desc_case_3_8a "\
Verify that a release message may be received at an exchange from a succeeding\n\
or preceding exchange after the release of the switch path is initiated."
static int
test_case_3_8a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 13;
	send(RLC);
	state = 14;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_3_8b test_group_3
#define sgrp_case_3_8b test_group_3_8
#define numb_case_3_8b "3.8b"
#define name_case_3_8b "REL received first"
#define xtra_case_3_8b NULL
#define sref_case_3_8b NULL
#define desc_case_3_8b "\
Verify that a release message may be received at an exchange from a succeeding\n\
or preceding exchange after the release of the switch path is initiated."
static int
test_case_3_8b(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 10;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	state = 12;
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 13;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_4 "Unsuccessful call setup"
#define test_group_4_1 "Validate a set of known causes for release"

#define tgrp_case_4_1x test_group_4
#define sgrp_case_4_1x test_group_4_1
#define name_case_4_1x "Case A"
#define xtra_case_4_1x NULL
#define sref_case_4_1x NULL
#define desc_case_4_1x "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1x(ulong cause)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = cause;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 3;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 4;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 6;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 7;
	return __RESULT_SUCCESS;
}

#define tgrp_case_4_1y test_group_4
#define sgrp_case_4_1y test_group_4_1
#define name_case_4_1y "Case B"
#define xtra_case_4_1y NULL
#define sref_case_4_1y NULL
#define desc_case_4_1y "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1y(ulong cause)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_PROCEEDING_IND)
		return __RESULT_FAILURE;
	state = 5;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = cause;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 6;
	if (cpc_event() != CPC_RELEASE_IND)
		return __RESULT_FAILURE;
	state = 7;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_4_1a test_group_4
#define sgrp_case_4_1a test_group_4_1
#define numb_case_4_1a "4.1a"
#define name_case_4_1a "Case A"
#define xtra_case_4_1a "\"unallocated number\""
#define sref_case_4_1a NULL
#define desc_case_4_1a "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1a(void)
{
	return test_case_4_1x(CC_CAUS_UNALLOCATED_NUMBER);
}

#define tgrp_case_4_1b test_group_4
#define sgrp_case_4_1b test_group_4_1
#define numb_case_4_1b "4.1b"
#define name_case_4_1b "Case A"
#define xtra_case_4_1b "\"no circuit available\""
#define sref_case_4_1b NULL
#define desc_case_4_1b "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1b(void)
{
	return test_case_4_1x(CC_CAUS_NO_CCT_AVAILABLE);
}

#define tgrp_case_4_1c test_group_4
#define sgrp_case_4_1c test_group_4_1
#define numb_case_4_1c "4.1c"
#define name_case_4_1c "Case A"
#define xtra_case_4_1c "\"switching equipment congestion\""
#define sref_case_4_1c NULL
#define desc_case_4_1c "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1c(void)
{
	return test_case_4_1x(CC_CAUS_SWITCHING_EQUIP_CONGESTION);
}

#define tgrp_case_4_1d test_group_4
#define sgrp_case_4_1d test_group_4_1
#define numb_case_4_1d "4.1d"
#define name_case_4_1d "Case B"
#define xtra_case_4_1d "\"unallocated number\""
#define sref_case_4_1d NULL
#define desc_case_4_1d "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1d(void)
{
	return test_case_4_1y(CC_CAUS_UNALLOCATED_NUMBER);
}

#define tgrp_case_4_1e test_group_4
#define sgrp_case_4_1e test_group_4_1
#define numb_case_4_1e "4.1e"
#define name_case_4_1e "Case B"
#define xtra_case_4_1e "\"no circuit available\""
#define sref_case_4_1e NULL
#define desc_case_4_1e "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1e(void)
{
	return test_case_4_1y(CC_CAUS_NO_CCT_AVAILABLE);
}

#define tgrp_case_4_1f test_group_4
#define sgrp_case_4_1f test_group_4_1
#define numb_case_4_1f "4.1f"
#define name_case_4_1f "Case B"
#define xtra_case_4_1f "\"switching equipment congestion\""
#define sref_case_4_1f NULL
#define desc_case_4_1f "\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1f(void)
{
	return test_case_4_1y(CC_CAUS_SWITCHING_EQUIP_CONGESTION);
}

#define test_group_5_1 "Abnormal situation during a call"

#define tgrp_case_5_1 test_group_5_1
#define sgrp_case_5_1 NULL
#define name_case_5_1 "Inability to release in response to a REL after ANM"
#define xtra_case_5_1 NULL
#define sref_case_5_1 NULL
#define numb_case_5_1 "5.1"
#define desc_case_5_1 "\
Verify that if the SP is unable to return a circuit to the idle condition in\n\
response to a release message, the circuit will be blocked."
static int
test_case_5_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_5_2 "Timers"

#define tgrp_case_5_2_1 test_group_5_2
#define sgrp_case_5_2_1 NULL
#define numb_case_5_2_1 "5.2.1"
#define name_case_5_2_1 "T7: waiting for ACM or CON"
#define xtra_case_5_2_1 NULL
#define sref_case_5_2_1 NULL
#define desc_case_5_2_1 "\
Check that at the expiry of T7 the circuit is released."
static int
test_case_5_2_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	start_st(timer[t7].hi);
	beg_time = milliseconds("  T7  ");
	state = 3;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 4;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 5;
	if (check_time("  T7  ", now() - beg_time, timer[t7].lo, timer[t7].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 6;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 7;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	state = 8;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_2 test_group_5_2
#define sgrp_case_5_2_2 NULL
#define numb_case_5_2_2 "5.2.2"
#define name_case_5_2_2 "T9: waiting for ANM"
#define xtra_case_5_2_2 NULL
#define sref_case_5_2_2 NULL
#define desc_case_5_2_2 "\
Verify that if an answer message is not received within T9 after receiving an\n\
address complete message, the connection is released by the outgoing\n\
signalling point."
static int
test_case_5_2_2(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	start_st(timer[t9].hi);
	beg_time = milliseconds("  T9  ");
	state = 7;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 9;
	if (check_time("  T9  ", now() - beg_time, timer[t9].lo, timer[t9].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 10;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_3 test_group_5_2
#define sgrp_case_5_2_3 NULL
#define numb_case_5_2_3 "5.2.3"
#define name_case_5_2_3 "T1 and T5: failure to receive a RLC"
#define xtra_case_5_2_3 NULL
#define sref_case_5_2_3 NULL
#define desc_case_5_2_3 "\
Verify that appropriate actions take place at the expiry of timers T1 and T5."
static int
test_case_5_2_3(void)
{
	int subsequent_t1_expiry = 0;
	unsigned long t1_time = now(), t5_time = now(), rel_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			pmsg.cic = 12;
			pmsg.nci = 0;
			pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8;
			pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
			pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
			strncpy(pmsg.cdpn.num, "17805551212/", 24);
			pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
			send(IAM);
			state = 1;
		case 1:
			if (cpc_event() != CPC_SETUP_IND)
				return __RESULT_FAILURE;
			state = 2;
		case 2:
			iut_cpc_signal(SETUP_RES);
			state = 3;
		case 3:
			if (cpc_event() != CPC_OK_ACK)
				return __RESULT_FAILURE;
			state = 4;
		case 4:
			cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
			cprim.opt.len = 0;
			iut_cpc_signal(ALERTING_REQ);
			if (cpc_event() != CPC_OK_ACK)
				return __RESULT_FAILURE;
			state = 5;
		case 5:
			if (pt_event() != ACM)
				return __RESULT_FAILURE;
			state = 6;
		case 6:
			iut_cpc_signal(RINGING);
			state = 7;
		case 7:
			iut_cpc_signal(CONNECT_REQ);
			state = 8;
		case 8:
			if (cpc_event() != CPC_SETUP_COMPLETE_IND)
				return __RESULT_FAILURE;
			state = 9;
		case 9:
			if (pt_event() != ANM)
				return __RESULT_FAILURE;
			state = 10;
		case 10:
			iut_cpc_signal(COMMUNICATION);
			state = 11;
		case 11:
			cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
			cprim.opt.len = 0;
			cprim.addr.len = 0;
			iut_cpc_signal(RELEASE_REQ);
			state = 12;
		case 12:
			if (pt_event() != REL)
				return __RESULT_FAILURE;
			state = 13;
		case 13:
			t5_time = milliseconds("  T5  ");
			state = 14;
		case 14:
			t1_time = milliseconds_2nd("  T1  ");
			start_st(timer[t1].hi);
			state = 15;
		case 15:
			switch (pt_event()) {
			case REL:
				rel_time = now();
				if (check_time("  T1  ", rel_time - t1_time, timer[t1].lo, timer[t1].hi) != __RESULT_SUCCESS)
					goto failure;
				t1_time = milliseconds_2nd("  T1  ");
				start_st(timer[t1].hi);
				break;
			case RSC:
				rel_time = now();
				state = 17;
				goto state_17;
			default:
				goto failure;
			}
		case 16:
			state = 16;
			if (!subsequent_t1_expiry) {
				if (cpc_event() != CPC_RELEASE_CON)
					goto failure;
				subsequent_t1_expiry = 1;
			}
			state = 15;
			break;
		case 17:
		      state_17:
			if (check_time("  T5  ", now() - t5_time, timer[t5].lo, timer[t5].hi) != __RESULT_SUCCESS)
				goto failure;
			start_st(timer[t5].hi);
			state = 18;
		case 18:
			if (mnt_event() != MGM_MAINT_IND)
				goto failure;
			state = 19;
		case 19:
			pmsg.cic = imsg.cic;
			pmsg.opt.len = 0;
			pmsg.caus.buf[0] = 0x80;
			pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
			pmsg.caus.len = 2;
			send(RLC);
			state = 20;
		case 20:
			if (wait_event(0, ANY) != NO_MSG)
				return __RESULT_FAILURE;
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	send(RLC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_4 test_group_5_2
#define sgrp_case_5_2_4 NULL
#define numb_case_5_2_4 "5.2.4"
#define name_case_5_2_4 "T6 :waiting for RES (Network) message"
#define xtra_case_5_2_4 NULL
#define sref_case_5_2_4 NULL
#define desc_case_5_2_4 "\
Verify that the call is released at the expiry of timer T6."
static int
test_case_5_2_4(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 7;
	iut_cpc_signal(COMMUNICATION);
	state = 8;
	pmsg.sris = ISUP_SRIS_NETWORK_INITIATED;
	send(SUS);
	state = 9;
	if (cpc_event() != CPC_SUSPEND_IND)
		return __RESULT_FAILURE;
	state = 10;
	start_st(timer[t6].hi);
	beg_time = milliseconds("  T6  ");
	state = 11;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 12;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 13;
	if (check_time("  T6  ", now() - beg_time, timer[t6].lo, timer[t6].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 14;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_5a test_group_5_2
#define sgrp_case_5_2_5a NULL
#define numb_case_5_2_5a "5.2.5a"
#define name_case_5_2_5a "T8: waiting for COT message if applicable: is required"
#define xtra_case_5_2_5a NULL
#define sref_case_5_2_5a NULL
#define desc_case_5_2_5a "\
Verify that when the IAM indicates that the continuity check:\n\
-- is required; or\n\
-- is performed on a previous circuit,\n\
and the COT message is not received within T8, the connection is released by\n\
the incoming signalling point."
static int
test_case_5_2_5a(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_REQUIRED;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		goto failure;
	state = 2;
	if (tst_event() != MGM_CONT_CHECK_IND)
		goto failure;
	state = 3;
	iut_tst_signal(CONT_TEST_REQ);
	state = 4;
	beg_time = milliseconds("  T8  ");
	state = 5;
	start_st(timer[t8].hi + 100);
	if (pt_event() != REL)
		goto failure;
	state = 6;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		goto failure;
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		goto failure;
	state = 8;
	if (check_time("  T8  ", now() - beg_time, timer[t8].lo, timer[t8].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 9;
	send(RLC);
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	send(RSC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_5b test_group_5_2
#define sgrp_case_5_2_5b NULL
#define numb_case_5_2_5b "5.2.5b"
#define name_case_5_2_5b "T8: waiting for COT message if applicable: is performed on a previous circuit"
#define xtra_case_5_2_5b NULL
#define sref_case_5_2_5b NULL
#define desc_case_5_2_5b "\
Verify that when the IAM indicates that the continuity check:\n\
-- is required; or\n\
-- is performed on a previous circuit,\n\
and the COT message is not received within T8, the connection is released by\n\
the incoming signalling point."
static int
test_case_5_2_5b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_PREVIOUS;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		goto failure;
	state = 2;
	beg_time = milliseconds("  T8  ");
	state = 3;
	start_st(timer[t8].hi + 100);
	if (pt_event() != REL)
		goto failure;
	state = 4;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		goto failure;
	state = 5;
	if (check_time("  T8  ", now() - beg_time, timer[t8].lo, timer[t8].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 6;
	send(RLC);
	state = 7;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	send(RSC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_6 test_group_5_2
#define sgrp_case_5_2_6 NULL
#define numb_case_5_2_6 "5.2.6"
#define name_case_5_2_6 "T12 and T13: failure to receive a BLA"
#define xtra_case_5_2_6 NULL
#define sref_case_5_2_6 NULL
#define desc_case_5_2_6 "\
Verify that appropriate actions take place at the expiry of timers T12 and\n\
T13."
static int
test_case_5_2_6(void)
{
	unsigned long t12_time = now(), t13_time = now(), blo_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.flags = ISUP_MAINTENANCE_ORIENTED;
			mprim.addr.add.scope = ISUP_SCOPE_CT;
			mprim.addr.add.id = 12;
			mprim.addr.add.cic = 12;
			mprim.addr.len = sizeof(mprim.addr.add);
			iut_mgm_signal(BLOCKING_REQ);
			if (any_event() != BLO)
				return __RESULT_FAILURE;
			t13_time = milliseconds(" T13  ");
			t12_time = milliseconds_2nd(" T12  ");
			start_st(timer[t12].hi);
			state = 1;
			break;
		case 1:
			switch (any_event()) {
			case BLO:
				blo_time = now();
				if (check_time(" T12  ", blo_time - t12_time, timer[t12].lo, timer[t12].hi) != __RESULT_SUCCESS) {
					state = 3;
					break;
				}
				t12_time = milliseconds_2nd(" T12  ");
				start_st(timer[t12].hi);
				break;
			case MGM_MAINT_IND:
				state = 2;
				break;
			default:
				goto failure;
			}
			break;
		case 2:
			if (any_event() != BLO)
				goto failure;
			blo_time = now();
			state = 3;
			break;
		case 3:
			if (check_time(" T13  ", now() - t13_time, timer[t13].lo, timer[t13].hi) != __RESULT_SUCCESS)
				goto failure;
			t13_time = milliseconds(" T13  ");
			start_st(timer[t13].hi);
			state = 4;
			break;
		case 4:
			switch (any_event()) {
			case BLO:
				blo_time = now();
				if (check_time(" T13  ", now() - t13_time, timer[t13].lo, timer[t13].hi) != __RESULT_SUCCESS)
					goto failure;
				t13_time = milliseconds(" T13  ");
				start_st(timer[t13].hi);
				state = 5;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 5:
			pmsg.cic = imsg.cic;
			pmsg.opt.len = 0;
			send(BLA);
			iut_mgm_signal(UNBLOCKING_REQ);
			send(UBA);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(BLA);
	iut_mgm_signal(UNBLOCKING_REQ);
	send(UBA);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_7 test_group_5_2
#define sgrp_case_5_2_7 NULL
#define numb_case_5_2_7 "5.2.7"
#define name_case_5_2_7 "T14 and T15: failure to receive UBA"
#define xtra_case_5_2_7 NULL
#define sref_case_5_2_7 NULL
#define desc_case_5_2_7 "\
Verify that appropriate actions take place at the expiry of timers T14 and\n\
T15."
static int
test_case_5_2_7(void)
{
	unsigned long t14_time = now(), t15_time = now(), ubl_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.flags = ISUP_MAINTENANCE_ORIENTED;
			mprim.addr.add.scope = ISUP_SCOPE_CT;
			mprim.addr.add.id = 12;
			mprim.addr.add.cic = 12;
			mprim.addr.len = sizeof(mprim.addr.add);
			iut_mgm_signal(BLOCKING_REQ);
			if (any_event() != BLO)
				return __RESULT_FAILURE;
			state = 1;
			break;
		case 1:
			pmsg.cic = imsg.cic;
			send(BLA);
			if (any_event() != MGM_BLOCKING_CON)
				return __RESULT_FAILURE;
			state = 2;
			break;
		case 2:
			mprim.flags = ISUP_MAINTENANCE_ORIENTED;
			mprim.addr.add.scope = ISUP_SCOPE_CT;
			mprim.addr.add.id = 12;
			mprim.addr.add.cic = 12;
			mprim.addr.len = sizeof(mprim.addr.add);
			iut_mgm_signal(UNBLOCKING_REQ);
			if (any_event() != UBL)
				return __RESULT_FAILURE;
			t15_time = milliseconds(" T15  ");
			t14_time = milliseconds_2nd(" T14  ");
			start_st(timer[t14].hi);
			state = 3;
			break;
		case 3:
			switch (any_event()) {
			case UBL:
				ubl_time = now();
				if (check_time(" T14  ", ubl_time - t14_time, timer[t14].lo, timer[t14].hi) != __RESULT_SUCCESS) {
					state = 5;
					break;
				}
				t14_time = milliseconds_2nd(" T14  ");
				start_st(timer[t14].hi);
				break;
			case MGM_MAINT_IND:
				state = 4;
				break;
			default:
				goto failure;
			}
			break;
		case 4:
			if (any_event() != UBL)
				goto failure;
			ubl_time = now();
			state = 5;
			break;
		case 5:
			if (check_time(" T15  ", ubl_time - t15_time, timer[t15].lo, timer[t15].hi)
			    != __RESULT_SUCCESS)
				goto failure;
			t15_time = milliseconds(" T15  ");
			start_st(timer[t15].hi);
			state = 6;
			break;
		case 6:
			switch (any_event()) {
			case UBL:
				ubl_time = now();
				if (check_time(" T15  ", ubl_time - t15_time, timer[t15].lo, timer[t15].hi) != __RESULT_SUCCESS)
					goto failure;
				t15_time = milliseconds(" T15  ");
				start_st(timer[t15].hi);
				state = 7;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 7:
			pmsg.cic = imsg.cic;
			pmsg.opt.len = 0;
			send(UBA);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(UBA);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_8 test_group_5_2
#define sgrp_case_5_2_8 NULL
#define numb_case_5_2_8 "5.2.8"
#define name_case_5_2_8 "T16 and T17: failure to receive a RLC"
#define xtra_case_5_2_8 NULL
#define sref_case_5_2_8 NULL
#define desc_case_5_2_8 "\
Verify that appropriate actions take place at the expiry of timers T16 and\n\
T17."
static int
test_case_5_2_8(void)
{
	unsigned long t16_time = now(), t17_time = now(), rsc_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.flags = ISUP_MAINTENANCE_ORIENTED;
			mprim.addr.add.scope = ISUP_SCOPE_CT;
			mprim.addr.add.id = 12;
			mprim.addr.add.cic = 12;
			mprim.addr.len = sizeof(mprim.addr.add);
			iut_mgm_signal(RESET_REQ);
			if (any_event() != RSC)
				return __RESULT_FAILURE;
			rsc_time = now();
			t17_time = milliseconds(" T17  ");
			t16_time = milliseconds_2nd(" T16  ");
			start_st(timer[t16].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case RSC:
				rsc_time = now();
				if (check_time(" T16  ", rsc_time - t16_time, timer[t16].lo, timer[t16].hi) != __RESULT_SUCCESS) {
					state = 3;
					break;
				}
				t16_time = milliseconds_2nd(" T16  ");
				start_st(timer[t16].hi);
				break;
			case MGM_MAINT_IND:
				state = 2;
				break;
			default:
				goto failure;
			}
			break;
		case 2:
			if (any_event() != RSC)
				goto failure;
			rsc_time = now();
			state = 3;
		case 3:
			if (check_time(" T17  ", rsc_time - t17_time, timer[t17].lo, timer[t17].hi)
			    != __RESULT_SUCCESS)
				goto failure;
			t17_time = milliseconds(" T17  ");
			start_st(timer[t17].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case RSC:
				rsc_time = now();
				if (check_time(" T17  ", rsc_time - t17_time, timer[t17].lo, timer[t17].hi) != __RESULT_SUCCESS)
					goto failure;
				t17_time = milliseconds(" T17  ");
				start_st(timer[t17].hi);
				state = 5;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 5:
			pmsg.cic = imsg.cic;
			pmsg.opt.len = 0;
			send(RLC);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_9 test_group_5_2
#define sgrp_case_5_2_9 NULL
#define numb_case_5_2_9 "5.2.9"
#define name_case_5_2_9 "T18 and T19: failure to receive a CGBA"
#define xtra_case_5_2_9 NULL
#define sref_case_5_2_9 NULL
#define desc_case_5_2_9 "\
Verify that appropriate actions take place at the expiry of timers T18 and\n\
T19."
static int
test_case_5_2_9(void)
{
	unsigned long t18_time = now(), t19_time = now(), cgb_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.addr.add.scope = ISUP_SCOPE_CG;
			mprim.addr.add.id = 1;
			mprim.addr.add.cic = 1;
			mprim.addr.len = sizeof(mprim.addr.add);
			mprim.flags = ISUP_GROUP | ISUP_MAINTENANCE_ORIENTED;
			iut_mgm_signal(BLOCKING_REQ);
			if (any_event() != CGB)
				return __RESULT_FAILURE;
			cgb_time = now();
			t19_time = milliseconds(" T19  ");
			t18_time = milliseconds_2nd(" T18  ");
			start_st(timer[t18].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case CGB:
				cgb_time = now();
				if (check_time(" T18  ", cgb_time - t18_time, timer[t18].lo, timer[t18].hi) != __RESULT_SUCCESS) {
					state = 3;
					break;
				}
				t18_time = milliseconds_2nd(" T18  ");
				start_st(timer[t18].hi);
				break;
			case MGM_MAINT_IND:
				state = 2;
				break;
			default:
				goto failure;
			}
			break;
		case 2:
			if (any_event() != CGB)
				goto failure;
			cgb_time = now();
			state = 3;
		case 3:
			if (check_time(" T19  ", cgb_time - t19_time, timer[t19].lo, timer[t19].hi)
			    != __RESULT_SUCCESS)
				goto failure;
			t19_time = milliseconds(" T19  ");
			start_st(timer[t19].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case CGB:
				cgb_time = now();
				if (check_time(" T19  ", cgb_time - t19_time, timer[t19].lo, timer[t19].hi) != __RESULT_SUCCESS)
					goto failure;
				t19_time = milliseconds(" T19  ");
				start_st(timer[t19].hi);
				state = 5;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 5:
			pmsg.cic = imsg.cic;
			bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
			pmsg.rs.len = imsg.rs.len;
			pmsg.cgi = imsg.cgi;
			send(CGBA);
			iut_mgm_signal(UNBLOCKING_REQ);
			send(CGUA);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
	return __RESULT_SCRIPT_ERROR;
      failure:
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGBA);
	iut_mgm_signal(UNBLOCKING_REQ);
	send(CGUA);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_10 test_group_5_2
#define sgrp_case_5_2_10 NULL
#define numb_case_5_2_10 "5.2.10"
#define name_case_5_2_10 "T20 and T21: failure to receive a CGUA"
#define xtra_case_5_2_10 NULL
#define sref_case_5_2_10 NULL
#define desc_case_5_2_10 "\
Verify that appropriate actions take place at the expiry of timers T20 and\n\
T21."
static int
test_case_5_2_10(void)
{
	unsigned long t20_time = now(), t21_time = now(), cgu_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.addr.add.scope = ISUP_SCOPE_CG;
			mprim.addr.add.id = 1;
			mprim.addr.add.cic = 1;
			mprim.addr.len = sizeof(mprim.addr.add);
			mprim.flags = ISUP_GROUP | ISUP_MAINTENANCE_ORIENTED;
			iut_mgm_signal(BLOCKING_REQ);
			if (any_event() != CGB)
				return __RESULT_FAILURE;
			state = 1;
		case 1:
			pmsg.cic = imsg.cic;
			bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
			pmsg.rs.len = imsg.rs.len;
			pmsg.cgi = imsg.cgi;
			send(CGBA);
			if (any_event() != MGM_BLOCKING_CON)
				return __RESULT_FAILURE;
			state = 2;
		case 2:
			mprim.addr.add.scope = ISUP_SCOPE_CG;
			mprim.addr.add.id = 1;
			mprim.addr.add.cic = 1;
			mprim.addr.len = sizeof(mprim.addr.add);
			mprim.flags = ISUP_GROUP | ISUP_MAINTENANCE_ORIENTED;
			iut_mgm_signal(UNBLOCKING_REQ);
			if (any_event() != CGU)
				return __RESULT_FAILURE;
			cgu_time = now();
			t21_time = milliseconds(" T21  ");
			t20_time = milliseconds_2nd(" T20  ");
			start_st(timer[t20].hi);
			state = 3;
		case 3:
			switch (any_event()) {
			case CGU:
				cgu_time = now();
				if (check_time(" T20  ", cgu_time - t20_time, timer[t20].lo, timer[t20].hi) != __RESULT_SUCCESS) {
					state = 5;
					break;
				}
				t20_time = milliseconds_2nd(" T20  ");
				start_st(timer[t20].hi);
				break;
			case MGM_MAINT_IND:
				state = 4;
				break;
			default:
				goto failure;
			}
			break;
		case 4:
			if (any_event() != CGU)
				goto failure;
			cgu_time = now();
			state = 5;
		case 5:
			if (check_time(" T21  ", cgu_time - t21_time, timer[t21].lo, timer[t21].hi)
			    != __RESULT_SUCCESS)
				goto failure;
			t21_time = milliseconds(" T20  ");
			start_st(timer[t21].hi);
			state = 6;
		case 6:
			switch (any_event()) {
			case CGU:
				cgu_time = now();
				if (check_time(" T21  ", cgu_time - t21_time, timer[t21].lo, timer[t21].hi) != __RESULT_SUCCESS)
					goto failure;
				t21_time = milliseconds(" T21  ");
				start_st(timer[t21].hi);
				state = 7;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 7:
			pmsg.cic = imsg.cic;
			bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
			pmsg.rs.len = imsg.rs.len;
			pmsg.cgi = imsg.cgi;
			send(CGUA);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
	return __RESULT_SCRIPT_ERROR;
      failure:
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGUA);
	return __RESULT_FAILURE;
}

#define tgrp_case_5_2_11 test_group_5_2
#define sgrp_case_5_2_11 NULL
#define numb_case_5_2_11 "5.2.11"
#define name_case_5_2_11 "T22 and T23: failure to receive a GRA"
#define xtra_case_5_2_11 NULL
#define sref_case_5_2_11 NULL
#define desc_case_5_2_11 "\
Verify that appropriate actions take place at the expiry of timers T22 and\n\
T23."
static int
test_case_5_2_11(void)
{
	unsigned long t22_time = now(), t23_time = now(), grs_time = now();

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			mprim.addr.add.scope = ISUP_SCOPE_CG;
			mprim.addr.add.id = 1;
			mprim.addr.add.cic = 1;
			mprim.addr.len = sizeof(mprim.addr.add);
			mprim.flags = ISUP_GROUP | ISUP_MAINTENANCE_ORIENTED;
			iut_mgm_signal(RESET_REQ);
			if (any_event() != GRS)
				return __RESULT_FAILURE;
			grs_time = now();
			t23_time = milliseconds(" T23  ");
			t22_time = milliseconds_2nd(" T22  ");
			start_st(timer[t22].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case GRS:
				grs_time = now();
				if (check_time(" T22  ", grs_time - t22_time, timer[t22].lo, timer[t22].hi) != __RESULT_SUCCESS) {
					state = 3;
					break;
				}
				t22_time = milliseconds_2nd(" T22  ");
				start_st(timer[t22].hi);
				break;
			case MGM_MAINT_IND:
				state = 2;
				break;
			default:
				goto failure;
			}
			break;
		case 2:
			if (any_event() != GRS)
				goto failure;
			grs_time = now();
			state = 3;
		case 3:
			if (check_time(" T23  ", grs_time - t23_time, timer[t23].lo, timer[t23].hi)
			    != __RESULT_SUCCESS)
				goto failure;
			t23_time = milliseconds(" T23  ");
			start_st(timer[t23].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case GRS:
				grs_time = now();
				if (check_time(" T23  ", grs_time - t23_time, timer[t23].lo, timer[t23].hi) != __RESULT_SUCCESS)
					goto failure;
				t23_time = milliseconds(" T23  ");
				start_st(timer[t23].hi);
				state = 5;
				break;
			case MGM_MAINT_IND:
				break;
			default:
				goto failure;
			}
			break;
		case 5:
			pmsg.cic = imsg.cic;
			pmsg.rs.buf[0] = imsg.rs.buf[0];
			pmsg.rs.buf[1] = 0x00;
			pmsg.rs.buf[2] = 0x00;
			pmsg.rs.buf[3] = 0x00;
			pmsg.rs.buf[4] = 0x00;
			pmsg.rs.len = 5;
			send(GRA);
			return __RESULT_SUCCESS;
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
	return __RESULT_SCRIPT_ERROR;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.rs.buf[0] = imsg.rs.buf[0];
	pmsg.rs.buf[1] = 0x00;
	pmsg.rs.buf[2] = 0x00;
	pmsg.rs.buf[3] = 0x00;
	pmsg.rs.buf[4] = 0x00;
	pmsg.rs.len = 5;
	send(GRA);
	return __RESULT_FAILURE;
}

#define test_group_5_3 "Reset of circuits during a call"

#define tgrp_case_5_3_1 test_group_5_3
#define sgrp_case_5_3_1 NULL
#define numb_case_5_3_1 "5.3.1"
#define name_case_5_3_1 "Of an outgoing circuit"
#define xtra_case_5_3_1 NULL
#define sref_case_5_3_1 NULL
#define desc_case_5_3_1 "\
Verify that on receipt of a reset message the call is immediately released --\n\
outgoing call."
static int
test_case_5_3_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.cic = 12;
	send(RSC);
	state = 9;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 10;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 11;
	iut_mgm_signal(RESET_RES);
	state = 12;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 13;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_5_3_2 test_group_5_3
#define sgrp_case_5_3_2 NULL
#define numb_case_5_3_2 "5.3.2"
#define name_case_5_3_2 "Of an incoming circuit"
#define xtra_case_5_3_2 NULL
#define sref_case_5_3_2 NULL
#define desc_case_5_3_2 "\
Verify that on receipt of a reset message the call is immediately released --\n\
incoming call."
static int
test_case_5_3_2(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		goto failure;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		goto failure;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 5;
	if (pt_event() != ACM)
		goto failure;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		goto failure;
	state = 9;
	if (pt_event() != ANM)
		goto failure;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	send(RSC);
	state = 12;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 13;
	iut_mgm_signal(RESET_RES);
	state = 14;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 15;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return __RESULT_FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	send(RSC);
	return __RESULT_FAILURE;
}

#define test_group_6_1 "Continuity check call"

#define tgrp_case_6_1_1 test_group_6_1
#define sgrp_case_6_1_1 NULL
#define numb_case_6_1_1 "6.1.1"
#define name_case_6_1_1 "Continuity check required"
#define xtra_case_6_1_1 NULL
#define sref_case_6_1_1 NULL
#define desc_case_6_1_1 "\
Verify that a call can be set up on a circuit requiring a continuity check."
static int
test_case_6_1_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_tst_signal(TONE);
	state = 5;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 6;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 7;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 8;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 9;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 10;
	send(RINGING);
	state = 11;
	send(ANM);
	state = 12;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_1_2 test_group_6_1
#define sgrp_case_6_1_2 NULL
#define numb_case_6_1_2 "6.1.2"
#define name_case_6_1_2 "COT applied on a previous circuit"
#define xtra_case_6_1_2 NULL
#define sref_case_6_1_2 NULL
#define desc_case_6_1_2 "\
Verify that if a continuity check is being peformed on a previous circuit, a\n\
backward message is delayed until receipt of the COT message."
static int
test_case_6_1_2(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_PREVIOUS;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		goto failure;
	state = 2;
	beg_time = milliseconds(" Delay");
	state = 3;
	start_st(timer[t8].lo / 2);
	if (any_event() != __EVENT_TIMEOUT)
		goto failure;
	state = 4;
	pmsg.coti = ISUP_COT_SUCCESS;
	pmsg.opt.len = 0;
	send(COT);
	state = 5;
	if (cpc_event() != CPC_CONT_REPORT_IND)
		goto failure;
	state = 6;
	iut_cpc_signal(SETUP_RES);
	state = 7;
	if (cpc_event() != CPC_OK_ACK)
		goto failure;
	state = 8;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ACM)
		goto failure;
	state = 10;
	iut_cpc_signal(RINGING);
	state = 11;
	iut_cpc_signal(CONNECT_REQ);
	state = 12;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		goto failure;
	state = 13;
	if (pt_event() != ANM)
		goto failure;
	state = 14;
	iut_cpc_signal(COMMUNICATION);
	state = 15;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 16;
	if (cpc_event() != CPC_RELEASE_IND)
		goto failure;
	state = 17;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 18;
	if (pt_event() != RLC)
		goto failure;
	state = 19;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	send(RSC);
	return __RESULT_FAILURE;
}

#define tgrp_case_6_1_3 test_group_6_1
#define sgrp_case_6_1_3 NULL
#define numb_case_6_1_3 "6.1.3"
#define name_case_6_1_3 "Calling party clears during a COT"
#define xtra_case_6_1_3 NULL
#define sref_case_6_1_3 NULL
#define desc_case_6_1_3 "\
Verify that the calling party can successfully clear the call during the\n\
continuity check phase."
static int
test_case_6_1_3(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_cpc_signal(TONE);
	state = 5;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 6;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 7;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_1_4 test_group_6_1
#define sgrp_case_6_1_4 NULL
#define numb_case_6_1_4 "6.1.4"
#define name_case_6_1_4 "Delay of through connect"
#define xtra_case_6_1_4 NULL
#define sref_case_6_1_4 NULL
#define desc_case_6_1_4 "\
Verify that the switching through of the speech path is delayed until the\n\
residual check-tone has propagated through the return of the speech path."
static int
test_case_6_1_4(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_cpc_signal(TONE);
	state = 5;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 6;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 7;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 8;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 9;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 10;
	send(RINGING);
	state = 11;
	send(ANM);
	state = 12;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_1_5 test_group_6_1
#define sgrp_case_6_1_5 NULL
#define numb_case_6_1_5 "6.1.5"
#define name_case_6_1_5 "COT unsuccessful"
#define xtra_case_6_1_5 NULL
#define sref_case_6_1_5 NULL
#define desc_case_6_1_5 "\
Verify that a repeat attempt of the continuity check is made on the failed\n\
circuit."
static int
test_case_6_1_5(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = 12;
	cprim.addr.add.cic = 12;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_cpc_signal(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	start_st(timer[t24].hi + 100);
	state = 6;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return __RESULT_FAILURE;
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 9;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 10;
	beg_time = milliseconds(" T25  ");
	state = 11;
	start_st(timer[t25].hi + 100);
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 12;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 13;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 14;
	check_time(" T25  ", now() - beg_time, timer[t25].lo, timer[t25].hi);
	state = 15;
	send(LOOPBACK);
	state = 16;
	iut_tst_signal(TONE);
	state = 17;
	beg_time = milliseconds(" T24  ");
	state = 18;
	start_st(timer[t24].hi + 100);
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 19;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 20;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 21;
	beg_time = milliseconds(" T26  ");
	state = 22;
	start_st(timer[t26].hi + 100);
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 23;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 24;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 25;
	check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
	state = 26;
	send(LOOPBACK);
	state = 27;
	iut_tst_signal(TONE);
	state = 28;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 29;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 30;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define test_group_6_2 "Automatic repeat attempt"

#define tgrp_case_6_2_1 test_group_6_2
#define sgrp_case_6_2_1 NULL
#define numb_case_6_2_1 "6.2.1"
#define name_case_6_2_1 "Dual seizure for non-controlling SP"
#define xtra_case_6_2_1 NULL
#define sref_case_6_2_1 NULL
#define desc_case_6_2_1 "\
Verify that an automatic repeat attempt will be made on detection of a dual\n\
siezure."
static int
test_case_6_2_1(void)
{
	ulong cicx = 31, cicy = 30;

	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		goto failure;
	state = 2;
	cicx = pmsg.cic = imsg.cic;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 3;
	state = 4;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		goto failure;
	state = 5;
	if (cpc_event() != CPC_SETUP_IND)
		goto failure;
	state = 6;
	iut_cpc_signal(SETUP_RES);
	state = 7;
	if (cpc_event() != CPC_OK_ACK)
		goto failure;
	state = 8;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != ACM)
		goto failure;
	state = 10;
	iut_cpc_signal(RINGING);
	state = 11;
	iut_cpc_signal(CONNECT_REQ);
	state = 12;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		goto failure;
	state = 13;
	if (pt_event() != ANM)
		goto failure;
	state = 14;
	iut_cpc_signal(COMMUNICATION);
	state = 15;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 16;
	if (pt_event() != IAM)
		goto failure;
	state = 17;
	cicy = pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 18;
	if (cpc_event() != CPC_SETUP_CON)
		goto failure;
	cprim.user_ref = 0;
	state = 19;
	if (cpc_event() != CPC_ALERTING_IND)
		goto failure;
	state = 20;
	send(RINGING);
	state = 21;
	send(ANM);
	state = 22;
	if (cpc_event() != CPC_CONNECT_IND)
		goto failure;
	state = 23;
	iut_cpc_signal(COMMUNICATION);
	state = 24;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 25;
	if (pt_event() != REL)
		goto failure;
	state = 26;
	send(RLC);
	state = 27;
	if (cpc_event() != CPC_RELEASE_CON)
		goto failure;
	state = 28;
	pmsg.cic = cicx;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 29;
	if (cpc_event() != CPC_RELEASE_IND)
		goto failure;
	state = 30;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return __RESULT_FAILURE;
	state = 31;
	if (pt_event() != RLC)
		goto failure;
	state = 32;
	if (wait_event(0, ANY) != NO_MSG)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = cicy;
	send(RSC);
	pmsg.cic = cicx;
	send(RSC);
	return __RESULT_FAILURE;
}

#define tgrp_case_6_2_2 test_group_6_2
#define sgrp_case_6_2_2 NULL
#define numb_case_6_2_2 "6.2.2"
#define name_case_6_2_2 "Blocking of a circuit"
#define xtra_case_6_2_2 NULL
#define sref_case_6_2_2 NULL
#define desc_case_6_2_2 "\
Verify that an automatic repeat attempt will be made on receipt of the\n\
blocking message after sending an Initial address message and before any\n\
backward messsages have been received."
static int
test_case_6_2_2(void)
{
	ulong cic = 31;

	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	cic = pmsg.cic = imsg.cic;
	send(BLO);
	state = 4;
	if (mgm_event() != MGM_BLOCKING_IND)
		return __RESULT_FAILURE;
	state = 5;
	iut_mgm_signal(BLOCKING_RES);
	state = 6;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 7;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != BLA)
		return __RESULT_FAILURE;
	state = 9;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 10;
	send(RLC);
	state = 11;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 12;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 13;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 14;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 15;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 16;
	send(RINGING);
	state = 17;
	send(ANM);
	state = 18;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 19;
	iut_cpc_signal(COMMUNICATION);
	state = 20;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = imsg.cic;
	cprim.addr.add.cic = imsg.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.opt.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 21;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 22;
	send(RLC);
	state = 23;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 24;
	pmsg.cic = cic;
	send(UBL);
	state = 25;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return __RESULT_FAILURE;
	state = 26;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 27;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 28;
	if (pt_event() != UBA)
		return __RESULT_FAILURE;
	state = 29;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_2_3 test_group_6_2
#define sgrp_case_6_2_3 NULL
#define numb_case_6_2_3 "6.2.3"
#define name_case_6_2_3 "Circuit reset"
#define xtra_case_6_2_3 NULL
#define sref_case_6_2_3 NULL
#define desc_case_6_2_3 "\
Verify that an automatic repeat attempt will be made on receipt of circuit\n\
reset after sending of an Initial address message and before a backward\n\
message has been received."
static int
test_case_6_2_3(void)
{
	ulong cic = 31;

	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	cic = pmsg.cic = imsg.cic;
	send(RSC);
	state = 4;
	if (mgm_event() != MGM_RESET_IND)
		return __RESULT_FAILURE;
	state = 5;
	iut_mgm_signal(RESET_RES);
	state = 6;
	if (mgm_event() != MGM_OK_ACK)
		return __RESULT_FAILURE;
	state = 7;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != RLC)
		return __RESULT_FAILURE;
	state = 9;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 10;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 12;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 13;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 14;
	send(RINGING);
	state = 15;
	send(ANM);
	state = 16;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 17;
	iut_cpc_signal(COMMUNICATION);
	state = 18;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = imsg.cic;
	cprim.addr.add.cic = imsg.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.opt.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 19;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 20;
	send(RLC);
	state = 21;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 22;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_2_4 test_group_6_2
#define sgrp_case_6_2_4 NULL
#define numb_case_6_2_4 "6.2.4"
#define name_case_6_2_4 "Continuity check failure"
#define xtra_case_6_2_4 NULL
#define sref_case_6_2_4 NULL
#define desc_case_6_2_4 "\
Verify that an automatic repeat attempt will be made on continuity check\n\
failure."
static int
test_case_6_2_4(void)
{
	ulong cicx, cicy;

	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 3;
	cicx = imsg.cic;
	send(LOOPBACK);
	state = 4;
	iut_cpc_signal(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	state = 6;
	start_st(timer[t24].hi + 100);
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return __RESULT_FAILURE;
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 9;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 10;
	beg_time = milliseconds(" T25  ");
	state = 11;
	start_st(timer[t25].hi + 100);
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY | ISUP_NCI_CONT_CHECK_REQUIRED;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 12;
	if (pt_event() != IAM)
		return __RESULT_FAILURE;
	state = 13;
	send(LOOPBACK);
	state = 14;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 15;
	iut_tst_signal(TONE);
	state = 16;
	cicy = imsg.cic;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 17;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 18;
	pmsg.cic = cicy;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 19;
	if (cpc_event() != CPC_SETUP_CON)
		return __RESULT_FAILURE;
	cprim.user_ref = 0;
	state = 20;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 21;
	send(RINGING);
	state = 22;
	send(ANM);
	state = 23;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 24;
	iut_cpc_signal(COMMUNICATION);
	state = 25;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = imsg.cic;
	cprim.addr.add.cic = imsg.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.opt.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 26;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 27;
	send(RLC);
	state = 28;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 29;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 30;
	if (tst_event() != MGM_CONT_TEST_IND)
		return __RESULT_FAILURE;
	state = 31;
	iut_cpc_signal(TONE);
	state = 32;
	if (pt_event() != CCR)
		return __RESULT_FAILURE;
	state = 33;
	send(LOOPBACK);
	state = 34;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 35;
	if (pt_event() != COT)
		return __RESULT_FAILURE;
	state = 36;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_6_2_5 test_group_6_2
#define sgrp_case_6_2_5 NULL
#define numb_case_6_2_5 "6.2.5"
#define name_case_6_2_5 "Receipt of unreasonable signalling information"
#define xtra_case_6_2_5 NULL
#define sref_case_6_2_5 NULL
#define desc_case_6_2_5 "\
Verify that an automatic repeat attempt will be made on receipt of\n\
unreasonable signalling information after sending the Intitial address message\n\
and before one of the backward signals has been received."
static int
test_case_6_2_5(void)
{
	ulong cicx = 31, cicy = 30;

	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	state = 2;
	if (pt_event() != IAM)
		goto failure;
	state = 3;
	cicx = pmsg.cic = imsg.cic;
	send(RES);
	state = 4;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		goto failure;
	state = 5;
	if (mnt_event() != MGM_MAINT_IND)
		goto failure;
	state = 6;
	if (pt_event() != RSC)
		goto failure;
	send(RLC);
	state = 7;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 8;
	if (pt_event() != IAM)
		goto failure;
	state = 9;
	cicy = pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 10;
	if (cpc_event() != CPC_SETUP_CON)
		goto failure;
	cprim.user_ref = 0;
	state = 11;
	if (cpc_event() != CPC_ALERTING_IND)
		return __RESULT_FAILURE;
	state = 12;
	send(RINGING);
	state = 13;
	send(ANM);
	state = 14;
	if (cpc_event() != CPC_CONNECT_IND)
		return __RESULT_FAILURE;
	state = 15;
	iut_cpc_signal(COMMUNICATION);
	state = 16;
	cprim.addr.add.scope = ISUP_SCOPE_CT;
	cprim.addr.add.id = imsg.cic;
	cprim.addr.add.cic = imsg.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.opt.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 17;
	if (pt_event() != REL)
		return __RESULT_FAILURE;
	state = 18;
	send(RLC);
	state = 19;
	if (cpc_event() != CPC_RELEASE_CON)
		return __RESULT_FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = cicy;
	send(RSC);
	pmsg.cic = cicx;
	send(RSC);
	return __RESULT_FAILURE;
}

static int iut_option(void);

#define test_group_6_3 "Dual seizure"

#define tgrp_case_6_3_1 test_group_6_3
#define sgrp_case_6_3_1 NULL
#define numb_case_6_3_1 "6.3.1"
#define name_case_6_3_1 "Dual seizure for controlling SP"
#define xtra_case_6_3_1 NULL
#define sref_case_6_3_1 NULL
#define desc_case_6_3_1 "\
Verify that on detection of dual siezure, the call initiated by the\n\
controlling signalling point is completed and the non-controlling signalling\n\
point is backed off."
static int
test_case_6_3_1(void)
{
	ulong cicx = 31;

	iut_tg_opt.flags |= ISUP_TGF_GLARE_PRIORITY;
	if (iut_option() != __RESULT_SUCCESS)
		return __RESULT_INCONCLUSIVE;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags = ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
	cprim.cdpn.buf[0] = 0x00;
	cprim.cdpn.buf[1] = 0x10;
	cprim.cdpn.buf[2] = 0x71;
	cprim.cdpn.buf[3] = 0x08;
	cprim.cdpn.buf[4] = 0x94;
	cprim.cdpn.buf[5] = 0x10;
	cprim.cdpn.buf[6] = 0x41;
	cprim.cdpn.buf[7] = 0xf0;
	cprim.cdpn.len = 8;
	cprim.opt.buf[0] = ISUP_PT_CGPN;
	cprim.opt.buf[1] = 8;
	cprim.opt.buf[2] = 0x80;
	cprim.opt.buf[3] = 0x13;
	cprim.opt.buf[4] = 0x71;
	cprim.opt.buf[5] = 0x08;
	cprim.opt.buf[6] = 0x94;
	cprim.opt.buf[7] = 0x10;
	cprim.opt.buf[8] = 0x42;
	cprim.opt.buf[9] = 0x01;
	cprim.opt.len = 10;
	cprim.addr.add.scope = ISUP_SCOPE_TG;
	cprim.addr.add.id = 1;
	cprim.addr.add.cic = 0;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.user_ref = ++user_ref;
	cprim.call_ref = 0;
	iut_cpc_signal(SETUP_REQ);
	state = 1;
	if (pt_event() != IAM)
		goto failure;
	state = 2;
	cicx = pmsg.cic = imsg.cic;
	pmsg.nci = 0;
	pmsg.fci = (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 3;
	pmsg.bci = ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 4;
	if (cpc_event() != CPC_SETUP_CON)
		goto failure;
	cprim.user_ref = 0;
	state = 5;
	if (cpc_event() != CPC_ALERTING_IND)
		goto failure;
	state = 6;
	send(RINGING);
	state = 7;
	send(ANM);
	state = 8;
	if (cpc_event() != CPC_CONNECT_IND)
		goto failure;
	state = 9;
	iut_cpc_signal(COMMUNICATION);
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 11;
	if (pt_event() != REL)
		goto failure;
	state = 12;
	send(RLC);
	state = 13;
	if (cpc_event() != CPC_RELEASE_CON)
		goto failure;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	pmsg.cic = cicx;
	send(RSC);
	return __RESULT_FAILURE;
}

#define test_group_6_4 "Semi-automatic operation"

#define tgrp_case_6_4_1 test_group_6_4
#define sgrp_case_6_4_1 NULL
#define numb_case_6_4_1 "6.4.1"
#define name_case_6_4_1 "FOT sent following a call to a subscriber"
#define xtra_case_6_4_1 NULL
#define sref_case_6_4_1 NULL
#define desc_case_6_4_1 "\
Verify that the FOT is correctly sent."
static int
test_case_6_4_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_4_2 test_group_6_4
#define sgrp_case_6_4_2 NULL
#define numb_case_6_4_2 "6.4.2"
#define name_case_6_4_2 "FOT received following a call to a subscriber"
#define xtra_case_6_4_2 NULL
#define sref_case_6_4_2 NULL
#define desc_case_6_4_2 "\
Verify that the FOT is correctly received."
static int
test_case_6_4_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_4_3 test_group_6_4
#define sgrp_case_6_4_3 NULL
#define numb_case_6_4_3 "6.4.3"
#define name_case_6_4_3 "FOT sent following a call via codes 11 and 12"
#define xtra_case_6_4_3 NULL
#define sref_case_6_4_3 NULL
#define desc_case_6_4_3 "\
Verify that the FOT is correctly sent."
static int
test_case_6_4_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_4_4 test_group_6_4
#define sgrp_case_6_4_4 NULL
#define numb_case_6_4_4 "6.4.4"
#define name_case_6_4_4 "FOT received following a call via codes 11 and 12"
#define xtra_case_6_4_4 NULL
#define sref_case_6_4_4 NULL
#define desc_case_6_4_4 "\
Verify that the FOT is correctly received."
static int
test_case_6_4_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_6_5 "Simple segmentation"

#define tgrp_case_6_5_1 test_group_6_5
#define sgrp_case_6_5_1 NULL
#define numb_case_6_5_1 "6.5.1"
#define name_case_6_5_1 "Sending of SGM"
#define xtra_case_6_5_1 NULL
#define sref_case_6_5_1 NULL
#define desc_case_6_5_1 "\
Verify that a call can be successfully completed if segmentation is applied."
static int
test_case_6_5_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_5_2 test_group_6_5
#define sgrp_case_6_5_2 NULL
#define numb_case_6_5_2 "6.5.2"
#define name_case_6_5_2 "Receipt of SGM"
#define xtra_case_6_5_2 NULL
#define sref_case_6_5_2 NULL
#define desc_case_6_5_2 "\
Verify that a call can be successfully completed if segmentation is applied."
static int
test_case_6_5_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_5_3 test_group_6_5
#define sgrp_case_6_5_3 NULL
#define numb_case_6_5_3 "6.5.3"
#define name_case_6_5_3 "Receipt of SGM after timer T34 expired"
#define xtra_case_6_5_3 NULL
#define sref_case_6_5_3 NULL
#define desc_case_6_5_3 "\
Verify that a call can be successfully completed and the SGM will be\n\
discarded."
static int
test_case_6_5_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_5_4 test_group_6_5
#define sgrp_case_6_5_4 NULL
#define numb_case_6_5_4 "6.5.4"
#define name_case_6_5_4 "Receipt of SGM in forward direction"
#define xtra_case_6_5_4 NULL
#define sref_case_6_5_4 NULL
#define desc_case_6_5_4 "\
Verify that SP A is able to discard a SGM message without disrupting normal\n\
call handling."
static int
test_case_6_5_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_5_5 test_group_6_5
#define sgrp_case_6_5_5 NULL
#define numb_case_6_5_5 "6.5.5"
#define name_case_6_5_5 "Receipt of SGM in backward direction"
#define xtra_case_6_5_5 NULL
#define sref_case_6_5_5 NULL
#define desc_case_6_5_5 "\
Verify that SP A is able to discard a SGM message without disrupting normal\n\
call handling."
static int
test_case_6_5_5(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_6_6 "Fallback"

#define tgrp_case_6_6_1 test_group_6_6
#define sgrp_case_6_6_1 NULL
#define numb_case_6_6_1 "6.6.1"
#define name_case_6_6_1 "Fallback does not occur"
#define xtra_case_6_6_1 NULL
#define sref_case_6_6_1 NULL
#define desc_case_6_6_1 "\
Verify that a call can be successfully completed."
static int
test_case_6_6_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_6_2 test_group_6_6
#define sgrp_case_6_6_2 NULL
#define numb_case_6_6_2 "6.6.2"
#define name_case_6_6_2 "Fallback occurs behind SP A"
#define xtra_case_6_6_2 NULL
#define sref_case_6_6_2 NULL
#define desc_case_6_6_2 "\
Verify that a call can be successfully completed using Fallback that was\n\
indicated behind SP A."
static int
test_case_6_6_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_6_3 test_group_6_6
#define sgrp_case_6_6_3 NULL
#define numb_case_6_6_3 "6.6.3"
#define name_case_6_6_3 "Fallback occurs in SP A"
#define xtra_case_6_6_3 NULL
#define sref_case_6_6_3 NULL
#define desc_case_6_6_3 "\
Verify that SP A is able to perform fallback."
static int
test_case_6_6_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_6_6_4 test_group_6_6
#define sgrp_case_6_6_4 NULL
#define numb_case_6_6_4 "6.6.4"
#define name_case_6_6_4 "Abnormal procedure, Fallback connection types sent to an exchange not supporting the fallback procedure"
#define xtra_case_6_6_4 NULL
#define sref_case_6_6_4 NULL
#define desc_case_6_6_4 "\
Verify that SP A is able to release the call."
static int
test_case_6_6_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_7_1 "64 kbit/s unrestricted"

#define tgrp_case_7_1_1 test_group_7_1
#define sgrp_case_7_1_1 NULL
#define numb_case_7_1_1 "7.1.1"
#define name_case_7_1_1 "Successful call setup"
#define xtra_case_7_1_1 NULL
#define sref_case_7_1_1 NULL
#define desc_case_7_1_1 "\
Verify that a 64 kbit/s call can be successfully completed using appropriate\n\
transmission medium requirement and user service information parameters."
static int
test_case_7_1_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_1_2 test_group_7_1
#define sgrp_case_7_1_2 NULL
#define numb_case_7_1_2 "7.1.2"
#define name_case_7_1_2 "Unsuccessful call setup"
#define xtra_case_7_1_2 NULL
#define sref_case_7_1_2 NULL
#define desc_case_7_1_2 "\
Verify that the call will be immediately released by the outgoing SP if a\n\
release message with a given cause is received and, for circuits equipped with\n\
echo control, the echo control device is enabled."
static int
test_case_7_1_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_1_3 test_group_7_1
#define sgrp_case_7_1_3 NULL
#define numb_case_7_1_3 "7.1.3"
#define name_case_7_1_3 "Dual siezure"
#define xtra_case_7_1_3 NULL
#define sref_case_7_1_3 NULL
#define desc_case_7_1_3 "\
Verify that an automatic repeat attempt will be made on detection of dual\n\
siezure."
static int
test_case_7_1_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_7_2 "3.1 kHz audio"

#define tgrp_case_7_2_1 test_group_7_2
#define sgrp_case_7_2_1 NULL
#define numb_case_7_2_1 "7.2.1"
#define name_case_7_2_1 "Successful call setup"
#define xtra_case_7_2_1 NULL
#define sref_case_7_2_1 NULL
#define desc_case_7_2_1 "\
Verify that a 3.1 kHz audio call can be successfully completed using\n\
appropriate transmission medium requirement and user service information\n\
parameters."
static int
test_case_7_2_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_7_3 "Multirate connection types"

#define tgrp_case_7_3_1 test_group_7_3
#define sgrp_case_7_3_1 NULL
#define numb_case_7_3_1 "7.3.1"
#define name_case_7_3_1 "Successful multirate outgoing call setup"
#define xtra_case_7_3_1 NULL
#define sref_case_7_3_1 NULL
#define desc_case_7_3_1 "\
Verify that SP A is able to set up an outgoing call with a multirate bearer\n\
service."
static int
test_case_7_3_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_3_2 test_group_7_3
#define sgrp_case_7_3_2 NULL
#define numb_case_7_3_2 "7.3.2"
#define name_case_7_3_2 "Successful multirate incoming call setup"
#define xtra_case_7_3_2 NULL
#define sref_case_7_3_2 NULL
#define desc_case_7_3_2 "\
Verify that SP A is able to handle an incoming call with a multirate bearer\n\
service."
static int
test_case_7_3_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_3_3 test_group_7_3
#define sgrp_case_7_3_3 NULL
#define numb_case_7_3_3 "7.3.3"
#define name_case_7_3_3 "Unsuccessful multirate incoming call setup -- one circuit already busy"
#define xtra_case_7_3_3 NULL
#define sref_case_7_3_3 NULL
#define desc_case_7_3_3 "\
Verify that a 1920 kbit/s multirate call is rejected by SP A is one of the\n\
circuits necessary for the call is already busy."
static int
test_case_7_3_3(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_3_4 test_group_7_3
#define sgrp_case_7_3_4 NULL
#define numb_case_7_3_4 "7.3.4"
#define name_case_7_3_4 "Dual seizure of different connection types: Controlling exchange"
#define xtra_case_7_3_4 NULL
#define sref_case_7_3_4 NULL
#define desc_case_7_3_4 "\
Verify that SP A is able to detect dual seizure for calls for different\n\
multirate connection types and it completes the call involving the greater\n\
number of circuits."
static int
test_case_7_3_4(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_3_5 test_group_7_3
#define sgrp_case_7_3_5 NULL
#define numb_case_7_3_5 "7.3.5"
#define name_case_7_3_5 "Dual seizure of connection types: Non-controlling exchange"
#define xtra_case_7_3_5 NULL
#define sref_case_7_3_5 NULL
#define desc_case_7_3_5 "\
Verify that SP A is able to detect dual seizure for calls of different\n\
multirate connection types and it reattempts the call involving the smaller\n\
number of circuits."
static int
test_case_7_3_5(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_7_3_6 test_group_7_3
#define sgrp_case_7_3_6 NULL
#define numb_case_7_3_6 "7.3.6"
#define name_case_7_3_6 "Abnormal procedure, Multirate connection types call sent to an exchange not supporting the procedure"
#define xtra_case_7_3_6 NULL
#define sref_case_7_3_6 NULL
#define desc_case_7_3_6 "\
Verify that SP A is able to release the call."
static int
test_case_7_3_6(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_8_1 "Automatic congestion control"

#define tgrp_case_8_1_1 test_group_8_1
#define sgrp_case_8_1_1 NULL
#define numb_case_8_1_1 "8.1.1"
#define name_case_8_1_1 "Receipt of a release message containing an automatic congestion level parameter"
#define xtra_case_8_1_1 NULL
#define sref_case_8_1_1 NULL
#define desc_case_8_1_1 "\
Verify that the adjacent exchange (SP A), after receiving a release message\n\
containing an automatic congestion level parameter reduces the traffic to the\n\
overload affected exchange (SP B)."
static int
test_case_8_1_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_8_1_2 test_group_8_1
#define sgrp_case_8_1_2 NULL
#define numb_case_8_1_2 "8.1.2"
#define name_case_8_1_2 "Sending of a release message containing an automatic congestion level parameter"
#define xtra_case_8_1_2 NULL
#define sref_case_8_1_2 NULL
#define desc_case_8_1_2 "\
Verify that the SP A is able to send a release message containing an automatic\n\
congestion level parameter."
static int
test_case_8_1_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define test_group_8_2 "ISUP availability control"

#define tgrp_case_8_2_1 test_group_8_2
#define sgrp_case_8_2_1 NULL
#define numb_case_8_2_1 "8.2.1"
#define name_case_8_2_1 "Receipt of an UPT"
#define xtra_case_8_2_1 NULL
#define sref_case_8_2_1 NULL
#define desc_case_8_2_1 "\
Verify that on receipt of a user part test message SP A will respond by\n\
sending a user part available message."
static int
test_case_8_2_1(void)
{
	state = 0;
	send(UPT);
	if (any_event() != UPA)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_8_2_2 test_group_8_2
#define sgrp_case_8_2_2 NULL
#define numb_case_8_2_2 "8.2.2"
#define name_case_8_2_2 "Sending of a UPT"
#define xtra_case_8_2_2 NULL
#define sref_case_8_2_2 NULL
#define desc_case_8_2_2 "\
Verify that SP A is able to send a user part test message."
static int
test_case_8_2_2(void)
{
	state = 0;
	send(USER_PART_UNAVAILABLE);
	state = 1;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != UPT)
		return __RESULT_FAILURE;
	state = 3;
	send(UPA);
	state = 4;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

#define tgrp_case_8_2_3 test_group_8_2
#define sgrp_case_8_2_3 NULL
#define numb_case_8_2_3 "8.2.3"
#define name_case_8_2_3 "T4: failure to receive a response to a UPT"
#define xtra_case_8_2_3 NULL
#define sref_case_8_2_3 NULL
#define desc_case_8_2_3 "\
Verify that SP A is able to restart the availability test procedure after the\n\
expiry of timer T4."
static int
test_case_8_2_3(void)
{
	long upt_time = now(), t4_time = now();

	state = 0;
	send(USER_PART_UNAVAILABLE);
	state = 1;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 2;
	if (pt_event() != UPT)
		return __RESULT_FAILURE;
	state = 3;
	upt_time = now();
	t4_time = milliseconds("  T4  ");
	state = 4;
	start_st(timer[t4].hi);
	if (pt_event() != UPT)
		return __RESULT_FAILURE;
	state = 5;
	upt_time = now();
	if (check_time("  T4  ", upt_time - t4_time, timer[t4].lo, timer[t4].hi) != __RESULT_SUCCESS)
		goto failure;
	state = 6;
	send(UPA);
	state = 7;
	if (mnt_event() != MGM_MAINT_IND)
		return __RESULT_FAILURE;
	state = 8;
	if (wait_event(0, ANY) != NO_MSG)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
      failure:
	send(UPA);
	return __RESULT_FAILURE;
}

#define test_group_9_1 "Echo control procedures according to Q.767"

#define tgrp_case_9_1_1 test_group_9_1
#define sgrp_case_9_1_1 NULL
#define numb_case_9_1_1 "9.1.1"
#define name_case_9_1_1 "Q.767 echo control procedure for call set up (initiated in SP A)"
#define xtra_case_9_1_1 NULL
#define sref_case_9_1_1 NULL
#define desc_case_9_1_1 "\
Verify that a call can be successfully established with the inclusion of echo\n\
control devices."
static int
test_case_9_1_1(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
}

#define tgrp_case_9_1_2 test_group_9_1
#define sgrp_case_9_1_2 NULL
#define numb_case_9_1_2 "9.1.2"
#define name_case_9_1_2 "Q.767 echo control procedure for call set up (initiated in SP B)"
#define xtra_case_9_1_2 NULL
#define sref_case_9_1_2 NULL
#define desc_case_9_1_2 "\
Verify that a call can be successfully established, if SP A does not include\n\
an outgoing half echo control device."
static int
test_case_9_1_2(void)
{
	state = 0;
	return __RESULT_INCONCLUSIVE;
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
	int (*preamble) (void);		/* test preamble */
	int (*testcase) (void);		/* test case */
	int (*postamble) (void);	/* test postamble */
	ulong duration;			/* maximum duration */
	int run;			/* whether to run this test */
	int result;			/* results of test */
	int expect;			/* expected result */
} tests[] = {
	{
	numb_case_1_1, tgrp_case_1_1, sgrp_case_1_1, name_case_1_1, xtra_case_1_1, desc_case_1_1, sref_case_1_1, &preamble_0, &test_case_1_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_1, tgrp_case_1_2_1, sgrp_case_1_2_1, name_case_1_2_1, xtra_case_1_2_1, desc_case_1_2_1, sref_case_1_2_1, &preamble_0, &test_case_1_2_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_2, tgrp_case_1_2_2, sgrp_case_1_2_2, name_case_1_2_2, xtra_case_1_2_2, desc_case_1_2_2, sref_case_1_2_2, &preamble_0, &test_case_1_2_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_3, tgrp_case_1_2_3, sgrp_case_1_2_3, name_case_1_2_3, xtra_case_1_2_3, desc_case_1_2_3, sref_case_1_2_3, &preamble_0, &test_case_1_2_3, &postamble_1, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_4, tgrp_case_1_2_4, sgrp_case_1_2_4, name_case_1_2_4, xtra_case_1_2_4, desc_case_1_2_4, sref_case_1_2_4, &preamble_0, &test_case_1_2_4, &postamble_2, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_5a, tgrp_case_1_2_5a, sgrp_case_1_2_5a, name_case_1_2_5a, xtra_case_1_2_5a, desc_case_1_2_5a, sref_case_1_2_5a, &preamble_1, &test_case_1_2_5a, &postamble_1, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_5b, tgrp_case_1_2_5b, sgrp_case_1_2_5b, name_case_1_2_5b, xtra_case_1_2_5b, desc_case_1_2_5b, sref_case_1_2_5b, &preamble_0, &test_case_1_2_5b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_5c, tgrp_case_1_2_5c, sgrp_case_1_2_5c, name_case_1_2_5c, xtra_case_1_2_5c, desc_case_1_2_5c, sref_case_1_2_5c, &preamble_0, &test_case_1_2_5c, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_6, tgrp_case_1_2_6, sgrp_case_1_2_6, name_case_1_2_6, xtra_case_1_2_6, desc_case_1_2_6, sref_case_1_2_6, &preamble_0, &test_case_1_2_6, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_2_7, tgrp_case_1_2_7, sgrp_case_1_2_7, name_case_1_2_7, xtra_case_1_2_7, desc_case_1_2_7, sref_case_1_2_7, &preamble_0, &test_case_1_2_7, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1a, tgrp_case_1_3_1_1a, sgrp_case_1_3_1_1a, name_case_1_3_1_1a, xtra_case_1_3_1_1a, desc_case_1_3_1_1a, sref_case_1_3_1_1a, &preamble_0, &test_case_1_3_1_1a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1b, tgrp_case_1_3_1_1b, sgrp_case_1_3_1_1b, name_case_1_3_1_1b, xtra_case_1_3_1_1b, desc_case_1_3_1_1b, sref_case_1_3_1_1b, &preamble_0, &test_case_1_3_1_1b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1c, tgrp_case_1_3_1_1c, sgrp_case_1_3_1_1c, name_case_1_3_1_1c, xtra_case_1_3_1_1c, desc_case_1_3_1_1c, sref_case_1_3_1_1c, &preamble_0, &test_case_1_3_1_1c, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1d, tgrp_case_1_3_1_1d, sgrp_case_1_3_1_1d, name_case_1_3_1_1d, xtra_case_1_3_1_1d, desc_case_1_3_1_1d, sref_case_1_3_1_1d, &preamble_0, &test_case_1_3_1_1d, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1e, tgrp_case_1_3_1_1e, sgrp_case_1_3_1_1e, name_case_1_3_1_1e, xtra_case_1_3_1_1e, desc_case_1_3_1_1e, sref_case_1_3_1_1e, &preamble_0, &test_case_1_3_1_1e, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_1f, tgrp_case_1_3_1_1f, sgrp_case_1_3_1_1f, name_case_1_3_1_1f, xtra_case_1_3_1_1f, desc_case_1_3_1_1f, sref_case_1_3_1_1f, &preamble_0, &test_case_1_3_1_1f, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_2a, tgrp_case_1_3_1_2a, sgrp_case_1_3_1_2a, name_case_1_3_1_2a, xtra_case_1_3_1_2a, desc_case_1_3_1_2a, sref_case_1_3_1_2a, &preamble_0, &test_case_1_3_1_2a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_1_2b, tgrp_case_1_3_1_2b, sgrp_case_1_3_1_2b, name_case_1_3_1_2b, xtra_case_1_3_1_2b, desc_case_1_3_1_2b, sref_case_1_3_1_2b, &preamble_0, &test_case_1_3_1_2b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_2_1, tgrp_case_1_3_2_1, sgrp_case_1_3_2_1, name_case_1_3_2_1, xtra_case_1_3_2_1, desc_case_1_3_2_1, sref_case_1_3_2_1, &preamble_0, &test_case_1_3_2_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_2_2, tgrp_case_1_3_2_2, sgrp_case_1_3_2_2, name_case_1_3_2_2, xtra_case_1_3_2_2, desc_case_1_3_2_2, sref_case_1_3_2_2, &preamble_0, &test_case_1_3_2_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_2_3, tgrp_case_1_3_2_3, sgrp_case_1_3_2_3, name_case_1_3_2_3, xtra_case_1_3_2_3, desc_case_1_3_2_3, sref_case_1_3_2_3, &preamble_0, &test_case_1_3_2_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_2_4, tgrp_case_1_3_2_4, sgrp_case_1_3_2_4, name_case_1_3_2_4, xtra_case_1_3_2_4, desc_case_1_3_2_4, sref_case_1_3_2_4, &preamble_0, &test_case_1_3_2_4, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_3_2_5, tgrp_case_1_3_2_5, sgrp_case_1_3_2_5, name_case_1_3_2_5, xtra_case_1_3_2_5, desc_case_1_3_2_5, sref_case_1_3_2_5, &preamble_0, &test_case_1_3_2_5, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_4_1, tgrp_case_1_4_1, sgrp_case_1_4_1, name_case_1_4_1, xtra_case_1_4_1, desc_case_1_4_1, sref_case_1_4_1, &preamble_0, &test_case_1_4_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_4_2, tgrp_case_1_4_2, sgrp_case_1_4_2, name_case_1_4_2, xtra_case_1_4_2, desc_case_1_4_2, sref_case_1_4_2, &preamble_0, &test_case_1_4_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_4_3, tgrp_case_1_4_3, sgrp_case_1_4_3, name_case_1_4_3, xtra_case_1_4_3, desc_case_1_4_3, sref_case_1_4_3, &preamble_0, &test_case_1_4_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_4_4, tgrp_case_1_4_4, sgrp_case_1_4_4, name_case_1_4_4, xtra_case_1_4_4, desc_case_1_4_4, sref_case_1_4_4, &preamble_0, &test_case_1_4_4, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_4_5, tgrp_case_1_4_5, sgrp_case_1_4_5, name_case_1_4_5, xtra_case_1_4_5, desc_case_1_4_5, sref_case_1_4_5, &preamble_0, &test_case_1_4_5, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_1_5_1, tgrp_case_1_5_1, sgrp_case_1_5_1, name_case_1_5_1, xtra_case_1_5_1, desc_case_1_5_1, sref_case_1_5_1, &preamble_0, &test_case_1_5_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_5_2, tgrp_case_1_5_2, sgrp_case_1_5_2, name_case_1_5_2, xtra_case_1_5_2, desc_case_1_5_2, sref_case_1_5_2, &preamble_0, &test_case_1_5_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_5_3, tgrp_case_1_5_3, sgrp_case_1_5_3, name_case_1_5_3, xtra_case_1_5_3, desc_case_1_5_3, sref_case_1_5_3, &preamble_0, &test_case_1_5_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_1_1, tgrp_case_1_6_1_1, sgrp_case_1_6_1_1, name_case_1_6_1_1, xtra_case_1_6_1_1, desc_case_1_6_1_1, sref_case_1_6_1_1, &preamble_0, &test_case_1_6_1_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_1_2, tgrp_case_1_6_1_2, sgrp_case_1_6_1_2, name_case_1_6_1_2, xtra_case_1_6_1_2, desc_case_1_6_1_2, sref_case_1_6_1_2, &preamble_0, &test_case_1_6_1_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_2_1, tgrp_case_1_6_2_1, sgrp_case_1_6_2_1, name_case_1_6_2_1, xtra_case_1_6_2_1, desc_case_1_6_2_1, sref_case_1_6_2_1, &preamble_0, &test_case_1_6_2_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_2_2, tgrp_case_1_6_2_2, sgrp_case_1_6_2_2, name_case_1_6_2_2, xtra_case_1_6_2_2, desc_case_1_6_2_2, sref_case_1_6_2_2, &preamble_0, &test_case_1_6_2_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_3_1, tgrp_case_1_6_3_1, sgrp_case_1_6_3_1, name_case_1_6_3_1, xtra_case_1_6_3_1, desc_case_1_6_3_1, sref_case_1_6_3_1, &preamble_0, &test_case_1_6_3_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_6_3_2, tgrp_case_1_6_3_2, sgrp_case_1_6_3_2, name_case_1_6_3_2, xtra_case_1_6_3_2, desc_case_1_6_3_2, sref_case_1_6_3_2, &preamble_0, &test_case_1_6_3_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_1, tgrp_case_1_7_1_1, sgrp_case_1_7_1_1, name_case_1_7_1_1, xtra_case_1_7_1_1, desc_case_1_7_1_1, sref_case_1_7_1_1, &preamble_0, &test_case_1_7_1_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_2, tgrp_case_1_7_1_2, sgrp_case_1_7_1_2, name_case_1_7_1_2, xtra_case_1_7_1_2, desc_case_1_7_1_2, sref_case_1_7_1_2, &preamble_0, &test_case_1_7_1_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_3, tgrp_case_1_7_1_3, sgrp_case_1_7_1_3, name_case_1_7_1_3, xtra_case_1_7_1_3, desc_case_1_7_1_3, sref_case_1_7_1_3, &preamble_0, &test_case_1_7_1_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_4, tgrp_case_1_7_1_4, sgrp_case_1_7_1_4, name_case_1_7_1_4, xtra_case_1_7_1_4, desc_case_1_7_1_4, sref_case_1_7_1_4, &preamble_0, &test_case_1_7_1_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_5, tgrp_case_1_7_1_5, sgrp_case_1_7_1_5, name_case_1_7_1_5, xtra_case_1_7_1_5, desc_case_1_7_1_5, sref_case_1_7_1_5, &preamble_0, &test_case_1_7_1_5, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_6, tgrp_case_1_7_1_6, sgrp_case_1_7_1_6, name_case_1_7_1_6, xtra_case_1_7_1_6, desc_case_1_7_1_6, sref_case_1_7_1_6, &preamble_0, &test_case_1_7_1_6, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_1_7, tgrp_case_1_7_1_7, sgrp_case_1_7_1_7, name_case_1_7_1_7, xtra_case_1_7_1_7, desc_case_1_7_1_7, sref_case_1_7_1_7, &preamble_0, &test_case_1_7_1_7, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_1, tgrp_case_1_7_2_1, sgrp_case_1_7_2_1, name_case_1_7_2_1, xtra_case_1_7_2_1, desc_case_1_7_2_1, sref_case_1_7_2_1, &preamble_0, &test_case_1_7_2_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_3, tgrp_case_1_7_2_3, sgrp_case_1_7_2_3, name_case_1_7_2_3, xtra_case_1_7_2_3, desc_case_1_7_2_3, sref_case_1_7_2_3, &preamble_0, &test_case_1_7_2_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_4, tgrp_case_1_7_2_4, sgrp_case_1_7_2_4, name_case_1_7_2_4, xtra_case_1_7_2_4, desc_case_1_7_2_4, sref_case_1_7_2_4, &preamble_0, &test_case_1_7_2_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_5, tgrp_case_1_7_2_5, sgrp_case_1_7_2_5, name_case_1_7_2_5, xtra_case_1_7_2_5, desc_case_1_7_2_5, sref_case_1_7_2_5, &preamble_0, &test_case_1_7_2_5, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_6, tgrp_case_1_7_2_6, sgrp_case_1_7_2_6, name_case_1_7_2_6, xtra_case_1_7_2_6, desc_case_1_7_2_6, sref_case_1_7_2_6, &preamble_0, &test_case_1_7_2_6, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_7, tgrp_case_1_7_2_7, sgrp_case_1_7_2_7, name_case_1_7_2_7, xtra_case_1_7_2_7, desc_case_1_7_2_7, sref_case_1_7_2_7, &preamble_0, &test_case_1_7_2_7, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_8, tgrp_case_1_7_2_8, sgrp_case_1_7_2_8, name_case_1_7_2_8, xtra_case_1_7_2_8, desc_case_1_7_2_8, sref_case_1_7_2_8, &preamble_0, &test_case_1_7_2_8, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_9, tgrp_case_1_7_2_9, sgrp_case_1_7_2_9, name_case_1_7_2_9, xtra_case_1_7_2_9, desc_case_1_7_2_9, sref_case_1_7_2_9, &preamble_0, &test_case_1_7_2_9, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_2_10, tgrp_case_1_7_2_10, sgrp_case_1_7_2_10, name_case_1_7_2_10, xtra_case_1_7_2_10, desc_case_1_7_2_10, sref_case_1_7_2_10, &preamble_0, &test_case_1_7_2_10, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_3_1, tgrp_case_1_7_3_1, sgrp_case_1_7_3_1, name_case_1_7_3_1, xtra_case_1_7_3_1, desc_case_1_7_3_1, sref_case_1_7_3_1, &preamble_0, &test_case_1_7_3_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_1_7_3_2, tgrp_case_1_7_3_2, sgrp_case_1_7_3_2, name_case_1_7_3_2, xtra_case_1_7_3_2, desc_case_1_7_3_2, sref_case_1_7_3_2, &preamble_0, &test_case_1_7_3_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_2_1_1, tgrp_case_2_1_1, sgrp_case_2_1_1, name_case_2_1_1, xtra_case_2_1_1, desc_case_2_1_1, sref_case_2_1_1, &preamble_0, &test_case_2_1_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_1_2, tgrp_case_2_1_2, sgrp_case_2_1_2, name_case_2_1_2, xtra_case_2_1_2, desc_case_2_1_2, sref_case_2_1_2, &preamble_0, &test_case_2_1_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_2_1a, tgrp_case_2_2_1a, sgrp_case_2_2_1a, name_case_2_2_1a, xtra_case_2_2_1a, desc_case_2_2_1a, sref_case_2_2_1a, &preamble_0, &test_case_2_2_1a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_2_1b, tgrp_case_2_2_1b, sgrp_case_2_2_1b, name_case_2_2_1b, xtra_case_2_2_1b, desc_case_2_2_1b, sref_case_2_2_1b, &preamble_0, &test_case_2_2_1b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_2_2a, tgrp_case_2_2_2a, sgrp_case_2_2_2a, name_case_2_2_2a, xtra_case_2_2_2a, desc_case_2_2_2a, sref_case_2_2_2a, &preamble_0, &test_case_2_2_2a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_2_2b, tgrp_case_2_2_2b, sgrp_case_2_2_2b, name_case_2_2_2b, xtra_case_2_2_2b, desc_case_2_2_2b, sref_case_2_2_2b, &preamble_0, &test_case_2_2_2b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1a, tgrp_case_2_3_1a, sgrp_case_2_3_1a, name_case_2_3_1a, xtra_case_2_3_1a, desc_case_2_3_1a, sref_case_2_3_1a, &preamble_0, &test_case_2_3_1a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1b, tgrp_case_2_3_1b, sgrp_case_2_3_1b, name_case_2_3_1b, xtra_case_2_3_1b, desc_case_2_3_1b, sref_case_2_3_1b, &preamble_0, &test_case_2_3_1b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1c, tgrp_case_2_3_1c, sgrp_case_2_3_1c, name_case_2_3_1c, xtra_case_2_3_1c, desc_case_2_3_1c, sref_case_2_3_1c, &preamble_0, &test_case_2_3_1c, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1d, tgrp_case_2_3_1d, sgrp_case_2_3_1d, name_case_2_3_1d, xtra_case_2_3_1d, desc_case_2_3_1d, sref_case_2_3_1d, &preamble_0, &test_case_2_3_1d, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1e, tgrp_case_2_3_1e, sgrp_case_2_3_1e, name_case_2_3_1e, xtra_case_2_3_1e, desc_case_2_3_1e, sref_case_2_3_1e, &preamble_0, &test_case_2_3_1e, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1f, tgrp_case_2_3_1f, sgrp_case_2_3_1f, name_case_2_3_1f, xtra_case_2_3_1f, desc_case_2_3_1f, sref_case_2_3_1f, &preamble_0, &test_case_2_3_1f, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1g, tgrp_case_2_3_1g, sgrp_case_2_3_1g, name_case_2_3_1g, xtra_case_2_3_1g, desc_case_2_3_1g, sref_case_2_3_1g, &preamble_0, &test_case_2_3_1g, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_1h, tgrp_case_2_3_1h, sgrp_case_2_3_1h, name_case_2_3_1h, xtra_case_2_3_1h, desc_case_2_3_1h, sref_case_2_3_1h, &preamble_0, &test_case_2_3_1h, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2a, tgrp_case_2_3_2a, sgrp_case_2_3_2a, name_case_2_3_2a, xtra_case_2_3_2a, desc_case_2_3_2a, sref_case_2_3_2a, &preamble_0, &test_case_2_3_2a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2b, tgrp_case_2_3_2b, sgrp_case_2_3_2b, name_case_2_3_2b, xtra_case_2_3_2b, desc_case_2_3_2b, sref_case_2_3_2b, &preamble_0, &test_case_2_3_2b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2c, tgrp_case_2_3_2c, sgrp_case_2_3_2c, name_case_2_3_2c, xtra_case_2_3_2c, desc_case_2_3_2c, sref_case_2_3_2c, &preamble_0, &test_case_2_3_2c, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2d, tgrp_case_2_3_2d, sgrp_case_2_3_2d, name_case_2_3_2d, xtra_case_2_3_2d, desc_case_2_3_2d, sref_case_2_3_2d, &preamble_0, &test_case_2_3_2d, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2e, tgrp_case_2_3_2e, sgrp_case_2_3_2e, name_case_2_3_2e, xtra_case_2_3_2e, desc_case_2_3_2e, sref_case_2_3_2e, &preamble_0, &test_case_2_3_2e, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_2f, tgrp_case_2_3_2f, sgrp_case_2_3_2f, name_case_2_3_2f, xtra_case_2_3_2f, desc_case_2_3_2f, sref_case_2_3_2f, &preamble_0, &test_case_2_3_2f, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_3a, tgrp_case_2_3_3a, sgrp_case_2_3_3a, name_case_2_3_3a, xtra_case_2_3_3a, desc_case_2_3_3a, sref_case_2_3_3a, &preamble_0, &test_case_2_3_3a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_3b, tgrp_case_2_3_3b, sgrp_case_2_3_3b, name_case_2_3_3b, xtra_case_2_3_3b, desc_case_2_3_3b, sref_case_2_3_3b, &preamble_0, &test_case_2_3_3b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_4a, tgrp_case_2_3_4a, sgrp_case_2_3_4a, name_case_2_3_4a, xtra_case_2_3_4a, desc_case_2_3_4a, sref_case_2_3_4a, &preamble_0, &test_case_2_3_4a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_4b, tgrp_case_2_3_4b, sgrp_case_2_3_4b, name_case_2_3_4b, xtra_case_2_3_4b, desc_case_2_3_4b, sref_case_2_3_4b, &preamble_0, &test_case_2_3_4b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_5a, tgrp_case_2_3_5a, sgrp_case_2_3_5a, name_case_2_3_5a, xtra_case_2_3_5a, desc_case_2_3_5a, sref_case_2_3_5a, &preamble_0, &test_case_2_3_5a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_5b, tgrp_case_2_3_5b, sgrp_case_2_3_5b, name_case_2_3_5b, xtra_case_2_3_5b, desc_case_2_3_5b, sref_case_2_3_5b, &preamble_0, &test_case_2_3_5b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_6a, tgrp_case_2_3_6a, sgrp_case_2_3_6a, name_case_2_3_6a, xtra_case_2_3_6a, desc_case_2_3_6a, sref_case_2_3_6a, &preamble_0, &test_case_2_3_6a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_3_6b, tgrp_case_2_3_6b, sgrp_case_2_3_6b, name_case_2_3_6b, xtra_case_2_3_6b, desc_case_2_3_6b, sref_case_2_3_6b, &preamble_0, &test_case_2_3_6b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_2_4_1, tgrp_case_2_4_1, sgrp_case_2_4_1, name_case_2_4_1, xtra_case_2_4_1, desc_case_2_4_1, sref_case_2_4_1, &preamble_0, &test_case_2_4_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_2_4_2, tgrp_case_2_4_2, sgrp_case_2_4_2, name_case_2_4_2, xtra_case_2_4_2, desc_case_2_4_2, sref_case_2_4_2, &preamble_0, &test_case_2_4_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_2_4_3, tgrp_case_2_4_3, sgrp_case_2_4_3, name_case_2_4_3, xtra_case_2_4_3, desc_case_2_4_3, sref_case_2_4_3, &preamble_0, &test_case_2_4_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_2_4_4, tgrp_case_2_4_4, sgrp_case_2_4_4, name_case_2_4_4, xtra_case_2_4_4, desc_case_2_4_4, sref_case_2_4_4, &preamble_0, &test_case_2_4_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_2_4_5, tgrp_case_2_4_5, sgrp_case_2_4_5, name_case_2_4_5, xtra_case_2_4_5, desc_case_2_4_5, sref_case_2_4_5, &preamble_0, &test_case_2_4_5, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_3_1a, tgrp_case_3_1a, sgrp_case_3_1a, name_case_3_1a, xtra_case_3_1a, desc_case_3_1a, sref_case_3_1a, &preamble_0, &test_case_3_1a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_1b, tgrp_case_3_1b, sgrp_case_3_1b, name_case_3_1b, xtra_case_3_1b, desc_case_3_1b, sref_case_3_1b, &preamble_0, &test_case_3_1b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_2a, tgrp_case_3_2a, sgrp_case_3_2a, name_case_3_2a, xtra_case_3_2a, desc_case_3_2a, sref_case_3_2a, &preamble_0, &test_case_3_2a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_2b, tgrp_case_3_2b, sgrp_case_3_2b, name_case_3_2b, xtra_case_3_2b, desc_case_3_2b, sref_case_3_2b, &preamble_0, &test_case_3_2b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_3a, tgrp_case_3_3a, sgrp_case_3_3a, name_case_3_3a, xtra_case_3_3a, desc_case_3_3a, sref_case_3_3a, &preamble_0, &test_case_3_3a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_3b, tgrp_case_3_3b, sgrp_case_3_3b, name_case_3_3b, xtra_case_3_3b, desc_case_3_3b, sref_case_3_3b, &preamble_0, &test_case_3_3b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_4a, tgrp_case_3_4a, sgrp_case_3_4a, name_case_3_4a, xtra_case_3_4a, desc_case_3_4a, sref_case_3_4a, &preamble_0, &test_case_3_4a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_4b, tgrp_case_3_4b, sgrp_case_3_4b, name_case_3_4b, xtra_case_3_4b, desc_case_3_4b, sref_case_3_4b, &preamble_0, &test_case_3_4b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_5a, tgrp_case_3_5a, sgrp_case_3_5a, name_case_3_5a, xtra_case_3_5a, desc_case_3_5a, sref_case_3_5a, &preamble_0, &test_case_3_5a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_5b, tgrp_case_3_5b, sgrp_case_3_5b, name_case_3_5b, xtra_case_3_5b, desc_case_3_5b, sref_case_3_5b, &preamble_0, &test_case_3_5b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_6a, tgrp_case_3_6a, sgrp_case_3_6a, name_case_3_6a, xtra_case_3_6a, desc_case_3_6a, sref_case_3_6a, &preamble_0, &test_case_3_6a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_6b, tgrp_case_3_6b, sgrp_case_3_6b, name_case_3_6b, xtra_case_3_6b, desc_case_3_6b, sref_case_3_6b, &preamble_0, &test_case_3_6b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_7a, tgrp_case_3_7a, sgrp_case_3_7a, name_case_3_7a, xtra_case_3_7a, desc_case_3_7a, sref_case_3_7a, &preamble_0, &test_case_3_7a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_7b, tgrp_case_3_7b, sgrp_case_3_7b, name_case_3_7b, xtra_case_3_7b, desc_case_3_7b, sref_case_3_7b, &preamble_0, &test_case_3_7b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_8a, tgrp_case_3_8a, sgrp_case_3_8a, name_case_3_8a, xtra_case_3_8a, desc_case_3_8a, sref_case_3_8a, &preamble_0, &test_case_3_8a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_3_8b, tgrp_case_3_8b, sgrp_case_3_8b, name_case_3_8b, xtra_case_3_8b, desc_case_3_8b, sref_case_3_8b, &preamble_0, &test_case_3_8b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1a, tgrp_case_4_1a, sgrp_case_4_1a, name_case_4_1a, xtra_case_4_1a, desc_case_4_1a, sref_case_4_1a, &preamble_0, &test_case_4_1a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1b, tgrp_case_4_1b, sgrp_case_4_1b, name_case_4_1b, xtra_case_4_1b, desc_case_4_1b, sref_case_4_1b, &preamble_0, &test_case_4_1b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1c, tgrp_case_4_1c, sgrp_case_4_1c, name_case_4_1c, xtra_case_4_1c, desc_case_4_1c, sref_case_4_1c, &preamble_0, &test_case_4_1c, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1d, tgrp_case_4_1d, sgrp_case_4_1d, name_case_4_1d, xtra_case_4_1d, desc_case_4_1d, sref_case_4_1d, &preamble_0, &test_case_4_1d, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1e, tgrp_case_4_1e, sgrp_case_4_1e, name_case_4_1e, xtra_case_4_1e, desc_case_4_1e, sref_case_4_1e, &preamble_0, &test_case_4_1e, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_4_1f, tgrp_case_4_1f, sgrp_case_4_1f, name_case_4_1f, xtra_case_4_1f, desc_case_4_1f, sref_case_4_1f, &preamble_0, &test_case_4_1f, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_1, tgrp_case_5_1, sgrp_case_5_1, name_case_5_1, xtra_case_5_1, desc_case_5_1, sref_case_5_1, &preamble_0, &test_case_5_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_5_2_1, tgrp_case_5_2_1, sgrp_case_5_2_1, name_case_5_2_1, xtra_case_5_2_1, desc_case_5_2_1, sref_case_5_2_1, &preamble_0, &test_case_5_2_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_2, tgrp_case_5_2_2, sgrp_case_5_2_2, name_case_5_2_2, xtra_case_5_2_2, desc_case_5_2_2, sref_case_5_2_2, &preamble_0, &test_case_5_2_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_3, tgrp_case_5_2_3, sgrp_case_5_2_3, name_case_5_2_3, xtra_case_5_2_3, desc_case_5_2_3, sref_case_5_2_3, &preamble_0, &test_case_5_2_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_4, tgrp_case_5_2_4, sgrp_case_5_2_4, name_case_5_2_4, xtra_case_5_2_4, desc_case_5_2_4, sref_case_5_2_4, &preamble_0, &test_case_5_2_4, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_5a, tgrp_case_5_2_5a, sgrp_case_5_2_5a, name_case_5_2_5a, xtra_case_5_2_5a, desc_case_5_2_5a, sref_case_5_2_5a, &preamble_0, &test_case_5_2_5a, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_5b, tgrp_case_5_2_5b, sgrp_case_5_2_5b, name_case_5_2_5b, xtra_case_5_2_5b, desc_case_5_2_5b, sref_case_5_2_5b, &preamble_0, &test_case_5_2_5b, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_6, tgrp_case_5_2_6, sgrp_case_5_2_6, name_case_5_2_6, xtra_case_5_2_6, desc_case_5_2_6, sref_case_5_2_6, &preamble_0, &test_case_5_2_6, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_7, tgrp_case_5_2_7, sgrp_case_5_2_7, name_case_5_2_7, xtra_case_5_2_7, desc_case_5_2_7, sref_case_5_2_7, &preamble_0, &test_case_5_2_7, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_8, tgrp_case_5_2_8, sgrp_case_5_2_8, name_case_5_2_8, xtra_case_5_2_8, desc_case_5_2_8, sref_case_5_2_8, &preamble_0, &test_case_5_2_8, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_9, tgrp_case_5_2_9, sgrp_case_5_2_9, name_case_5_2_9, xtra_case_5_2_9, desc_case_5_2_9, sref_case_5_2_9, &preamble_0, &test_case_5_2_9, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_10, tgrp_case_5_2_10, sgrp_case_5_2_10, name_case_5_2_10, xtra_case_5_2_10, desc_case_5_2_10, sref_case_5_2_10, &preamble_0, &test_case_5_2_10, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_2_11, tgrp_case_5_2_11, sgrp_case_5_2_11, name_case_5_2_11, xtra_case_5_2_11, desc_case_5_2_11, sref_case_5_2_11, &preamble_0, &test_case_5_2_11, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_3_1, tgrp_case_5_3_1, sgrp_case_5_3_1, name_case_5_3_1, xtra_case_5_3_1, desc_case_5_3_1, sref_case_5_3_1, &preamble_0, &test_case_5_3_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_5_3_2, tgrp_case_5_3_2, sgrp_case_5_3_2, name_case_5_3_2, xtra_case_5_3_2, desc_case_5_3_2, sref_case_5_3_2, &preamble_0, &test_case_5_3_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_1_1, tgrp_case_6_1_1, sgrp_case_6_1_1, name_case_6_1_1, xtra_case_6_1_1, desc_case_6_1_1, sref_case_6_1_1, &preamble_0, &test_case_6_1_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_1_2, tgrp_case_6_1_2, sgrp_case_6_1_2, name_case_6_1_2, xtra_case_6_1_2, desc_case_6_1_2, sref_case_6_1_2, &preamble_0, &test_case_6_1_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_1_3, tgrp_case_6_1_3, sgrp_case_6_1_3, name_case_6_1_3, xtra_case_6_1_3, desc_case_6_1_3, sref_case_6_1_3, &preamble_0, &test_case_6_1_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_1_4, tgrp_case_6_1_4, sgrp_case_6_1_4, name_case_6_1_4, xtra_case_6_1_4, desc_case_6_1_4, sref_case_6_1_4, &preamble_0, &test_case_6_1_4, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_1_5, tgrp_case_6_1_5, sgrp_case_6_1_5, name_case_6_1_5, xtra_case_6_1_5, desc_case_6_1_5, sref_case_6_1_5, &preamble_0, &test_case_6_1_5, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_2_1, tgrp_case_6_2_1, sgrp_case_6_2_1, name_case_6_2_1, xtra_case_6_2_1, desc_case_6_2_1, sref_case_6_2_1, &preamble_0, &test_case_6_2_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_2_2, tgrp_case_6_2_2, sgrp_case_6_2_2, name_case_6_2_2, xtra_case_6_2_2, desc_case_6_2_2, sref_case_6_2_2, &preamble_0, &test_case_6_2_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_2_3, tgrp_case_6_2_3, sgrp_case_6_2_3, name_case_6_2_3, xtra_case_6_2_3, desc_case_6_2_3, sref_case_6_2_3, &preamble_0, &test_case_6_2_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_2_4, tgrp_case_6_2_4, sgrp_case_6_2_4, name_case_6_2_4, xtra_case_6_2_4, desc_case_6_2_4, sref_case_6_2_4, &preamble_0, &test_case_6_2_4, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_2_5, tgrp_case_6_2_5, sgrp_case_6_2_5, name_case_6_2_5, xtra_case_6_2_5, desc_case_6_2_5, sref_case_6_2_5, &preamble_0, &test_case_6_2_5, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_3_1, tgrp_case_6_3_1, sgrp_case_6_3_1, name_case_6_3_1, xtra_case_6_3_1, desc_case_6_3_1, sref_case_6_3_1, &preamble_0, &test_case_6_3_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_6_4_1, tgrp_case_6_4_1, sgrp_case_6_4_1, name_case_6_4_1, xtra_case_6_4_1, desc_case_6_4_1, sref_case_6_4_1, &preamble_0, &test_case_6_4_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_4_2, tgrp_case_6_4_2, sgrp_case_6_4_2, name_case_6_4_2, xtra_case_6_4_2, desc_case_6_4_2, sref_case_6_4_2, &preamble_0, &test_case_6_4_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_4_3, tgrp_case_6_4_3, sgrp_case_6_4_3, name_case_6_4_3, xtra_case_6_4_3, desc_case_6_4_3, sref_case_6_4_3, &preamble_0, &test_case_6_4_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_4_3, tgrp_case_6_4_4, sgrp_case_6_4_4, name_case_6_4_4, xtra_case_6_4_4, desc_case_6_4_4, sref_case_6_4_4, &preamble_0, &test_case_6_4_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_5_1, tgrp_case_6_5_1, sgrp_case_6_5_1, name_case_6_5_1, xtra_case_6_5_1, desc_case_6_5_1, sref_case_6_5_1, &preamble_0, &test_case_6_5_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_5_2, tgrp_case_6_5_2, sgrp_case_6_5_2, name_case_6_5_2, xtra_case_6_5_2, desc_case_6_5_2, sref_case_6_5_2, &preamble_0, &test_case_6_5_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_5_3, tgrp_case_6_5_3, sgrp_case_6_5_3, name_case_6_5_3, xtra_case_6_5_3, desc_case_6_5_3, sref_case_6_5_3, &preamble_0, &test_case_6_5_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_5_4, tgrp_case_6_5_4, sgrp_case_6_5_4, name_case_6_5_4, xtra_case_6_5_4, desc_case_6_5_4, sref_case_6_5_4, &preamble_0, &test_case_6_5_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_5_4, tgrp_case_6_5_5, sgrp_case_6_5_5, name_case_6_5_5, xtra_case_6_5_5, desc_case_6_5_5, sref_case_6_5_5, &preamble_0, &test_case_6_5_5, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_6_1, tgrp_case_6_6_1, sgrp_case_6_6_1, name_case_6_6_1, xtra_case_6_6_1, desc_case_6_6_1, sref_case_6_6_1, &preamble_0, &test_case_6_6_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_6_2, tgrp_case_6_6_2, sgrp_case_6_6_2, name_case_6_6_2, xtra_case_6_6_2, desc_case_6_6_2, sref_case_6_6_2, &preamble_0, &test_case_6_6_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_6_3, tgrp_case_6_6_3, sgrp_case_6_6_3, name_case_6_6_3, xtra_case_6_6_3, desc_case_6_6_3, sref_case_6_6_3, &preamble_0, &test_case_6_6_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_6_6_4, tgrp_case_6_6_4, sgrp_case_6_6_4, name_case_6_6_4, xtra_case_6_6_4, desc_case_6_6_4, sref_case_6_6_4, &preamble_0, &test_case_6_6_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_1_1, tgrp_case_7_1_1, sgrp_case_7_1_1, name_case_7_1_1, xtra_case_7_1_1, desc_case_7_1_1, sref_case_7_1_1, &preamble_0, &test_case_7_1_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_1_2, tgrp_case_7_1_2, sgrp_case_7_1_2, name_case_7_1_2, xtra_case_7_1_2, desc_case_7_1_2, sref_case_7_1_2, &preamble_0, &test_case_7_1_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_1_3, tgrp_case_7_1_3, sgrp_case_7_1_3, name_case_7_1_3, xtra_case_7_1_3, desc_case_7_1_3, sref_case_7_1_3, &preamble_0, &test_case_7_1_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_2_1, tgrp_case_7_2_1, sgrp_case_7_2_1, name_case_7_2_1, xtra_case_7_2_1, desc_case_7_2_1, sref_case_7_2_1, &preamble_0, &test_case_7_2_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_1, tgrp_case_7_3_1, sgrp_case_7_3_1, name_case_7_3_1, xtra_case_7_3_1, desc_case_7_3_1, sref_case_7_3_1, &preamble_0, &test_case_7_3_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_2, tgrp_case_7_3_2, sgrp_case_7_3_2, name_case_7_3_2, xtra_case_7_3_2, desc_case_7_3_2, sref_case_7_3_2, &preamble_0, &test_case_7_3_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_3, tgrp_case_7_3_3, sgrp_case_7_3_3, name_case_7_3_3, xtra_case_7_3_3, desc_case_7_3_3, sref_case_7_3_3, &preamble_0, &test_case_7_3_3, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_4, tgrp_case_7_3_4, sgrp_case_7_3_4, name_case_7_3_4, xtra_case_7_3_4, desc_case_7_3_4, sref_case_7_3_4, &preamble_0, &test_case_7_3_4, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_5, tgrp_case_7_3_5, sgrp_case_7_3_5, name_case_7_3_5, xtra_case_7_3_5, desc_case_7_3_5, sref_case_7_3_5, &preamble_0, &test_case_7_3_5, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_7_3_6, tgrp_case_7_3_6, sgrp_case_7_3_6, name_case_7_3_6, xtra_case_7_3_6, desc_case_7_3_6, sref_case_7_3_6, &preamble_0, &test_case_7_3_6, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_8_1_1, tgrp_case_8_1_1, sgrp_case_8_1_1, name_case_8_1_1, xtra_case_8_1_1, desc_case_8_1_1, sref_case_8_1_1, &preamble_0, &test_case_8_1_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_8_1_2, tgrp_case_8_1_2, sgrp_case_8_1_2, name_case_8_1_2, xtra_case_8_1_2, desc_case_8_1_2, sref_case_8_1_2, &preamble_0, &test_case_8_1_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_8_2_1, tgrp_case_8_2_1, sgrp_case_8_2_1, name_case_8_2_1, xtra_case_8_2_1, desc_case_8_2_1, sref_case_8_2_1, &preamble_0, &test_case_8_2_1, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_8_2_2, tgrp_case_8_2_2, sgrp_case_8_2_2, name_case_8_2_2, xtra_case_8_2_2, desc_case_8_2_2, sref_case_8_2_2, &preamble_0, &test_case_8_2_2, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_8_2_3, tgrp_case_8_2_3, sgrp_case_8_2_3, name_case_8_2_3, xtra_case_8_2_3, desc_case_8_2_3, sref_case_8_2_3, &preamble_0, &test_case_8_2_3, &postamble_0, 0, 0, 0, __RESULT_SUCCESS}, {
	numb_case_9_1_1, tgrp_case_9_1_1, sgrp_case_9_1_1, name_case_9_1_1, xtra_case_9_1_1, desc_case_9_1_1, sref_case_9_1_1, &preamble_0, &test_case_9_1_1, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
	numb_case_9_1_2, tgrp_case_9_1_2, sgrp_case_9_1_2, name_case_9_1_2, xtra_case_9_1_2, desc_case_9_1_2, sref_case_9_1_2, &preamble_0, &test_case_9_1_2, &postamble_0, 0, 0, 0, __RESULT_INCONCLUSIVE}, {
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
	if (verbose < 0)
		return;
	print_header();
	fprintf(stdout, "\
\n\
Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Affero  General  Public  License (AGPL)  Version  3, so long as the  software is\n\
distributed with, and only used for the testing of, OpenSS7 modules, drivers and\n\
libraries.\n\
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
	fflush(stdout);
}

void
version(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock\n\
\n\
Copyright (c) 2008, 2009  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU Affero General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision: 1.1.2.1 $ $Date: 2009-06-21 11:44:18 $");
}

void
usage(int argc, char *argv[])
{
	if (verbose < 0)
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
	if (verbose < 0)
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
    -p, --client-port [PORT]\n\
        port number from which to connect [default: %3$d+index*3]\n\
    -P, --server-port [PORT]\n\
        port number to which to connect or upon which to listen\n\
        [default: %3$d+index*3+2]\n\
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
Symbols:\n\
    [STANDARD]\n\
        SS7_PVAR_ITUT_83  SS7_PVAR_ETSI_88  SS7_PVAR_ANSI_92  SS7_PVAR_SING_88\n\
        SS7_PVAR_ITUT_93  SS7_PVAR_ETSI_93  SS7_PVAR_JTTC_94  SS7_PVAR_SPAN_88\n\
        SS7_PVAR_ITUT_96  SS7_PVAR_ETSI_96  SS7_PVAR_ANSI_96\n\
        SS7_PVAR_ITUT_00  SS7_PVAR_ETSI_00  SS7_PVAR_ANSI_00  SS7_PVAR_CHIN_00\n\
\n\
", argv[0], devname, TEST_PORT_NUMBER);
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
			{"draft",	required_argument,	NULL, 'D'},
			{"decl-std",	required_argument,	NULL, 'D'},
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

		c = getopt_long(argc, argv, "uD:cSawp:P:i:I:rRd:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "uD:cSawp:P:i:I:rRd:el::f::so:t:mqvhVC?");
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
				show_msg++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			show_msg = val;
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
