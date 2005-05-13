/*****************************************************************************

 @(#) $RCSfile: test-q784.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/13 11:15:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2005/05/13 11:15:51 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-q784.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/13 11:15:51 $"

static char const ident[] =
    "$RCSfile: test-q784.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/13 11:15:51 $";

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

#define FFLUSH(stream) fflush(stream)

enum {
	TIMEOUT = -5, UNKNOWN = -4, DECODEERROR = -3, SCRIPTERROR = -2,
	INCONCLUSIVE = -1, SUCCESS = 0, FAILURE = 1,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

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
#define HZ 100

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
	(ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	 ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8,
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
	(ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	 ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8,
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

static int verbose = 0;
static int show_msg = 0;
static int show_acks = 0;

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

#undef HZ
#define HZ 1000

/* *INDENT-OFF* */
static timer_range_t timer[tmax] = {
	{(15 * HZ),		(60 * HZ)},		/* T1 15-60 seconds */
	{(3 * 60 * HZ),		(3 * 60 * HZ)},		/* T2 3 minutes */
	{(2 * 60 * HZ),		(2 * 60 * HZ)},		/* T3 2 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T4 5-15 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T5 5-15 minutes */
	{(10 * HZ),		(32 * HZ)},		/* T6 10-32 seconds (specified in Q.118) */
	{(20 * HZ),		(30 * HZ)},		/* T7 20-30 seconds */
	{(10 * HZ),		(15 * HZ)},		/* T8 10-15 seconds */
	{(2 * 60 * HZ),		(4 * 60 * HZ)},		/* T9 2-4 minutes (specified in Q.118) */
	{(4 * HZ),		(6 * HZ)},		/* T10 4-6 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T11 15-20 seconds */
	{(15 * HZ),		(60 * HZ)},		/* T12 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T13 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T14 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T15 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T16 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T17 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T18 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T19 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T20 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T21 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T22 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T23 5-15 minutes */
	{(1 * HZ),		(2 * HZ)},		/* T24 < 2 seconds */
	{(1 * HZ),		(10 * HZ)},		/* T25 1-10 seconds */
	{(1 * 60 * HZ),		(3 * 60 * HZ)},		/* T26 1-3 minutes */
	{(4 * 60 * HZ),		(4 * 60 * HZ)},		/* T27 4 minutes */
	{(10 * HZ),		(10 * HZ)},		/* T28 10 seconds */
	{(300 * HZ / 1000),	(600 * HZ / 1000)},	/* T29 300-600 milliseconds */
	{(5 * HZ),		(10 * HZ)},		/* T30 5-10 seconds */
	{(6 * 60 * HZ),		(7 * 60 * HZ)},		/* T31 > 6 minutes */
	{(3 * HZ),		(5 * HZ)},		/* T32 3-5 seconds */
	{(12 * HZ),		(15 * HZ)},		/* T33 12-15 seconds */
	{(12 * HZ),		(15 * HZ)},		/* T34 12-15 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T35 15-20 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T36 15-20 seconds */
	{(2 * HZ),		(4 * HZ)},		/* T37 2-4 seconds */
	{(15 * HZ),		(20 * HZ)}		/* T38 15-20 seconds */
};
/* *INDENT-ON* */

long test_start = 0;

static int state;

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
		FFLUSH(stdout);
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
	printf("                    .  .              :                .                    \n");
	printf("                    .  .            %6s             .                    (%d)\n", t,
	       state);
	printf("                    .  .              :                .                    \n");
	FFLUSH(stdout);
	return now();
}
static long
milliseconds_2nd(char *t)
{
	printf("                    .  .              :   :            .                    \n");
	printf("                    .  .              : %6s         .                    (%d)\n", t,
	       state);
	printf("                    .  .              :   :            .                    \n");
	FFLUSH(stdout);
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
	printf("                    |  |(%7.3g <= %7.3g <= %7.3g)| %s             (%d)\n",
	       dlo - tol, itv, dhi + tol, t, state);
	FFLUSH(stdout);
	if (dlo - tol <= itv && itv <= dhi + tol)
		return SUCCESS;
	else
		return FAILURE;
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
		printf
		    ("                    |  | %11.6g                   |                    (%d)\n",
		     t, state);
		FFLUSH(stdout);
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
static int
start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;
	return start_tt(sdur);
}

static int
stop_tt(void)
{
	struct itimerval setting = { {0, 0}, {0, 0} };
	sigset_t mask;
	struct sigaction act;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return FAILURE;
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
	int ret = SUCCESS;
	int i;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*pt_dat), 0, pt_dat };
	union MTP_primitives *m = (typeof(m)) cbuf;
	unsigned char *d = pt_dat;
	unsigned char *p = pt_dat;
	/* precharge buffer */
	*d++ = pmsg.cic;
	*d++ = pmsg.cic >> 8;
	switch (msg) {
	case IAM:
		printf
		    ("                    |<-+---------------------%3ld--IAM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_IAM;
		*d++ = pmsg.nci;
		*d++ = pmsg.fci;
		*d++ = pmsg.fci >> 8;
		*d++ = pmsg.cpc;
		*d++ = pmsg.tmr;
		p = d + 2;
		*d = p - d; d++;	/* pointer to cdpn */
		pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
		*p++ = 3 + ((pmsg.cdpn.len + 1) >> 1) + 1;	/* pointer to optional parameters */
		*p++ = 2 + ((pmsg.cdpn.len + 1) >> 1);	/* cdpn len */
		*p++ = pmsg.cdpn.nai | ((pmsg.cdpn.len & 0x1) ? 0x80 : 0);	/* nai and o/e */
		*p++ = 0x10;	/* inn allowed E.164 */
		for (i = 0; i < pmsg.cdpn.len; i += 2)
			*p++ = (pmsg.cdpn.num[i] & 0x0f) | ((pmsg.cdpn.num[i + 1] & 0x0f) << 4);
		/* optional parameters */
		*d = p - d; d++;
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
		printf
		    ("                    |<-+---------------------%3ld--SAM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_SAM;
		p = d + 2;
		*d = p - d; d++;	/* pointer to subn */
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
		printf
		    ("                    |<-+---------------------%3ld--INR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_INR;
		*d++ = pmsg.inri;
		*d++ = 0;	/* eop */
		goto send_isup;
	case INF:
		printf
		    ("                    |<-+---------------------%3ld--INF--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_INF;
		*d++ = pmsg.infi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case COT:
		*d++ = ISUP_MT_COT;
		*d++ = pmsg.coti;	/* success = 1, failure = 0 */
		if (pmsg.coti) {
			printf
			    ("                    |<-+---------------------%3ld--COT--| (success)          (%d)\n",
			     pmsg.cic, state);
			FFLUSH(stdout);
		} else {
			printf
			    ("                    |<-+---------------------%3ld--COT--| (failure)          (%d)\n",
			     pmsg.cic, state);
			FFLUSH(stdout);
		}
		goto send_isup;
	case ACM:
		printf
		    ("                    |<-+---------------------%3ld--ACM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_ACM;
		*d++ = pmsg.bci;
		*d++ = pmsg.bci >> 8;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CON:
		printf
		    ("                    |<-+---------------------%3ld--CON--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CON;
		*d++ = pmsg.bci;
		*d++ = pmsg.bci >> 8;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FOT:
		printf
		    ("                    |<-+---------------------%3ld--FOT--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FOT;
		*d++ = 0;	/* eop */
		goto send_isup;
	case ANM:
		printf
		    ("                    |<-+---------------------%3ld--ANM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_ANM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case REL:
		printf
		    ("                    |<-+---------------------%3ld--REL--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_REL;
		*d++ = 2;	/* causv pointer */
		*d++ = 0;	/* eop */
		*d++ = pmsg.caus.len;
		for (i = 0; i < pmsg.caus.len; i++)
			*d++ = pmsg.caus.buf[i];
		goto send_isup;
	case SUS:
		printf
		    ("                    |<-+---------------------%3ld--SUS--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_SUS;
		*d++ = pmsg.sris;
		*d++ = 0;	/* eop */
		goto send_isup;
	case RES:
		printf
		    ("                    |<-+---------------------%3ld--RES--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_RES;
		*d++ = pmsg.sris;
		*d++ = 0;	/* eop */
		goto send_isup;
	case RLC:
		printf
		    ("                    |<-+---------------------%3ld--RLC--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_RLC;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CCR:
		printf
		    ("                    |<-+---------------------%3ld--CCR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CCR;
		goto send_isup;
	case RSC:
		printf
		    ("                    |<-+---------------------%3ld--RSC--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_RSC;
		goto send_isup;
	case BLO:
		printf
		    ("                    |<-+---------------------%3ld--BLO--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_BLO;
		goto send_isup;
	case UBL:
		printf
		    ("                    |<-+---------------------%3ld--UBL--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_UBL;
		goto send_isup;
	case BLA:
		printf
		    ("                    |<-+---------------------%3ld--BLA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_BLA;
		goto send_isup;
	case UBA:
		printf
		    ("                    |<-+---------------------%3ld--UBA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_UBA;
		goto send_isup;
	case GRS:
		printf
		    ("                    |<-+---------------------%3ld--GRS--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_GRS;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGB:
		printf
		    ("                    |<-+---------------------%3ld--CGB--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CGB;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGU:
		printf
		    ("                    |<-+---------------------%3ld--CGU--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CGU;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGBA:
		printf
		    ("                    |<-+---------------------%3ld--CGBA-|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CGBA;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CGUA:
		printf
		    ("                    |<-+---------------------%3ld--CGUA-|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CGUA;
		*d++ = pmsg.cgi;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CMR:
		printf
		    ("                    |<-+---------------------%3ld--CMR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CMR;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CMC:
		printf
		    ("                    |<-+---------------------%3ld--CMC--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CMC;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CMRJ:
		printf
		    ("                    |<-+---------------------%3ld--CMRJ-|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CMRJ;
		*d++ = pmsg.cmi;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAR:
		printf
		    ("                    |<-+---------------------%3ld--FAR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FAR;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAA:
		printf
		    ("                    |<-+---------------------%3ld--FAA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FAA;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FRJ:
		printf
		    ("                    |<-+---------------------%3ld--FRJ--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FRJ;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAD:
		printf
		    ("                    |<-+---------------------%3ld--FAD--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FAD;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAI:
		printf
		    ("                    |<-+---------------------%3ld--FAI--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FAI;
		*d++ = pmsg.faci;
		*d++ = 0;	/* eop */
		goto send_isup;
	case LPA:
		printf
		    ("                    |<-+---------------------%3ld--LPA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_LPA;
		goto send_isup;
	case DRS:
		printf
		    ("                    |<-+---------------------%3ld--DRS--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_DRS;
		*d++ = 0;	/* eop */
		goto send_isup;
	case PAM:
		printf
		    ("                    |<-+---------------------%3ld--PAM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_PAM;
		for (i = 0; i < pmsg.pam.len; i++)
			*d++ = pmsg.pam.buf[i];
		goto send_isup;
	case GRA:
		printf
		    ("                    |<-+---------------------%3ld--GRA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_GRA;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CQM:
		printf
		    ("                    |<-+---------------------%3ld--CQM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CQM;
		*d++ = 1;
		*d++ = pmsg.rs.len;
		for (i = 0; i < pmsg.rs.len; i++)
			*d++ = pmsg.rs.buf[i];
		goto send_isup;
	case CQR:
		printf
		    ("                    |<-+---------------------%3ld--CQR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
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
		printf
		    ("                    |<-+---------------------%3ld--CPG--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CPG;
		*d++ = pmsg.evnt;
		*d++ = 0;	/* eop */
		goto send_isup;
	case USR:
		printf
		    ("                    |<-+---------------------%3ld--USR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_USR;
		*d++ = 2;
		*d++ = 0;	/* eop */
		*d++ = pmsg.uui.len;
		for (i = 0; i < pmsg.uui.len; i++)
			*d++ = pmsg.uui.buf[i];
		goto send_isup;
	case UCIC:
		printf
		    ("                    |<-+---------------------%3ld--UCIC-|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_UCIC;
		goto send_isup;
	case CFN:
		printf
		    ("                    |<-+---------------------%3ld--CFN--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CFN;
		*d++ = 2;
		*d++ = 0;	/* eop */
		*d++ = pmsg.caus.len;
		for (i = 0; i < pmsg.caus.len; i++)
			*d++ = pmsg.caus.buf[i];
		goto send_isup;
	case OLM:
		printf
		    ("                    |<-+---------------------%3ld--OLM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_OLM;
		goto send_isup;
	case CRG:
		printf
		    ("                    |<-+---------------------%3ld--CRG--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CRG;
		goto send_isup;
	case NRM:
		printf
		    ("                    |<-+---------------------%3ld--NRM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_NRM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case FAC:
		printf
		    ("                    |<-+---------------------%3ld--FAC--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_FAC;
		*d++ = 0;	/* eop */
		goto send_isup;
	case UPT:
		printf
		    ("                    |<-+---------------------%3ld--UPT--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_UPT;
		*d++ = 0;	/* eop */
		goto send_isup;
	case UPA:
		printf
		    ("                    |<-+---------------------%3ld--UPA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_UPA;
		*d++ = 0;	/* eop */
		goto send_isup;
	case IDR:
		printf
		    ("                    |<-+---------------------%3ld--IDR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_IDR;
		*d++ = 0;	/* eop */
		goto send_isup;
	case IRS:
		printf
		    ("                    |<-+---------------------%3ld--IRS--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_IRS;
		*d++ = 0;	/* eop */
		goto send_isup;
	case SGM:
		printf
		    ("                    |<-+---------------------%3ld--SGM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_SGM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CRA:
		printf
		    ("                    |<-+---------------------%3ld--CRA--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CRA;
		goto send_isup;
	case CRM:
		printf
		    ("                    |<-+---------------------%3ld--CRM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CRM;
		*d++ = pmsg.nci;
		goto send_isup;
	case CVR:
		printf
		    ("                    |<-+---------------------%3ld--CVR--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CVR;
		*d++ = pmsg.cvri;
		*d++ = pmsg.cgri;
		*d++ = 0;	/* eop */
		goto send_isup;
	case CVT:
		printf
		    ("                    |<-+---------------------%3ld--CVT--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CVT;
		goto send_isup;
	case EXM:
		printf
		    ("                    |<-+---------------------%3ld--EXM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_EXM;
		*d++ = 0;	/* eop */
		goto send_isup;
	case NON:
		printf
		    ("                    |<-+---------------------%3ld--NON--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_NON;
		*d++ = pmsg.ton;
		*d++ = 0;	/* eop */
		goto send_isup;
	case LLM:
		printf
		    ("                    |<-+---------------------%3ld--LLM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_LLM;
		goto send_isup;
	case CAK:
		printf
		    ("                    |<-+---------------------%3ld--CAK--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_CAK;
		goto send_isup;
	case TCM:
		printf
		    ("                    |<-+---------------------%3ld--TCM--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_TCM;
		*d++ = pmsg.cri;
		goto send_isup;
	case MCP:
		printf
		    ("                    |<-+---------------------%3ld--MCP--|                    (%d)\n",
		     pmsg.cic, state);
		FFLUSH(stdout);
		*d++ = ISUP_MT_MCP;
		goto send_isup;
	case PAUSE:
		printf
		    ("                    |<-+--------------------------TFP--|                    (%d)\n",
		     state);
		FFLUSH(stdout);
		m->pause_ind.mtp_primitive = MTP_PAUSE_IND;
		m->pause_ind.mtp_addr_offset = sizeof(m->pause_ind);
		m->pause_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->pause_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->pause_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case RESUME:
		printf
		    ("                    |<-+--------------------------TFA--|                    (%d)\n",
		     state);
		FFLUSH(stdout);
		m->resume_ind.mtp_primitive = MTP_RESUME_IND;
		m->resume_ind.mtp_addr_offset = sizeof(m->resume_ind);
		m->resume_ind.mtp_addr_length = sizeof(rem_addr);
		*(mtp_addr_t *) (&m->resume_ind + 1) = rem_addr;
		ctrl.len = sizeof(m->resume_ind) + sizeof(mtp_addr_t);
		data.len = 0;
		goto send_mtp;
	case RESTART_COMPLETE:
		printf
		    ("                    |<-+--------------------------TRA--|                    (%d)\n",
		     state);
		FFLUSH(stdout);
		m->pause_ind.mtp_primitive = MTP_RESTART_COMPLETE_IND;
		ctrl.len = sizeof(m->restart_complete_ind);
		data.len = 0;
		goto send_mtp;
	case USER_PART_UNKNOWN:
		printf
		    ("                    |<-+--------------------------UPU--| (unknown)          (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("                    |<-+--------------------------UPU--| (unequipped)       (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("                    |<-+--------------------------UPU--| (unavailable)      (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("                    |<-+--------------------------TFC--|                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("<<<<<<<<<<<<<<<<<<<<|<<|<<< IN-BAND INFORMATION <<<<<<<|<<<<<<<<<<<<<<<<<<<<(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case RINGING:
		printf
		    ("<<<<<<<<<<<<<<<<<<<<|<<|<<<<<<<<< RINGING <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<<<(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case COMMUNICATION:
		printf
		    ("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case TONE:
		printf
		    ("^<^<^<^<^<^<^<^<^<^<|<^|^<^<^< CONTINUITY TONE <^<^<^<^|^<^<^<^<^<^<^<^<^<^<(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case LOOPBACK:
		printf
		    ("                    |__|_______________________________|_                   \n");
		printf
		    ("                    |<<|_______________________________|_| LOOPBACK         (%d)\n",
		     state);
		printf
		    ("                    |  |                               |                    \n");
		FFLUSH(stdout);
		return (SUCCESS);
	default:
		printf
		    ("                    |<-+--------------------------\?\?\?--|                    (%d)\n",
		     state);
		FFLUSH(stdout);
		return (FAILURE);
	}
      send_isup:
	data.len = (d > p) ? (d - pt_dat) : (p - pt_dat);
	if (show_msg) {
		printf("Sent msg: ");
		for (i = 0; i < data.len; i++)
			printf("%02X ", (unsigned char) data.buf[i]);
		printf("\n");
		FFLUSH(stdout);
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
	data.buf = pt_dat;
	if (putmsg(pt_fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, 0) < 0) {
		if (errno == EAGAIN || errno == EINTR)
			return FAILURE;
		printf
		    ("                    |<-+--------------------******** ERROR: putmsg failed                                         \n");
		printf("                                                          : %-13s:%40s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
			printf
			    ("---INFO------------>|  |                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
		}
		m->cc_primitive = CC_INFO_REQ;
		ctrl.len = sizeof(m->info_req);
		data.len = 0;
		goto signal_msg;
	case OPTMGMT_REQ:
		if (verbose) {
			printf
			    ("---OPTMGMT--------->|  |                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
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
			printf
			    ("---BIND------------>|  |                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			if (verbose > 2) {
				if (cprim.addr.len) {
					printf
					    (" addr scope =    %2s |  |                               |                    \n",
					     scope_string(cprim.addr.add.scope));
					printf
					    (" addr id    = %5ld |  |                               |                    \n",
					     cprim.addr.add.id);
					printf
					    (" addr cic   = %5ld |  |                               |                    \n",
					     cprim.addr.add.cic);
				}
				printf
				    (" setup inds = %5ld |  |                               |                    \n",
				     cprim.setup_ind);
				printf
				    (" flags      =  %4lx |  |                               |                    \n",
				     cprim.bind_flags);
				FFLUSH(stdout);
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
			printf
			    ("---UNBIND---------->|  |                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_UNBIND_REQ;
		ctrl.len = sizeof(m->unbind_req);
		data.len = 0;
		goto signal_msg;
	case ADDR_REQ:
		if (verbose) {
			printf
			    ("---ADDR------------>|  |                               |                    (%d)\n",
			     state);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		FFLUSH(stdout);
		m->cc_primitive = CC_ADDR_REQ;
		m->addr_req.cc_call_ref = cprim.call_ref;
		ctrl.len = sizeof(m->addr_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_REQ:
		printf
		    ("---SETUP----------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_REQ;
		m->setup_req.cc_user_ref = cprim.user_ref;
		m->setup_req.cc_call_type = cprim.call_type;
		m->setup_req.cc_call_flags = cprim.call_flags;
		m->setup_req.cc_cdpn_length = cprim.cdpn.len;
		m->setup_req.cc_cdpn_offset = cprim.cdpn.len ? sizeof(m->setup_req) : 0;
		m->setup_req.cc_opt_length = cprim.opt.len;
		m->setup_req.cc_opt_offset =
		    cprim.opt.len ? sizeof(m->setup_req) + cprim.cdpn.len : 0;
		m->setup_req.cc_addr_length = cprim.addr.len;
		m->setup_req.cc_addr_offset =
		    cprim.addr.len ? sizeof(m->setup_req) + cprim.cdpn.len + cprim.opt.len : 0;
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
		printf
		    ("---MORE-INFO------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---INFORMATION----->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_INFORMATION_REQ;
		m->information_req.cc_user_ref = cprim.user_ref;
		m->information_req.cc_subn_length = cprim.subn.len;
		m->information_req.cc_subn_offset = cprim.subn.len ? sizeof(m->information_req) : 0;
		m->information_req.cc_opt_length = cprim.opt.len;
		m->information_req.cc_opt_offset =
		    cprim.opt.len ? sizeof(m->information_req) + cprim.subn.len : 0;
		c = (unsigned char *) (&m->information_req + 1);
		bcopy(cprim.subn.buf, c, cprim.subn.len);
		c += cprim.subn.len;
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->information_req) + cprim.subn.len + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONT_CHECK_REQ:
		printf
		    ("---CONT CHECK------>|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_CHECK_REQ;
		m->cont_check_req.cc_addr_length = cprim.addr.len;
		m->cont_check_req.cc_addr_offset = sizeof(m->cont_check_req);
		c = (unsigned char *) (&m->cont_check_req + 1);
		bcopy(&cprim.addr.add, c, cprim.addr.len);
		ctrl.len = sizeof(m->cont_check_req) + cprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case CONT_TEST_REQ:
		printf
		    ("---CONT-TEST------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_TEST_REQ;
		m->cont_test_req.cc_token_value = cprim.token_value;
		m->cont_test_req.cc_call_ref = cprim.call_ref;
		ctrl.len = sizeof(m->cont_test_req);
		data.len = 0;
		goto signal_msg;
	case CONT_REPORT_REQ:
		printf
		    ("---CONT REPORT----->|  |                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_REPORT_REQ;
		m->cont_report_req.cc_user_ref = cprim.user_ref;
		m->cont_report_req.cc_call_ref = cprim.call_ref;
		m->cont_report_req.cc_result = cprim.result;
		ctrl.len = sizeof(m->cont_report_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_RES:
		printf
		    ("---SETUP OK-------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_RES;
		m->setup_res.cc_call_ref = cprim.call_ref;
		m->setup_res.cc_token_value = cprim.token_value;
		ctrl.len = sizeof(m->setup_res);
		data.len = 0;
		goto signal_msg;
	case PROCEEDING_REQ:
		printf
		    ("---PROCEEDING------>|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---ALERTING-------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---PROGRESS-------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---IBI------------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---CONNECT--------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SETUP-COMPLETE-->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_COMPLETE_REQ;
		m->setup_complete_req.cc_call_ref = cprim.call_ref;
		m->setup_complete_req.cc_opt_length = cprim.opt.len;
		m->setup_complete_req.cc_opt_offset =
		    cprim.opt.len ? sizeof(m->setup_complete_req) : 0;
		c = (unsigned char *) (&m->setup_complete_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->setup_complete_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case FORWXFER_REQ:
		printf
		    ("---FORWXFER-------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND--------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND-OK------>|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND-REJECT-->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SUSPEND_REJECT_REQ;
		m->suspend_reject_req.cc_call_ref = cprim.call_ref;
		m->suspend_reject_req.cc_cause = cprim.cause;
		m->suspend_reject_req.cc_opt_length = cprim.opt.len;
		m->suspend_reject_req.cc_opt_offset =
		    cprim.opt.len ? sizeof(m->suspend_reject_req) : 0;
		c = (unsigned char *) (&m->suspend_reject_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->suspend_reject_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REQ:
		printf
		    ("---RESUME---------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RESUME-OK------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RESUME-REJECT--->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_RESUME_REJECT_REQ;
		m->resume_reject_req.cc_call_ref = cprim.call_ref;
		m->resume_reject_req.cc_cause = cprim.cause;
		m->resume_reject_req.cc_opt_length = cprim.opt.len;
		m->resume_reject_req.cc_opt_offset =
		    cprim.opt.len ? sizeof(m->resume_reject_req) : 0;
		c = (unsigned char *) (&m->resume_reject_req + 1);
		bcopy(cprim.opt.buf, c, cprim.opt.len);
		c += cprim.opt.len;
		ctrl.len = sizeof(m->resume_reject_req) + cprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case REJECT_REQ:
		printf
		    ("---REJECT---------->|  |                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RELEASE--------->|  |                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		FFLUSH(stdout);
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
		printf
		    ("---RELEASE OK------>|  |                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		FFLUSH(stdout);
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
		printf
		    ("---RESET----------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---RESET OK-------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---BLOCKING-------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---BLOCKING-OK----->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---UNBLOCKING------>|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---UNBLOCKING-OK--->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---QUERY----------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---QUERY OK-------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    (">>>>>>>>>>>>>>>>>>>>|>>|>>> IN-BAND INFORMATION >>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case RINGING:
		printf
		    (">>>>>>>>>>>>>>>>>>>>|>>|>>>>>>>>> RINGING >>>>>>>>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case COMMUNICATION:
		printf
		    ("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case TONE:
		printf
		    ("^>^>^>^>^>^>^>^>^>^>|>^|^>^>^> CONTINUITY TONE >^>^>^>^|^>^>^>^>^>^>^>^>^>^>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case LOOPBACK:
		printf
		    ("                   _|__|_______________________________|                    \n");
		printf
		    ("         LOOPBACK |_|__|_____________________________>>|                    (%d)\n",
		     state);
		printf
		    ("                    |  |                               |                    \n");
		FFLUSH(stdout);
		return (SUCCESS);
	default:
		printf
		    ("\?\?\?\?--------------->|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		return FAILURE;
	}
      signal_msg:
	if ((ret =
	     putmsg(iut_cpc_fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, RS_HIPRI)) < 0) {
		printf
		    ("************** ERROR: putmsg failed                                         \n");
		printf("                    : %-13s:%40s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return (FAILURE);
	}
	return time_event(SUCCESS);
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
			printf
			    ("---INFO-------------+->|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_INFO_REQ;
		ctrl.len = sizeof(m->info_req);
		data.len = 0;
		goto signal_msg;
	case OPTMGMT_REQ:
		if (verbose)
			printf
			    ("---OPTMGMT----------+->|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
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
			printf
			    ("---BIND-------------+->|                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			if (verbose > 2) {
				if (mprim.addr.len) {
					printf
					    (" addr scope =    %2s |  |                               |                    \n",
					     scope_string(mprim.addr.add.scope));
					printf
					    (" addr id    = %5ld |  |                               |                    \n",
					     mprim.addr.add.id);
					printf
					    (" addr cic   = %5ld |  |                               |                    \n",
					     mprim.addr.add.cic);
				}
				printf
				    (" setup inds = %5ld |  |                               |                    \n",
				     mprim.setup_ind);
				printf
				    (" flags      =  %4lx |  |                               |                    \n",
				     mprim.bind_flags);
				FFLUSH(stdout);
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
			printf
			    ("---UNBIND-----------+->|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_UNBIND_REQ;
		ctrl.len = sizeof(m->unbind_req);
		data.len = 0;
		goto signal_msg;
	case ADDR_REQ:
		if (verbose)
			printf
			    ("---ADDR-------------+->|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_ADDR_REQ;
		ctrl.len = sizeof(m->addr_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_REQ:
		printf
		    ("---SETUP------------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_REQ;
		m->setup_req.cc_user_ref = mprim.user_ref;
		m->setup_req.cc_call_type = mprim.call_type;
		m->setup_req.cc_call_flags = mprim.call_flags;
		m->setup_req.cc_cdpn_length = mprim.cdpn.len;
		m->setup_req.cc_cdpn_offset = mprim.cdpn.len ? sizeof(m->setup_req) : 0;
		m->setup_req.cc_opt_length = mprim.opt.len;
		m->setup_req.cc_opt_offset =
		    mprim.opt.len ? sizeof(m->setup_req) + mprim.cdpn.len : 0;
		m->setup_req.cc_addr_length = mprim.addr.len;
		m->setup_req.cc_addr_offset =
		    mprim.addr.len ? sizeof(m->setup_req) + mprim.cdpn.len + mprim.opt.len : 0;
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
		printf
		    ("---MORE-INFO--------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---INFORMATION------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_INFORMATION_REQ;
		m->information_req.cc_user_ref = mprim.user_ref;
		m->information_req.cc_subn_length = mprim.subn.len;
		m->information_req.cc_subn_offset = mprim.subn.len ? sizeof(m->information_req) : 0;
		m->information_req.cc_opt_length = mprim.opt.len;
		m->information_req.cc_opt_offset =
		    mprim.opt.len ? sizeof(m->information_req) + mprim.subn.len : 0;
		c = (unsigned char *) (&m->information_req + 1);
		bcopy(mprim.subn.buf, c, mprim.subn.len);
		c += mprim.subn.len;
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->information_req) + mprim.subn.len + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case CONT_CHECK_REQ:
		printf
		    ("---CONT CHECK-------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_CHECK_REQ;
		m->cont_check_req.cc_addr_length = mprim.addr.len;
		m->cont_check_req.cc_addr_offset = sizeof(m->cont_check_req);
		c = (unsigned char *) (&m->cont_check_req + 1);
		bcopy(&mprim.addr.add, c, mprim.addr.len);
		ctrl.len = sizeof(m->cont_check_req) + mprim.addr.len;
		data.len = 0;
		goto signal_msg;
	case CONT_TEST_REQ:
		printf
		    ("---CONT-TEST--------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_TEST_REQ;
		m->cont_test_req.cc_call_ref = mprim.call_ref;
		m->cont_test_req.cc_token_value = mprim.token_value;
		ctrl.len = sizeof(m->cont_test_req);
		data.len = 0;
		goto signal_msg;
	case CONT_REPORT_REQ:
		printf
		    ("---CONT REPORT------+->|                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		FFLUSH(stdout);
		m->cc_primitive = CC_CONT_REPORT_REQ;
		m->cont_report_req.cc_user_ref = mprim.user_ref;
		m->cont_report_req.cc_call_ref = mprim.call_ref;
		m->cont_report_req.cc_result = mprim.result;
		ctrl.len = sizeof(m->cont_report_req);
		data.len = 0;
		goto signal_msg;
	case SETUP_RES:
		printf
		    ("---SETUP OK---------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_RES;
		m->setup_res.cc_call_ref = mprim.call_ref;
		m->setup_res.cc_token_value = mprim.token_value;
		ctrl.len = sizeof(m->setup_res);
		data.len = 0;
		goto signal_msg;
	case PROCEEDING_REQ:
		printf
		    ("---PROCEEDING-------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---ALERTING---------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---PROGRESS---------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---IBI--------------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---CONNECT----------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SETUP-COMPLETE---+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SETUP_COMPLETE_REQ;
		m->setup_complete_req.cc_call_ref = mprim.call_ref;
		m->setup_complete_req.cc_opt_length = mprim.opt.len;
		m->setup_complete_req.cc_opt_offset =
		    mprim.opt.len ? sizeof(m->setup_complete_req) : 0;
		c = (unsigned char *) (&m->setup_complete_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->setup_complete_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case FORWXFER_REQ:
		printf
		    ("---FORWXFER---------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND----------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND-OK-------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---SUSPEND-REJECT---+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_SUSPEND_REJECT_REQ;
		m->suspend_reject_req.cc_call_ref = mprim.call_ref;
		m->suspend_reject_req.cc_cause = mprim.cause;
		m->suspend_reject_req.cc_opt_length = mprim.opt.len;
		m->suspend_reject_req.cc_opt_offset =
		    mprim.opt.len ? sizeof(m->suspend_reject_req) : 0;
		c = (unsigned char *) (&m->suspend_reject_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->suspend_reject_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RESUME_REQ:
		printf
		    ("---RESUME-----------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RESUME-OK--------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RESUME-REJECT----+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
		m->cc_primitive = CC_RESUME_REJECT_REQ;
		m->resume_reject_req.cc_call_ref = mprim.call_ref;
		m->resume_reject_req.cc_cause = mprim.cause;
		m->resume_reject_req.cc_opt_length = mprim.opt.len;
		m->resume_reject_req.cc_opt_offset =
		    mprim.opt.len ? sizeof(m->resume_reject_req) : 0;
		c = (unsigned char *) (&m->resume_reject_req + 1);
		bcopy(mprim.opt.buf, c, mprim.opt.len);
		c += mprim.opt.len;
		ctrl.len = sizeof(m->resume_reject_req) + mprim.opt.len;
		data.len = 0;
		goto signal_msg;
	case RELEASE_REQ:
		printf
		    ("---RELEASE----------+->|                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		FFLUSH(stdout);
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
		printf
		    ("---REJECT-----------+->|                               |                    (%d)\n",
		     state);
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		FFLUSH(stdout);
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
		printf
		    ("---RELEASE OK-------+->|                               |                    (%d)\n",
		     state);
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		FFLUSH(stdout);
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
		printf
		    ("---RESET------------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---RESET OK---------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---BLOCKING---------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---BLOCKING-OK------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---UNBLOCKING-------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---UNBLOCKING-OK----+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---QUERY------------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    ("---QUERY OK---------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
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
		printf
		    (">>>>>>>>>>>>>>>>>>>>|>>|>>> IN-BAND INFORMATION >>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case RINGING:
		printf
		    (">>>>>>>>>>>>>>>>>>>>|>>|>>>>>>>>> RINGING >>>>>>>>>>>>>|>>>>>>>>>>>>>>>>>>>>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case COMMUNICATION:
		printf
		    ("<><><><><><><><><><>|><|><><><> COMMUNICATION ><><><><>|<><><><><><><><><><>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case TONE:
		printf
		    ("^>^>^>^>^>^>^>^>^>^>|>^|^>^>^> CONTINUITY TONE >^>^>^>^|^>^>^>^>^>^>^>^>^>^>(%d)\n",
		     state);
		FFLUSH(stdout);
		return (SUCCESS);
	case LOOPBACK:
		printf
		    ("                   _|__|_______________________________|                    \n");
		printf
		    ("         LOOPBACK |_|__|_____________________________>>|                    (%d)\n",
		     state);
		printf
		    ("                    |  |                               |                    \n");
		FFLUSH(stdout);
		return (SUCCESS);
	default:
		printf
		    ("\?\?\?\?----------------+->|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		return FAILURE;
	}
      signal_msg:
	if ((ret = putmsg(fd, ctrl.len ? &ctrl : NULL, data.len ? &data : NULL, RS_HIPRI)) < 0) {
		printf
		    ("***************** ERROR: putmsg failed                                         \n");
		printf("                       : %-13s:%40s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return (FAILURE);
	}
	return time_event(SUCCESS);
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
	unsigned char *d = data.buf, *p = data.buf, *e = data.buf + data.len;
	x = *d++;
	y = *d++;
	imsg.cic = x | (y << 8);
	mt = *d++;
	switch (mt) {
	case ISUP_MT_IAM:
		printf
		    ("                    |--+-IAM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
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
		p = d + *d; d++;
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
		printf
		    ("                    |--+-SAM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (SAM);
		p = d + *d; d++;
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
		printf
		    ("                    |--+-INR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (INR);
		imsg.inri = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_INF:
		printf
		    ("                    |--+-INF--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (INF);
		imsg.infi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_COT:
		ret = (COT);
		imsg.coti = *d++;
		if (imsg.coti) {
			printf
			    ("          (success) |--+-COT--%3ld--------------------->|                    (%d)\n",
			     imsg.cic, state);
			FFLUSH(stdout);
		} else {
			printf
			    ("          (failure) |--+-COT--%3ld--------------------->|                    (%d)\n",
			     imsg.cic, state);
			FFLUSH(stdout);
		}
		goto decode_none;
	case ISUP_MT_ACM:
		printf
		    ("                    |--+-ACM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (ACM);
	      decode_bci:
		imsg.bci = ((ulong) (*d++));
		imsg.bci |= (((ulong) (*d++)) << 8);
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CON:
		printf
		    ("                    |--+-CON--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CON);
		goto decode_bci;
	case ISUP_MT_FOT:
		printf
		    ("                    |--+-FOT--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FOT);
		goto decode_opt;
	case ISUP_MT_ANM:
		printf
		    ("                    |--+-ANM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (ANM);
		goto decode_opt;
	case ISUP_MT_REL:
		printf
		    ("                    |--+-REL--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (REL);
	      decode_caus:
		p = d + *d; d++;
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
		printf
		    ("                    |--+-SUS--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (SUS);
		imsg.sris = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_RES:
		printf
		    ("                    |--+-RES--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (RES);
		imsg.sris = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_RLC:
		printf
		    ("                    |--+-RLC--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (RLC);
		goto decode_opt;
	case ISUP_MT_CCR:
		printf
		    ("                    |--+-CCR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CCR);
		goto decode_none;
	case ISUP_MT_RSC:
		printf
		    ("                    |--+-RSC--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (RSC);
		goto decode_none;
	case ISUP_MT_BLO:
		printf
		    ("                    |--+-BLO--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (BLO);
		goto decode_none;
	case ISUP_MT_UBL:
		printf
		    ("                    |--+-UBL--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UBL);
		goto decode_none;
	case ISUP_MT_BLA:
		printf
		    ("                    |--+-BLA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (BLA);
		goto decode_none;
	case ISUP_MT_UBA:
		printf
		    ("                    |--+-UBA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UBA);
		goto decode_none;
	case ISUP_MT_GRS:
		printf
		    ("                    |--+-GRS--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (GRS);
	      decode_rs:
		p = d + *d; d++;
		if (p > e) {
			printf("**** ERROR: pointer out of range p = %p, e = %p\n", p, e);
			FFLUSH(stdout);
			goto decode_error;
		}
		imsg.rs.len = *p++;
		if (p > e) {
			printf("**** ERROR: length out of range p = %p, e = %p\n", p, e);
			FFLUSH(stdout);
			goto decode_error;
		}
		if (imsg.rs.len > 33) {
			printf("**** ERROR: length too large len = %d\n", imsg.rs.len);
			FFLUSH(stdout);
			goto decode_error;
		}
		for (i = 0; i < imsg.rs.len; i++) {
			imsg.rs.buf[i] = *p++;
			if (p > e) {
				printf("**** ERROR: buffer out of range p = %p, e = %p\n", p, e);
				FFLUSH(stdout);
				goto decode_error;
			}
		}
		goto done;
	case ISUP_MT_CGB:
		printf
		    ("                    |--+-CGB--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CGB);
	      decode_cgi:
		imsg.cgi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_rs;
	case ISUP_MT_CGU:
		printf
		    ("                    |--+-CGU--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CGU);
		goto decode_cgi;
	case ISUP_MT_CGBA:
		printf
		    ("                    |--+-CGBA-%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CGBA);
		goto decode_cgi;
	case ISUP_MT_CGUA:
		printf
		    ("                    |--+-CGUA-%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CGUA);
		goto decode_cgi;
	case ISUP_MT_CMR:
		printf
		    ("                    |--+-CMR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CMR);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CMC:
		printf
		    ("                    |--+-CMC--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CMC);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CMRJ:
		printf
		    ("                    |--+-CMRJ-%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CMRJ);
		imsg.cmi = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_FAR:
		printf
		    ("                    |--+-FAR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FAR);
	      decode_faci:
		imsg.faci = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_FAA:
		printf
		    ("                    |--+-FAA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FAA);
		goto decode_faci;
	case ISUP_MT_FRJ:
		printf
		    ("                    |--+-FRJ--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FRJ);
		goto decode_faci;
	case ISUP_MT_FAD:
		printf
		    ("                    |--+-FAD--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FAD);
		goto decode_faci;
	case ISUP_MT_FAI:
		printf
		    ("                    |--+-FAI--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FAI);
		goto decode_faci;
	case ISUP_MT_LPA:
		printf
		    ("                    |--+-LPA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (LPA);
		goto decode_none;
	case ISUP_MT_DRS:
		printf
		    ("                    |--+-DRS--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (DRS);
		goto decode_opt;
	case ISUP_MT_PAM:
		printf
		    ("                    |--+-PAM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (PAM);
		pmsg.pam.len = data.len - (data.buf - (char *) d);
		bcopy(d, pmsg.pam.buf, pmsg.pam.len);
		d += pmsg.pam.len;
		break;
	case ISUP_MT_GRA:
		printf
		    ("                    |--+-GRA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (GRA);
		goto decode_rs;
	case ISUP_MT_CQM:
		printf
		    ("                    |--+-CQM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CQM);
		goto decode_rs;
	case ISUP_MT_CQR:
		printf
		    ("                    |--+-CQR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CQR);
		p = d + *d; d++;
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
		p = d + *d; d++;
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
		printf
		    ("                    |--+-CPG--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CPG);
		imsg.evnt = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_USR:
		printf
		    ("                    |--+-USR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (USR);
		p = d + *d; d++;
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
		printf
		    ("                    |--+-UCIC-%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UCIC);
		goto decode_none;
	case ISUP_MT_CFN:
		printf
		    ("                    |--+-CFN--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CFN);
		goto decode_caus;
	case ISUP_MT_OLM:
		printf
		    ("                    |--+-OLM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (OLM);
		goto decode_none;
	case ISUP_MT_CRG:
		printf
		    ("                    |--+-CRG--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CRG);
		goto decode_none;
	case ISUP_MT_NRM:
		printf
		    ("                    |--+-NRM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (NRM);
		goto decode_opt;
	case ISUP_MT_FAC:
		printf
		    ("                    |--+-FAC--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (FAC);
		goto decode_opt;
	case ISUP_MT_UPT:
		printf
		    ("                    |--+-UPT--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UPT);
		goto decode_opt;
	case ISUP_MT_UPA:
		printf
		    ("                    |--+-UPA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UPA);
		goto decode_opt;
	case ISUP_MT_IDR:
		printf
		    ("                    |--+-IDR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (IDR);
		goto decode_opt;
	case ISUP_MT_IRS:
		printf
		    ("                    |--+-IRS--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (IRS);
		goto decode_opt;
	case ISUP_MT_SGM:
		printf
		    ("                    |--+-SGM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (SGM);
		goto decode_opt;
	case ISUP_MT_CRA:
		printf
		    ("                    |--+-CRA--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CRA);
		goto decode_none;
	case ISUP_MT_CRM:
		printf
		    ("                    |--+-CRM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CRM);
		imsg.nci = *d++;
		goto decode_none;
	case ISUP_MT_CVR:
		printf
		    ("                    |--+-CVR--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CVR);
		imsg.cvri = *d++;
		if (d > e)
			goto decode_error;
		imsg.cgri = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_CVT:
		printf
		    ("                    |--+-CVT--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CVT);
		goto decode_none;
	case ISUP_MT_EXM:
		printf
		    ("                    |--+-EXM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (EXM);
		goto decode_opt;
	case ISUP_MT_NON:
		printf
		    ("                    |--+-NON--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (NON);
		imsg.ton = *d++;
		if (d > e)
			goto decode_error;
		goto decode_opt;
	case ISUP_MT_LLM:
		printf
		    ("                    |--+-LLM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (LLM);
		goto decode_none;
	case ISUP_MT_CAK:
		printf
		    ("                    |--+-CAK--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (CAK);
		goto decode_none;
	case ISUP_MT_TCM:
		printf
		    ("                    |--+-TCM--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (TCM);
		imsg.cri = *d++;
		goto decode_none;
	case ISUP_MT_MCP:
		printf
		    ("                    |--+-MCP--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (MCP);
		goto decode_none;
	default:
		printf
		    ("                    |--+-\?\?\?--%3ld--------------------->|                    (%d)\n",
		     imsg.cic, state);
		FFLUSH(stdout);
		ret = (UNKNOWN);
		break;
	}
      done:
	if (show_msg) {
		printf("Recv msg: ");
		for (i = 0; i < data.len; i++)
			printf("%02X ", (unsigned char) data.buf[i]);
		printf("\n");
		FFLUSH(stdout);
	}
	return (ret);
      decode_none:
	if (d != e)
		goto decode_error;
	goto done;
      decode_opt:
	p = d + *d; d++;
	if (p > e)
		goto decode_error;
	if (*p++) ;		/* have optional parameters */
	goto done;
      decode_error:
	printf("                    |XX+XXXXXXX DECODE ERROR XXXXXXXXXX|                    (%d)\n",
	       state);
	FFLUSH(stdout);
	ret = DECODEERROR;
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
	return (UNKNOWN);
}

static int
iut_cpc_decode_data(struct strbuf data)
{
	return (UNKNOWN);
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
			printf
			    ("<--OK---------------|  |                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
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
		FFLUSH(stdout);
		ret = CPC_ERROR_ACK;
		break;
	case CC_INFO_ACK:
		if (verbose)
			printf
			    ("<--INFO-------------|  |                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = CPC_INFO_ACK;
		break;
	case CC_BIND_ACK:
		if (verbose > 1) {
			printf
			    ("<--BIND-------------|  |                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
		}
		cprim.addr.len = m->bind_ack.cc_addr_length;
		bcopy((char *) &m->bind_ack + m->bind_ack.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		cprim.setup_ind = m->bind_ack.cc_setup_ind;
		if (verbose > 2) {
			if (cprim.addr.len) {
				printf
				    (" addr scope =    %2s |  |                               |                    \n",
				     scope_string(cprim.addr.add.scope));
				printf
				    (" addr id    = %5ld |  |                               |                    \n",
				     cprim.addr.add.id);
				printf
				    (" addr cic   = %5ld |  |                               |                    \n",
				     cprim.addr.add.cic);
			}
			printf
			    (" setup inds = %5ld |  |                               |                    \n",
			     cprim.setup_ind);
			FFLUSH(stdout);
		}
		ret = CPC_BIND_ACK;
		break;
	case CC_OPTMGMT_ACK:
		if (verbose)
			printf
			    ("<--OPTMGMT----------|  |                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = CPC_OPTMGMT_ACK;
		break;
	case CC_ADDR_ACK:
		if (verbose)
			printf
			    ("<--ADDR-------------|  |                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = CPC_ADDR_ACK;
		break;
	case CC_CALL_REATTEMPT_IND:
		ret = CPC_CALL_REATTEMPT_IND;
		cprim.user_ref = m->call_reattempt_ind.cc_user_ref;
		switch ((cprim.reason = m->call_reattempt_ind.cc_reason)) {
		case ISUP_REATTEMPT_DUAL_SIEZURE:
			printf
			    ("<--CALL-REATTEMPT---|  | (dual siezure)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_RESET:
			printf
			    ("<--CALL-REATTEMPT---|  | (reset)                       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_BLOCKING:
			printf
			    ("<--CALL-REATTEMPT---|  | (blocking)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_T24_TIMEOUT:
			printf
			    ("<--CALL-REATTEMPT---|  | (t24 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_UNEXPECTED:
			printf
			    ("<--CALL-REATTEMPT---|  | (unexpected message)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_COT_FAILURE:
			printf
			    ("<--CALL-REATTEMPT---|  | (continuity check failure)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_CIRCUIT_BUSY:
			printf
			    ("<--CALL-REATTEMPT---|  | (circuit busy)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--CALL-REATTEMPT---|  | (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
		break;
	case CC_SETUP_IND:
		printf
		    ("<--SETUP------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SETUP_IND;
		cprim.user_ref = 0;
		cprim.call_ref = m->setup_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.call_type = m->setup_ind.cc_call_type;
		cprim.call_flags = m->setup_ind.cc_call_flags;
		cprim.cdpn.len = m->setup_ind.cc_cdpn_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_cdpn_offset, &cprim.cdpn.buf,
		      cprim.cdpn.len);
		cprim.addr.len = m->setup_ind.cc_addr_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		cprim.opt.len = m->setup_ind.cc_opt_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_MORE_INFO_IND:
		printf
		    ("<--MORE-INFO--------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_MORE_INFO_IND;
		cprim.user_ref = m->more_info_ind.cc_user_ref;
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
		cprim.opt.len = m->more_info_ind.cc_opt_length;
		bcopy((char *) &m->more_info_ind + m->more_info_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_INFORMATION_IND:
		printf
		    ("<--INFORMATION------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_INFORMATION_IND;
		cprim.call_ref = m->information_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.subn.len = m->information_ind.cc_subn_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_subn_offset,
		      &cprim.subn.buf, cprim.subn.len);
		cprim.opt.len = m->information_ind.cc_opt_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_opt_offset,
		      &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_CONT_CHECK_IND:
		printf
		    ("<--CONT-CHECK-------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_CONT_CHECK_IND;
		cprim.call_ref = m->cont_check_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.addr.len = m->cont_check_ind.cc_addr_length;
		bcopy((char *) &m->cont_check_ind + m->cont_check_ind.cc_addr_offset,
		      &cprim.addr.add, cprim.addr.len);
		break;
	case CC_CONT_TEST_IND:
		printf
		    ("<--CONT-TEST--------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_CONT_TEST_IND;
		cprim.call_ref = m->cont_test_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.addr.len = m->cont_test_ind.cc_addr_length;
		bcopy((char *) &m->cont_test_ind + m->cont_test_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_CONT_REPORT_IND:
		printf
		    ("<--CONT-REPORT------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_CONT_REPORT_IND;
		cprim.call_ref = m->cont_report_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.result = m->cont_report_ind.cc_result;
		break;
	case CC_SETUP_CON:
		printf
		    ("<--SETUP-OK---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SETUP_CON;
		cprim.user_ref = m->setup_con.cc_user_ref;
		cprim.call_ref = m->setup_con.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		cprim.addr.len = m->setup_con.cc_addr_length;
		bcopy((char *) &m->setup_con + m->setup_con.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_PROCEEDING_IND:
		printf
		    ("<--PROCEEDING-------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_PROCEEDING_IND;
		cprim.call_ref = m->proceeding_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->proceeding_ind.cc_flags;
		cprim.opt.len = m->proceeding_ind.cc_opt_length;
		bcopy((char *) &m->proceeding_ind + m->proceeding_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_ALERTING_IND:
		printf
		    ("<--ALERTING---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_ALERTING_IND;
		cprim.call_ref = m->alerting_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->alerting_ind.cc_flags;
		cprim.opt.len = m->alerting_ind.cc_opt_length;
		bcopy((char *) &m->alerting_ind + m->alerting_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_PROGRESS_IND:
		ret = CPC_PROGRESS_IND;
		cprim.call_ref = m->progress_ind.cc_call_ref;
		cprim.event = m->progress_ind.cc_event;
		cprim.flags = m->progress_ind.cc_flags;
		cprim.opt.len = m->progress_ind.cc_opt_length;
		bcopy((char *) &m->progress_ind + m->progress_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		switch (m->progress_ind.cc_event & ISUP_EVNT_MASK) {
		case ISUP_EVNT_ALERTING:
			printf
			    ("<--PROGRESS---------|  | (alerting)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_PROGRESS:
			printf
			    ("<--PROGRESS---------|  | (progress)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_IBI:
			printf
			    ("<--PROGRESS---------|  | (in-band info avail)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFB:
			printf
			    ("<--PROGRESS---------|  | (call forwarded busy)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFNA:
			printf
			    ("<--PROGRESS---------|  | (call fowarded no reply)      |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFU:
			printf
			    ("<--PROGRESS---------|  | (call forwarded uncond)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--PROGRESS---------|  | (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		break;
	case CC_IBI_IND:
		printf
		    ("<--IBI--------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_IBI_IND;
		cprim.call_ref = m->ibi_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->ibi_ind.cc_flags;
		cprim.opt.len = m->ibi_ind.cc_opt_length;
		bcopy((char *) &m->ibi_ind + m->ibi_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_CONNECT_IND:
		printf
		    ("<--CONNECT----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_CONNECT_IND;
		cprim.call_ref = m->connect_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->connect_ind.cc_flags;
		cprim.opt.len = m->connect_ind.cc_opt_length;
		bcopy((char *) &m->connect_ind + m->connect_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_SETUP_COMPLETE_IND:
		printf
		    ("<--SETUP-COMPLETE---|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SETUP_COMPLETE_IND;
		cprim.call_ref = m->setup_complete_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.opt.len = m->setup_complete_ind.cc_opt_length;
		bcopy((char *) &m->setup_complete_ind + m->setup_complete_ind.cc_opt_offset,
		      &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_FORWXFER_IND:
		printf
		    ("<--FORWXFER---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_FORWXFER_IND;
		cprim.call_ref = m->forwxfer_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.opt.len = m->forwxfer_ind.cc_opt_length;
		bcopy((char *) &m->forwxfer_ind + m->forwxfer_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_SUSPEND_IND:
		printf
		    ("<--SUSPEND----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SUSPEND_IND;
		cprim.call_ref = m->suspend_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->suspend_ind.cc_flags;
		cprim.opt.len = m->suspend_ind.cc_opt_length;
		bcopy((char *) &m->suspend_ind + m->suspend_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_SUSPEND_CON:
		printf
		    ("<--SUSPEND-OK-------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SUSPEND_CON;
		cprim.call_ref = m->suspend_con.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.opt.len = m->suspend_con.cc_opt_length;
		bcopy((char *) &m->suspend_con + m->suspend_con.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_SUSPEND_REJECT_IND:
		printf
		    ("<--SUSPEND-REJECT---|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_SUSPEND_REJECT_IND;
		cprim.call_ref = m->suspend_reject_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.cause = m->suspend_reject_ind.cc_cause;
		cprim.opt.len = m->suspend_reject_ind.cc_opt_length;
		bcopy((char *) &m->suspend_reject_ind + m->suspend_reject_ind.cc_opt_offset,
		      &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_RESUME_IND:
		printf
		    ("<--RESUME-----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RESUME_IND;
		cprim.call_ref = m->resume_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.flags = m->resume_ind.cc_flags;
		cprim.opt.len = m->resume_ind.cc_opt_length;
		bcopy((char *) &m->resume_ind + m->resume_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_RESUME_CON:
		printf
		    ("<--RESUME-OK--------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RESUME_CON;
		cprim.call_ref = m->resume_con.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.opt.len = m->resume_con.cc_opt_length;
		bcopy((char *) &m->resume_con + m->resume_con.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_RESUME_REJECT_IND:
		printf
		    ("<--RESUME-REJECT----|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RESUME_REJECT_IND;
		cprim.call_ref = m->resume_reject_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		cprim.cause = m->resume_reject_ind.cc_cause;
		cprim.opt.len = m->resume_reject_ind.cc_opt_length;
		bcopy((char *) &m->resume_reject_ind + m->resume_reject_ind.cc_opt_offset,
		      &cprim.opt.buf, cprim.opt.len);
		break;
	case CC_REJECT_IND:
		printf
		    ("<--REJECT-----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_REJECT_IND;
		cprim.user_ref = m->reject_ind.cc_user_ref;
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
		cprim.cause = m->reject_ind.cc_cause;
		cprim.opt.len = m->reject_ind.cc_opt_length;
		bcopy((char *) &m->reject_ind + m->reject_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_CALL_FAILURE_IND:
		ret = CPC_CALL_FAILURE_IND;
		cprim.call_ref = m->call_failure_ind.cc_call_ref;
		cprim.reason = m->call_failure_ind.cc_reason;
		cprim.cause = m->call_failure_ind.cc_cause;
		switch (m->call_failure_ind.cc_reason) {
		case ISUP_CALL_FAILURE_COT_FAILURE:
			printf
			    ("<--CALL-FAILURE-----|  | (continuity check failure)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_RESET:
			printf
			    ("<--CALL-FAILURE-----|  | (reset)                       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_RECV_RLC:
			printf
			    ("<--CALL-FAILURE-----|  | (received RLC)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_BLOCKING:
			printf
			    ("<--CALL-FAILURE-----|  | (blocking)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T2_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t2 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T3_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t3 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T6_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t6 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T7_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t7 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T8_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t8 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T9_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t9 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T35_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t35 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T38_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----|  | (t38 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_CIRCUIT_BUSY:
			printf
			    ("<--CALL-FAILURE-----|  | (circuit busy)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--CALL-FAILURE-----|  | (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		break;
	case CC_RELEASE_IND:
		printf
		    ("<--RELEASE----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RELEASE_IND;
		cprim.user_ref = m->release_ind.cc_user_ref;
		cprim.call_ref = m->release_ind.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		cprim.cause = m->release_ind.cc_cause;
		cprim.opt.len = m->release_ind.cc_opt_length;
		bcopy((char *) &m->release_ind + m->release_ind.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_RELEASE_CON:
		printf
		    ("<--RELEASE-OK-------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RELEASE_CON;
		cprim.user_ref = m->release_con.cc_user_ref;
		cprim.call_ref = m->release_con.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     cprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		}
		cprim.opt.len = m->release_con.cc_opt_length;
		bcopy((char *) &m->release_con + m->release_con.cc_opt_offset, &cprim.opt.buf,
		      cprim.opt.len);
		break;
	case CC_RESET_IND:
		printf
		    ("<--RESET------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RESET_IND;
		cprim.flags = m->reset_ind.cc_flags;
		cprim.addr.len = m->reset_ind.cc_addr_length;
		bcopy((char *) &m->reset_ind + m->reset_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_RESET_CON:
		printf
		    ("<--RESET-OK---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_RESET_CON;
		cprim.flags = m->reset_con.cc_flags;
		cprim.addr.len = m->reset_con.cc_addr_length;
		bcopy((char *) &m->reset_con + m->reset_con.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_BLOCKING_IND:
		printf
		    ("<--BLOCK------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_BLOCKING_IND;
		cprim.flags = m->blocking_ind.cc_flags;
		cprim.addr.len = m->blocking_ind.cc_addr_length;
		bcopy((char *) &m->blocking_ind + m->blocking_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_BLOCKING_CON:
		printf
		    ("<--BLOCK-OK---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_BLOCKING_CON;
		cprim.flags = m->blocking_con.cc_flags;
		cprim.addr.len = m->blocking_con.cc_addr_length;
		bcopy((char *) &m->blocking_con + m->blocking_con.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_UNBLOCKING_IND:
		printf
		    ("<--UNBLOCK----------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_UNBLOCKING_IND;
		cprim.flags = m->unblocking_ind.cc_flags;
		cprim.addr.len = m->unblocking_ind.cc_addr_length;
		bcopy((char *) &m->unblocking_ind + m->unblocking_ind.cc_addr_offset,
		      &cprim.addr.add, cprim.addr.len);
		break;
	case CC_UNBLOCKING_CON:
		printf
		    ("<--UNBLOCK-OK-------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_UNBLOCKING_CON;
		cprim.flags = m->unblocking_con.cc_flags;
		cprim.addr.len = m->unblocking_con.cc_addr_length;
		bcopy((char *) &m->unblocking_con + m->unblocking_con.cc_addr_offset,
		      &cprim.addr.add, cprim.addr.len);
		break;
	case CC_QUERY_IND:
		printf
		    ("<--QUERY------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_QUERY_IND;
		cprim.flags = m->query_ind.cc_flags;
		cprim.addr.len = m->query_ind.cc_addr_length;
		bcopy((char *) &m->query_ind + m->query_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_QUERY_CON:
		printf
		    ("<--QUERY-OK---------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_QUERY_CON;
		cprim.flags = m->query_con.cc_flags;
		cprim.addr.len = m->query_con.cc_addr_length;
		bcopy((char *) &m->query_con + m->query_con.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		break;
	case CC_STOP_IND:
		printf
		    ("<--STOP-------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_STOP_IND;
		break;
	case CC_MAINT_IND:
		ret = CPC_MAINT_IND;
		cprim.reason = m->maint_ind.cc_reason;
		cprim.call_ref = m->maint_ind.cc_call_ref;
		cprim.addr.len = m->maint_ind.cc_addr_length;
		bcopy((char *) &m->maint_ind + m->maint_ind.cc_addr_offset, &cprim.addr.add,
		      cprim.addr.len);
		switch (m->maint_ind.cc_reason) {
		case ISUP_MAINT_T5_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t5 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T13_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t13 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T15_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t15 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T17_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t17 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T19_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t19 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T21_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t21 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T23_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t23 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T25_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t25 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T26_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t26 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T27_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t27 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T28_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t28 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T36_TIMEOUT:
			printf
			    ("<--MAINT------------|  | (t36 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGBA:
			printf
			    ("<--MAINT------------|  | (unexpected cgba)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGUA:
			printf
			    ("<--MAINT------------|  | (unexpected cgua)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_MESSAGE:
			printf
			    ("<--MAINT------------|  | (unexpected message)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEQUIPPED_CIC:
			a = (isup_addr_t *) (((unsigned char *) m) + m->maint_ind.cc_addr_offset);
			printf
			    ("<--MAINT------------|  | (unequipped cic = %5ld)      |                    (%d)\n",
			     a->cic, state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_SEGMENTATION_DISCARDED:
			printf
			    ("<--MAINT------------|  | (segmentation discarded)      |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNEQUIPPED:
			printf
			    ("<--MAINT------------|  | (user part unequipped)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNAVAILABLE:
			printf
			    ("<--MAINT------------|  | (user part unavailable)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_AVAILABLE:
			printf
			    ("<--MAINT------------|  | (user part available)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_MAN_MADE_BUSY:
			printf
			    ("<--MAINT------------|  | (user part man made busy)     |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_CONGESTED:
			printf
			    ("<--MAINT------------|  | (user part congested)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNCONGESTED:
			printf
			    ("<--MAINT------------|  | (user part uncongested)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGBA:
			printf
			    ("<--MAINT------------|  | (missing ack in cgba)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGUA:
			printf
			    ("<--MAINT------------|  | (missing ack in cgua)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGBA:
			printf
			    ("<--MAINT------------|  | (abnormal ack in cgba)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGUA:
			printf
			    ("<--MAINT------------|  | (abnormal ack in cgua)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_BLA:
			printf
			    ("<--MAINT------------|  | (unexpected bla)              |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_UBA:
			printf
			    ("<--MAINT------------|  | (unexpected uba)              |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_RELEASE_UNREC_INFO:
			printf
			    ("<--MAINT------------|  | (unrecognized information)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_RELEASE_FAILURE:
			printf
			    ("<--MAINT------------|  | (release failure)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MESSAGE_FORMAT_ERROR:
			printf
			    ("<--MAINT------------|  | (message format error)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--MAINT------------|  | (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			return (FAILURE);
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     cprim.call_ref);
		break;
	case CC_START_RESET_IND:
		printf
		    ("<--START-RESET------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = CPC_START_RESET_IND;
		break;
	default:
		printf
		    ("<--\?\?\?\?-------------|  |                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		return (UNKNOWN);
	}
	return (ret);
}

static int
iut_mgm_decode_data(struct strbuf data)
{
	return (UNKNOWN);
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
			printf
			    ("<--OK---------------+--|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
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
		FFLUSH(stdout);
		ret = MGM_ERROR_ACK;
		break;
	case CC_INFO_ACK:
		if (verbose)
			printf
			    ("<--INFO-------------+--|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = MGM_INFO_ACK;
		break;
	case CC_BIND_ACK:
		if (verbose > 1) {
			printf
			    ("<--BIND-------------+--|                               |                    (%d)\n",
			     state);
			FFLUSH(stdout);
		}
		mprim.addr.len = m->bind_ack.cc_addr_length;
		bcopy((char *) &m->bind_ack + m->bind_ack.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		mprim.setup_ind = m->bind_ack.cc_setup_ind;
		if (verbose > 2) {
			if (mprim.addr.len) {
				printf
				    (" addr scope =    %2s |  |                               |                    \n",
				     scope_string(mprim.addr.add.scope));
				printf
				    (" addr id    = %5ld |  |                               |                    \n",
				     mprim.addr.add.id);
				printf
				    (" addr cic   = %5ld |  |                               |                    \n",
				     mprim.addr.add.cic);
			}
			printf
			    (" setup inds = %5ld |  |                               |                    \n",
			     mprim.setup_ind);
			FFLUSH(stdout);
		}
		ret = MGM_BIND_ACK;
		break;
	case CC_OPTMGMT_ACK:
		if (verbose)
			printf
			    ("<--OPTMGMT----------+--|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = MGM_OPTMGMT_ACK;
		break;
	case CC_ADDR_ACK:
		if (verbose)
			printf
			    ("<--ADDR-------------+--|                               |                    (%d)\n",
			     state);
		FFLUSH(stdout);
		ret = MGM_ADDR_ACK;
		break;
	case CC_CALL_REATTEMPT_IND:
		ret = MGM_CALL_REATTEMPT_IND;
		mprim.user_ref = m->call_reattempt_ind.cc_user_ref;
		switch ((mprim.reason = m->call_reattempt_ind.cc_reason)) {
		case ISUP_REATTEMPT_DUAL_SIEZURE:
			printf
			    ("<--CALL-REATTEMPT---+--| (dual siezure)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_RESET:
			printf
			    ("<--CALL-REATTEMPT---+--| (reset)                       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_BLOCKING:
			printf
			    ("<--CALL-REATTEMPT---+--| (blocking)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_T24_TIMEOUT:
			printf
			    ("<--CALL-REATTEMPT---+--| (t24 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_UNEXPECTED:
			printf
			    ("<--CALL-REATTEMPT---+--| (unexpected message)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_COT_FAILURE:
			printf
			    ("<--CALL-REATTEMPT---+--| (continuity check failure)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_REATTEMPT_CIRCUIT_BUSY:
			printf
			    ("<--CALL-REATTEMPT---+--| (circuit busy)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--CALL-REATTEMPT---+--| (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
		break;
	case CC_SETUP_IND:
		printf
		    ("<--SETUP------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SETUP_IND;
		mprim.user_ref = 0;
		mprim.call_ref = m->setup_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.call_type = m->setup_ind.cc_call_type;
		mprim.call_flags = m->setup_ind.cc_call_flags;
		mprim.cdpn.len = m->setup_ind.cc_cdpn_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_cdpn_offset, &mprim.cdpn.buf,
		      mprim.cdpn.len);
		mprim.addr.len = m->setup_ind.cc_addr_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		mprim.opt.len = m->setup_ind.cc_opt_length;
		bcopy((char *) &m->setup_ind + m->setup_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_MORE_INFO_IND:
		printf
		    ("<--MORE-INFO--------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_MORE_INFO_IND;
		mprim.user_ref = m->more_info_ind.cc_user_ref;
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
		mprim.opt.len = m->more_info_ind.cc_opt_length;
		bcopy((char *) &m->more_info_ind + m->more_info_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_INFORMATION_IND:
		printf
		    ("<--INFORMATION------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_INFORMATION_IND;
		mprim.call_ref = m->information_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.subn.len = m->information_ind.cc_subn_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_subn_offset,
		      &mprim.subn.buf, mprim.subn.len);
		mprim.opt.len = m->information_ind.cc_opt_length;
		bcopy((char *) &m->information_ind + m->information_ind.cc_opt_offset,
		      &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_CONT_CHECK_IND:
		printf
		    ("<--CONT-CHECK-------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_CONT_CHECK_IND;
		mprim.call_ref = m->cont_check_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.addr.len = m->cont_check_ind.cc_addr_length;
		bcopy((char *) &m->cont_check_ind + m->cont_check_ind.cc_addr_offset,
		      &mprim.addr.add, mprim.addr.len);
		break;
	case CC_CONT_TEST_IND:
		printf
		    ("<--CONT-TEST--------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_CONT_TEST_IND;
		mprim.call_ref = m->cont_test_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.addr.len = m->cont_test_ind.cc_addr_length;
		bcopy((char *) &m->cont_test_ind + m->cont_test_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_CONT_REPORT_IND:
		printf
		    ("<--CONT-REPORT------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_CONT_REPORT_IND;
		mprim.call_ref = m->cont_report_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.result = m->cont_report_ind.cc_result;
		break;
	case CC_SETUP_CON:
		printf
		    ("<--SETUP-OK---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SETUP_CON;
		mprim.user_ref = m->setup_con.cc_user_ref;
		mprim.call_ref = m->setup_con.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		mprim.addr.len = m->setup_con.cc_addr_length;
		bcopy((char *) &m->setup_con + m->setup_con.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_PROCEEDING_IND:
		printf
		    ("<--PROCEEDING-------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_PROCEEDING_IND;
		mprim.call_ref = m->proceeding_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->proceeding_ind.cc_flags;
		mprim.opt.len = m->proceeding_ind.cc_opt_length;
		bcopy((char *) &m->proceeding_ind + m->proceeding_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_ALERTING_IND:
		printf
		    ("<--ALERTING---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_ALERTING_IND;
		mprim.call_ref = m->alerting_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->alerting_ind.cc_flags;
		mprim.opt.len = m->alerting_ind.cc_opt_length;
		bcopy((char *) &m->alerting_ind + m->alerting_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_PROGRESS_IND:
		ret = MGM_PROGRESS_IND;
		mprim.call_ref = m->progress_ind.cc_call_ref;
		mprim.event = m->progress_ind.cc_event;
		mprim.flags = m->progress_ind.cc_flags;
		mprim.opt.len = m->progress_ind.cc_opt_length;
		bcopy((char *) &m->progress_ind + m->progress_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		switch (m->progress_ind.cc_event & ISUP_EVNT_MASK) {
		case ISUP_EVNT_ALERTING:
			printf
			    ("<--PROGRESS---------+--| (alerting)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_PROGRESS:
			printf
			    ("<--PROGRESS---------+--| (progress)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_IBI:
			printf
			    ("<--PROGRESS---------+--| (in-band info avail)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFB:
			printf
			    ("<--PROGRESS---------+--| (call forwarded busy)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFNA:
			printf
			    ("<--PROGRESS---------+--| (call fowarded no reply)      |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_EVNT_CFU:
			printf
			    ("<--PROGRESS---------+--| (call forwarded uncond)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--PROGRESS---------+--| (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		break;
	case CC_IBI_IND:
		printf
		    ("<--IBI--------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_IBI_IND;
		mprim.call_ref = m->ibi_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->ibi_ind.cc_flags;
		mprim.opt.len = m->ibi_ind.cc_opt_length;
		bcopy((char *) &m->ibi_ind + m->ibi_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_CONNECT_IND:
		printf
		    ("<--CONNECT----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_CONNECT_IND;
		mprim.call_ref = m->connect_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->connect_ind.cc_flags;
		mprim.opt.len = m->connect_ind.cc_opt_length;
		bcopy((char *) &m->connect_ind + m->connect_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_SETUP_COMPLETE_IND:
		printf
		    ("<--SETUP-COMPLETE---+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SETUP_COMPLETE_IND;
		mprim.call_ref = m->setup_complete_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.opt.len = m->setup_complete_ind.cc_opt_length;
		bcopy((char *) &m->setup_complete_ind + m->setup_complete_ind.cc_opt_offset,
		      &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_FORWXFER_IND:
		printf
		    ("<--FORWXFER---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_FORWXFER_IND;
		mprim.call_ref = m->forwxfer_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.opt.len = m->forwxfer_ind.cc_opt_length;
		bcopy((char *) &m->forwxfer_ind + m->forwxfer_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_SUSPEND_IND:
		printf
		    ("<--SUSPEND----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SUSPEND_IND;
		mprim.call_ref = m->suspend_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->suspend_ind.cc_flags;
		mprim.opt.len = m->suspend_ind.cc_opt_length;
		bcopy((char *) &m->suspend_ind + m->suspend_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_SUSPEND_CON:
		printf
		    ("<--SUSPEND-OK-------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SUSPEND_CON;
		mprim.call_ref = m->suspend_con.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.opt.len = m->suspend_con.cc_opt_length;
		bcopy((char *) &m->suspend_con + m->suspend_con.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_SUSPEND_REJECT_IND:
		printf
		    ("<--SUSPEND-REJECT---+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_SUSPEND_REJECT_IND;
		mprim.call_ref = m->suspend_reject_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.cause = m->suspend_reject_ind.cc_cause;
		mprim.opt.len = m->suspend_reject_ind.cc_opt_length;
		bcopy((char *) &m->suspend_reject_ind + m->suspend_reject_ind.cc_opt_offset,
		      &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_RESUME_IND:
		printf
		    ("<--RESUME-----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RESUME_IND;
		mprim.call_ref = m->resume_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.flags = m->resume_ind.cc_flags;
		mprim.opt.len = m->resume_ind.cc_opt_length;
		bcopy((char *) &m->resume_ind + m->resume_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_RESUME_CON:
		printf
		    ("<--RESUME-OK--------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RESUME_CON;
		mprim.call_ref = m->resume_con.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.opt.len = m->resume_con.cc_opt_length;
		bcopy((char *) &m->resume_con + m->resume_con.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_RESUME_REJECT_IND:
		printf
		    ("<--RESUME-REJECT----+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RESUME_REJECT_IND;
		mprim.call_ref = m->resume_reject_ind.cc_call_ref;
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		mprim.cause = m->resume_reject_ind.cc_cause;
		mprim.opt.len = m->resume_reject_ind.cc_opt_length;
		bcopy((char *) &m->resume_reject_ind + m->resume_reject_ind.cc_opt_offset,
		      &mprim.opt.buf, mprim.opt.len);
		break;
	case CC_REJECT_IND:
		printf
		    ("<--REJECT-----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_REJECT_IND;
		mprim.user_ref = m->reject_ind.cc_user_ref;
		if (verbose)
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
		mprim.cause = m->reject_ind.cc_cause;
		mprim.opt.len = m->reject_ind.cc_opt_length;
		bcopy((char *) &m->reject_ind + m->reject_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_CALL_FAILURE_IND:
		ret = MGM_CALL_FAILURE_IND;
		mprim.call_ref = m->call_failure_ind.cc_call_ref;
		mprim.reason = m->call_failure_ind.cc_reason;
		mprim.cause = m->call_failure_ind.cc_cause;
		switch (m->call_failure_ind.cc_reason) {
		case ISUP_CALL_FAILURE_COT_FAILURE:
			printf
			    ("<--CALL-FAILURE-----+--| (continuity check failure)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_RESET:
			printf
			    ("<--CALL-FAILURE-----+--| (reset)                       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_RECV_RLC:
			printf
			    ("<--CALL-FAILURE-----+--| (received RLC)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_BLOCKING:
			printf
			    ("<--CALL-FAILURE-----+--| (blocking)                    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T2_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t2 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T3_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t3 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T6_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t6 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T7_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t7 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T8_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t8 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T9_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t9 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T35_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t35 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_T38_TIMEOUT:
			printf
			    ("<--CALL-FAILURE-----+--| (t38 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_CALL_FAILURE_CIRCUIT_BUSY:
			printf
			    ("<--CALL-FAILURE-----+--| (circuit busy)                |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--CALL-FAILURE-----+--| (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		break;
	case CC_RELEASE_IND:
		printf
		    ("<--RELEASE----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RELEASE_IND;
		mprim.user_ref = m->release_ind.cc_user_ref;
		mprim.call_ref = m->release_ind.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		mprim.cause = m->release_ind.cc_cause;
		mprim.opt.len = m->release_ind.cc_opt_length;
		bcopy((char *) &m->release_ind + m->release_ind.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_RELEASE_CON:
		printf
		    ("<--RELEASE-OK-------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RELEASE_CON;
		mprim.user_ref = m->release_con.cc_user_ref;
		mprim.call_ref = m->release_con.cc_call_ref;
		if (verbose) {
			printf
			    ("user ref = %-4ld     |  |                               |                    \n",
			     mprim.user_ref);
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		}
		mprim.opt.len = m->release_con.cc_opt_length;
		bcopy((char *) &m->release_con + m->release_con.cc_opt_offset, &mprim.opt.buf,
		      mprim.opt.len);
		break;
	case CC_RESET_IND:
		printf
		    ("<--RESET------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RESET_IND;
		mprim.flags = m->reset_ind.cc_flags;
		mprim.addr.len = m->reset_ind.cc_addr_length;
		bcopy((char *) &m->reset_ind + m->reset_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_RESET_CON:
		printf
		    ("<--RESET-OK---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_RESET_CON;
		mprim.flags = m->reset_con.cc_flags;
		mprim.addr.len = m->reset_con.cc_addr_length;
		bcopy((char *) &m->reset_con + m->reset_con.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_BLOCKING_IND:
		printf
		    ("<--BLOCK------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_BLOCKING_IND;
		mprim.flags = m->blocking_ind.cc_flags;
		mprim.addr.len = m->blocking_ind.cc_addr_length;
		bcopy((char *) &m->blocking_ind + m->blocking_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_BLOCKING_CON:
		printf
		    ("<--BLOCK-OK---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_BLOCKING_CON;
		mprim.flags = m->blocking_con.cc_flags;
		mprim.addr.len = m->blocking_con.cc_addr_length;
		bcopy((char *) &m->blocking_con + m->blocking_con.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_UNBLOCKING_IND:
		printf
		    ("<--UNBLOCK----------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_UNBLOCKING_IND;
		mprim.flags = m->unblocking_ind.cc_flags;
		mprim.addr.len = m->unblocking_ind.cc_addr_length;
		bcopy((char *) &m->unblocking_ind + m->unblocking_ind.cc_addr_offset,
		      &mprim.addr.add, mprim.addr.len);
		break;
	case CC_UNBLOCKING_CON:
		printf
		    ("<--UNBLOCK-OK-------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_UNBLOCKING_CON;
		mprim.flags = m->unblocking_con.cc_flags;
		mprim.addr.len = m->unblocking_con.cc_addr_length;
		bcopy((char *) &m->unblocking_con + m->unblocking_con.cc_addr_offset,
		      &mprim.addr.add, mprim.addr.len);
		break;
	case CC_QUERY_IND:
		printf
		    ("<--QUERY------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_QUERY_IND;
		mprim.flags = m->query_ind.cc_flags;
		mprim.addr.len = m->query_ind.cc_addr_length;
		bcopy((char *) &m->query_ind + m->query_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_QUERY_CON:
		printf
		    ("<--QUERY-OK---------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_QUERY_CON;
		mprim.flags = m->query_con.cc_flags;
		mprim.addr.len = m->query_con.cc_addr_length;
		bcopy((char *) &m->query_con + m->query_con.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		break;
	case CC_STOP_IND:
		printf
		    ("<--STOP-------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_STOP_IND;
		break;
	case CC_MAINT_IND:
		ret = MGM_MAINT_IND;
		mprim.reason = m->maint_ind.cc_reason;
		mprim.call_ref = m->maint_ind.cc_call_ref;
		mprim.addr.len = m->maint_ind.cc_addr_length;
		bcopy((char *) &m->maint_ind + m->maint_ind.cc_addr_offset, &mprim.addr.add,
		      mprim.addr.len);
		switch (m->maint_ind.cc_reason) {
		case ISUP_MAINT_T5_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t5 timeout)                  |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T13_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t13 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T15_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t15 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T17_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t17 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T19_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t19 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T21_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t21 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T23_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t23 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T25_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t25 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T26_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t26 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T27_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t27 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T28_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t28 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_T36_TIMEOUT:
			printf
			    ("<--MAINT------------+--| (t36 timeout)                 |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGBA:
			printf
			    ("<--MAINT------------+--| (unexpected cgba)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_CGUA:
			printf
			    ("<--MAINT------------+--| (unexpected cgua)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_MESSAGE:
			printf
			    ("<--MAINT------------+--| (unexpected message)          |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEQUIPPED_CIC:
			a = (isup_addr_t *) (((unsigned char *) m) + m->maint_ind.cc_addr_offset);
			printf
			    ("<--MAINT------------|  | (unequipped cic = %5ld)      |                    (%d)\n",
			     a->cic, state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_SEGMENTATION_DISCARDED:
			printf
			    ("<--MAINT------------+--| (segmentation discarded)      |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNEQUIPPED:
			printf
			    ("<--MAINT------------+--| (user part unequipped)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNAVAILABLE:
			printf
			    ("<--MAINT------------+--| (user part unavailable)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_AVAILABLE:
			printf
			    ("<--MAINT------------+--| (user part available)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_MAN_MADE_BUSY:
			printf
			    ("<--MAINT------------+--| (user part man made busy)     |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_CONGESTED:
			printf
			    ("<--MAINT------------+--| (user part congested)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_USER_PART_UNCONGESTED:
			printf
			    ("<--MAINT------------+--| (user part uncongested)       |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGBA:
			printf
			    ("<--MAINT------------+--| (missing ack in cgba)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MISSING_ACK_IN_CGUA:
			printf
			    ("<--MAINT------------+--| (missing ack in cgua)         |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGBA:
			printf
			    ("<--MAINT------------+--| (abnormal ack in cgba)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_ABNORMAL_ACK_IN_CGUA:
			printf
			    ("<--MAINT------------+--| (abnormal ack in cgua)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_BLA:
			printf
			    ("<--MAINT------------+--| (unexpected bla)              |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_UNEXPECTED_UBA:
			printf
			    ("<--MAINT------------+--| (unexpected uba)              |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_RELEASE_UNREC_INFO:
			printf
			    ("<--MAINT------------+--| (unrecognized information)    |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_RELEASE_FAILURE:
			printf
			    ("<--MAINT------------+--| (release failure)             |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		case ISUP_MAINT_MESSAGE_FORMAT_ERROR:
			printf
			    ("<--MAINT------------+--| (message format error)        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			break;
		default:
			printf
			    ("<--MAINT------------+--| (\?\?\?\?)                        |                    (%d)\n",
			     state);
			FFLUSH(stdout);
			return (FAILURE);
		}
		if (verbose)
			printf
			    ("call ref = %-4ld     |  |                               |                    \n",
			     mprim.call_ref);
		break;
	case CC_START_RESET_IND:
		printf
		    ("<--START-RESET------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		ret = MGM_START_RESET_IND;
		break;
	default:
		printf
		    ("<--\?\?\?\?-------------+--|                               |                    (%d)\n",
		     state);
		FFLUSH(stdout);
		return (UNKNOWN);
	}
	return (ret);
}

static int show_timeout = 0;
static int last_event = 0;

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
				printf
				    ("++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++(%d)\n",
				     state);
				FFLUSH(stdout);
				show_timeout--;
			}
			last_event = TIMEOUT;
			return time_event(TIMEOUT);
		}
		// printf("polling:\n");
		// FFLUSH(stdout);
		pfd[0].fd = pt_fd;
		pfd[0].events =
		    (source & PT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR
				     | POLLHUP) : 0;
		pfd[0].revents = 0;
		pfd[1].fd = iut_cpc_fd;
		pfd[1].events =
		    (source & CPC) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR
				      | POLLHUP) : 0;
		pfd[1].revents = 0;
		pfd[2].fd = iut_tst_fd;
		pfd[2].events =
		    (source & TST) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR
				      | POLLHUP) : 0;
		pfd[2].revents = 0;
		pfd[3].fd = iut_mgm_fd;
		pfd[3].events =
		    (source & MGM) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR
				      | POLLHUP) : 0;
		pfd[3].revents = 0;
		pfd[4].fd = iut_mnt_fd;
		pfd[4].events =
		    (source & MNT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR
				      | POLLHUP) : 0;
		pfd[4].revents = 0;
		switch (poll(pfd, 5, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			if (verbose) {
				printf
				    ("X-X-X-X-X-X-X-X-X-X-|-X|X-X-X-X-X- ERROR! -X-X-X-X-X-X-|-X-X-X-X-X-X-X-X-X-X(%d)\n",
				     state);
				FFLUSH(stdout);
			}
			break;
		case 0:
			if (verbose) {
				printf
				    ("- - - - - - - - - - | -|- - - - - nothing! - - - - - - | - - - - - - - - - -(%d)\n",
				     state);
				FFLUSH(stdout);
			}
			last_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			// printf("polled:\n");
			// FFLUSH(stdout);
			if (pfd[4].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, iut_ctl };
				struct strbuf data = { BUFSIZE, 0, iut_dat };
				// printf("getmsg from iut_mgm:\n");
				// FFLUSH(stdout);
				if (getmsg(iut_mnt_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (ctrl.len > 0) {
						if ((last_event =
						     iut_mgm_decode_msg(ctrl, data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event =
						     iut_mgm_decode_data(data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			// printf("polled:\n");
			// FFLUSH(stdout);
			if (pfd[3].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, iut_ctl };
				struct strbuf data = { BUFSIZE, 0, iut_dat };
				// printf("getmsg from iut_mgm:\n");
				// FFLUSH(stdout);
				if (getmsg(iut_mgm_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (ctrl.len > 0) {
						if ((last_event =
						     iut_mgm_decode_msg(ctrl, data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event =
						     iut_mgm_decode_data(data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			// printf("polled:\n");
			// FFLUSH(stdout);
			if (pfd[2].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, iut_ctl };
				struct strbuf data = { BUFSIZE, 0, iut_dat };
				// printf("getmsg from iut_mgm:\n");
				// FFLUSH(stdout);
				if (getmsg(iut_tst_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (ctrl.len > 0) {
						if ((last_event =
						     iut_mgm_decode_msg(ctrl, data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event =
						     iut_mgm_decode_data(data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[1].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, iut_ctl };
				struct strbuf data = { BUFSIZE, 0, iut_dat };
				// printf("getmsg from iut:\n");
				// FFLUSH(stdout);
				if (getmsg(iut_cpc_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (ctrl.len > 0) {
						if ((last_event =
						     iut_cpc_decode_msg(ctrl, data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event =
						     iut_cpc_decode_data(data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[0].revents) {
				int flags = 0;
				struct strbuf ctrl = { BUFSIZE, 0, pt_ctl };
				struct strbuf data = { BUFSIZE, 0, pt_dat };
				// printf("getmsg from pt:\n");
				// FFLUSH(stdout);
				if (getmsg(pt_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from pt [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (ctrl.len > 0) {
						if ((last_event =
						     pt_decode_msg(ctrl, data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = pt_decode_data(data)) != UNKNOWN)
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
	// if (send(RESUME) != SUCCESS)
	// return FAILURE;
	return SUCCESS;
};

static int
preamble_1(void)
{
	/* start with a blocked circuit at SP A */
	state = 0;
	start_tt(1000);
	// if (send(RESUME) != SUCCESS)
	// return FAILURE;
	state = 1;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(BLOCKING_REQ);
	state = 2;
	if (pt_event() != BLO)
		return FAILURE;
	state = 3;
	pmsg.cic = 12;
	send(BLA);
	state = 4;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	return SUCCESS;
}

static int
postamble_0(void)
{
	while (wait_event(0, ANY) != NO_MSG) ;
	state = 0;
	stop_tt();
	if (send(PAUSE) != SUCCESS)
		return FAILURE;
	return SUCCESS;
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
	return SUCCESS;
      failure:
	stop_tt();
	send(PAUSE);
	return FAILURE;
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
	return SUCCESS;
      failure:
	stop_tt();
	send(PAUSE);
	return FAILURE;
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  The Test Cases...
 *
 *  -------------------------------------------------------------------------
 */

#define desc_case_1_1 "\
Circuit supervision\n\
-- Non-allocated circuits\n\
Verify that on receipt of a CIC relating to a circuit which does not exist,\n\
SP A will discard the message and alert the maintenance system."
static int
test_case_1_1(void)
{
	state = 0;
	pmsg.cic = 16;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 2;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_2_1 "\
Reset of circuits\n\
-- RSC received on an idle circuit\n\
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
		return FAILURE;
	state = 2;
	iut_mgm_signal(RESET_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != RLC)
		return FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
	state = 2;
	pmsg.cic = 12;
	send(RLC);
	state = 3;
	if (mgm_event() != MGM_RESET_CON)
		return FAILURE;
	state = 4;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
	state = 2;
	pmsg.cic = 12;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 4;
	pmsg.cic = 12;
	send(RSC);
	state = 5;
	if (mgm_event() != MGM_RESET_IND)
		return FAILURE;
	state = 6;
	iut_mgm_signal(RESET_RES);
	state = 7;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 8;
	if (pt_event() != BLO)
		return FAILURE;
	state = 9;
	pmsg.cic = 12;
	send(BLA);
	state = 10;
	if (pt_event() != RLC)
		return FAILURE;
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return FAILURE;
	state = 5;
	pmsg.cic = 12;
	send(RSC);
	state = 6;
	if (mgm_event() != MGM_RESET_IND)
		return FAILURE;
	state = 7;
	iut_mgm_signal(RESET_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 9;
	if (pt_event() != RLC)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	state = 11;
	return SUCCESS;
}

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
		return FAILURE;
	state = 2;
	iut_mgm_signal(RESET_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != GRA)
		return FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	state = 6;
	return SUCCESS;
}

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
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
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
		return FAILURE;
	state = 4;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

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
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return FAILURE;
	state = 5;
	pmsg.cic = 13;
	send(BLO);
	state = 6;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 7;
	iut_mgm_signal(BLOCKING_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 9;
	if (pt_event() != BLA)
		return FAILURE;
	state = 10;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 30;
	pmsg.rs.len = 1;
	send(GRS);
	state = 11;
	if (mgm_event() != MGM_RESET_IND)
		return FAILURE;
	state = 12;
	iut_mgm_signal(RESET_RES);
	state = 13;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 14;
	if (pt_event() != GRA)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
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
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != CGBA)
		return FAILURE;
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
		return FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 10;
	if (pt_event() != CGUA)
		return FAILURE;
	/* FIXME: In here we should really attempt to launch some calls to the blocked circuits */
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
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
		return FAILURE;
	state = 2;
	pmsg.cic = 1;
	pmsg.rs.buf[0] = 0;
	pmsg.rs.len = 1;
	pmsg.cgi = cgi;
	send(CGU);
	state = 3;
	if (wait_event(100, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
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
		return FAILURE;
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
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_1_1a "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Maintenance Oriented, valid range\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1a(void)
{
	return test_case_1_3_1_1x(ISUP_MAINTENANCE_ORIENTED);
}

#define desc_case_1_3_1_1b "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Maintenance Oriented, range = 0\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1b(void)
{
	return test_case_1_3_1_1y(ISUP_MAINTENANCE_ORIENTED);
}

#define desc_case_1_3_1_1c "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Maintenance Oriented, range > 32\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1c(void)
{
	return test_case_1_3_1_1z(ISUP_MAINTENANCE_ORIENTED);
}

#define desc_case_1_3_1_1d "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Hardware Failure Oriented, valid range\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1d(void)
{
	return test_case_1_3_1_1x(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define desc_case_1_3_1_1e "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Hardware Failure Oriented, range = 0\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1e(void)
{
	return test_case_1_3_1_1y(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define desc_case_1_3_1_1f "\
Circuit group blocking/unblocking\n\
-- CGB and CGU received\n\
-- Hardware Failure Oriented, range > 32\n\
Verify that the circuit group blocking feature can be correctly initiated."
static int
test_case_1_3_1_1f(void)
{
	return test_case_1_3_1_1z(ISUP_HARDWARE_FAILURE_ORIENTED);
}

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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGBA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 4;
	mprim.addr.add.scope = ISUP_SCOPE_CG;
	mprim.addr.add.id = 1;
	mprim.addr.add.cic = 1;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_GROUP | cgi;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 5;
	if (pt_event() != CGU)
		return FAILURE;
	state = 6;
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGUA);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return FAILURE;
	state = 8;
	/* make sure both SP's can initiate calls */
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_1_2a "\
Circuit group blocking/unblocking\n\
-- CGB and CGU sent\n\
-- Maintenance oriented\n\
Verify that SP A is able to generate one circuit group blocking message and\n\
one circuit group unblocking message."
static int
test_case_1_3_1_2a(void)
{
	return test_case_1_3_1_2x(ISUP_MAINTENANCE_ORIENTED);
}

#define desc_case_1_3_1_2b "\
Circuit group blocking/unblocking\n\
-- CGB and CGU sent\n\
-- Hardware failure oriented\n\
Verify that SP A is able to generate one circuit group blocking message and\n\
one circuit group unblocking message."
static int
test_case_1_3_1_2b(void)
{
	return test_case_1_3_1_2x(ISUP_HARDWARE_FAILURE_ORIENTED);
}

#define desc_case_1_3_2_1 "\
Circuit blocking/unblocking\n\
-- BLO received\n\
Verify that the blocking/unblocking procedure can be correctly initiated."
static int
test_case_1_3_2_1(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return FAILURE;
	state = 5;
	/* FIXME: Make sure that a call cannot be made */
	state = 6;
	pmsg.cic = 12;
	send(UBL);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 10;
	if (pt_event() != UBA)
		return FAILURE;
	state = 11;
	/* FIXME: Make sure that a call can be made */
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_2_2 "\
Circuit blocking/unblocking\n\
-- BLO sent\n\
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 4;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 5;
	if (pt_event() != UBL)
		return FAILURE;
	state = 6;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return FAILURE;
	state = 8;
	/* make sure that both SP's can initiate calls */
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_2_3 "\
Circuit blocking/unblocking\n\
-- Blocking from both ends; removal of blocking from one end.\n\
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 3;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 4;
	pmsg.cic = 12;
	send(BLO);
	state = 5;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 6;
	iut_mgm_signal(BLOCKING_RES);
	state = 7;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 8;
	if (pt_event() != BLA)
		return FAILURE;
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
		return FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 13;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return FAILURE;
	state = 14;
	/* FIXME: make sure that both SP's can initiate calls */
	state = 15;
	pmsg.cic = 12;
	send(UBL);
	state = 16;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 17;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 18;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 19;
	if (pt_event() != UBA)
		return FAILURE;
	state = 20;
	/* Make sure that a call can be made */
	state = 21;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_2_4 "\
Circuit blocking/unblocking\n\
-- IAM received on a remotely blocked circuit\n\
Verify that an IAM will unblock a remotely blocked circuit."
static int
test_case_1_3_2_4(void)
{
	state = 0;
	pmsg.cic = 12;
	send(BLO);
	state = 1;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != BLA)
		return FAILURE;
	state = 5;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 6;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 7;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 8;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 9;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 10;
	iut_cpc_signal(SETUP_RES);
	state = 11;
	if (cpc_event() != CPC_OK_ACK)
		return (FAILURE);
	state = 12;
	cprim.flags =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 13;
	if (pt_event() != ACM)
		return FAILURE;
	state = 14;
	iut_cpc_signal(RINGING);
	state = 15;
	iut_cpc_signal(CONNECT_REQ);
	state = 16;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 17;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 21;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 22;
	if (pt_event() != RLC)
		return FAILURE;
	state = 23;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_3_2_5 "\
Circuit blocking/unblocking\n\
-- Blocking with CGB, unblocking with UBL\n\
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
		return FAILURE;
	state = 2;
	iut_mgm_signal(BLOCKING_RES);
	state = 3;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 4;
	if (pt_event() != CGBA)
		return FAILURE;
	state = 5;
	pmsg.cic = 12;
	send(UBL);
	state = 6;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 7;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 8;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 9;
	if (pt_event() != UBA)
		return FAILURE;
	state = 10;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 13;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 14;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 15;
	send(RINGING);
	state = 16;
	send(ANM);
	state = 17;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 18;
	iut_cpc_signal(COMMUNICATION);
	state = 19;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 20;
	if (pt_event() != REL)
		return FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 22;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
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
		return FAILURE;
	state = 25;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 26;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 27;
	if (pt_event() != CGUA)
		return FAILURE;
	state = 28;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_4_1 "\
Continuity check procedure\n\
-- CCR received: successful\n\
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
		return FAILURE;
	state = 2;
	mprim.token_value = 0;
	iut_tst_signal(LOOPBACK);
	state = 3;
	iut_tst_signal(CONT_TEST_REQ);
	state = 4;
	send(TONE);
	state = 5;
	if (wait_event(100, ANY) != NO_MSG)
		return FAILURE;
	state = 6;
	pmsg.cic = 12;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 7;
	if (tst_event() != MGM_RELEASE_IND)
		return FAILURE;
	state = 8;
	mprim.addr.len = 0;
	mprim.opt.len = 0;
	mprim.user_ref = 0;
	mprim.user_ref = 0;
	iut_tst_signal(RELEASE_RES);
	if (tst_event() != MGM_OK_ACK)
		return FAILURE;
	state = 9;
	if (pt_event() != RLC)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_4_2 "\
Continuity check procedure\n\
-- CCR sent: successful\n\
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
		return FAILURE;
	state = 2;
	if (pt_event() != CCR)
		return FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	start_st(timer[t24].lo / 2);
	iut_tst_signal(TONE);
	state = 5;
	if (any_event() != TIMEOUT)
		return FAILURE;
	state = 6;
	start_tt(100);
	mprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	mprim.user_ref = 0;
	iut_tst_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (tst_event() != MGM_RELEASE_CON)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_4_3 "\
Continuity check procedure\n\
-- CCR received: unsuccessful\n\
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
		return FAILURE;
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
	if (any_event() != TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return FAILURE;
	}
	start_tt(100);
	state = 7;
	pmsg.coti = ISUP_COT_FAILURE;
	send(COT);
	state = 8;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 9;
	beg_time = milliseconds(" T26  ");
	state = 10;
	start_st(timer[t26].lo);
	if (any_event() != TIMEOUT) {
		check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
		return FAILURE;
	}
	start_tt(100);
	state = 11;
	send(CCR);
	state = 12;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return FAILURE;
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
	if (any_event() != TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return FAILURE;
	}
	start_tt(100);
	state = 18;
	pmsg.coti = ISUP_COT_FAILURE;
	send(COT);
	state = 19;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 20;
	beg_time = milliseconds(" T26  ");
	state = 21;
	start_st(timer[t26].lo);
	if (any_event() != TIMEOUT) {
		check_time(" T26  ", now() - beg_time, timer[t26].lo, timer[t26].hi);
		return FAILURE;
	}
	start_tt(100);
	state = 22;
	send(CCR);
	state = 23;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return FAILURE;
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
	if (any_event() != TIMEOUT) {
		check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
		return FAILURE;
	}
	start_tt(100);
	state = 29;
	pmsg.coti = ISUP_COT_SUCCESS;
	send(COT);
	state = 30;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 31;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_4_4 "\
Continuity check procedure\n\
-- CCR sent: unsuccessful\n\
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
		return FAILURE;
	state = 2;
	if (pt_event() != CCR)
		return FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_tst_signal(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	state = 6;
	start_st(timer[t24].hi + 100);
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 7;
	if (pt_event() != COT)
		return FAILURE;
	state = 8;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 9;
	beg_time = milliseconds(" T26  ");
	state = 10;
	start_st(timer[t26].hi + 100);
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 11;
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 12;
	if (pt_event() != CCR)
		return FAILURE;
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
		return FAILURE;
	state = 18;
	if (pt_event() != COT)
		return FAILURE;
	state = 19;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 20;
	beg_time = milliseconds(" T26  ");
	state = 21;
	start_st(timer[t26].hi + 100);
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 22;
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 23;
	if (pt_event() != CCR)
		return FAILURE;
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
		return FAILURE;
	state = 26;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_4_5 "\
Continuity check procedure\n\
-- CCR not received unsuccessful; verify T27 timer\n\
Verify that the continuity check procedure for the proper alignment of\n\
circuits can be correctly received."
static int
test_case_1_4_5(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_REQUIRED;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
		return FAILURE;
	state = 3;
	if (tst_event() != MGM_CONT_CHECK_IND)
		return FAILURE;
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
		return FAILURE;
	state = 8;
	beg_time = milliseconds(" T27  ");
	state = 9;
	start_st(timer[t27].hi + 100);
	if (pt_event() != RSC)
		return FAILURE;
	state = 10;
	if (check_time(" T27  ", now() - beg_time, timer[t27].lo, timer[t27].hi) != SUCCESS)
		return FAILURE;
	start_tt(1000);
	state = 11;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 12;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 13;
	send(RLC);
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_1_5_1 "\
Receipt of unreasonable signalling information messages\n\
-- Receipt of unexpected messages\n\
Verify that SP A is able to handle unexpected messages."
static int
test_case_1_5_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_5_2 "\
Receipt of unreasonable signalling information messages\n\
-- Receipt of unexpected messages during call setup\n\
Verify that SP A is able to handle unexpected emssages."
static int
test_case_1_5_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_5_3 "\
Receipt of unreasonable signalling information messages\n\
-- Receipt of unexpected messages during a call\n\
Verify that SP A is able to handle unexpected emssages."
static int
test_case_1_5_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_1_1 "\
Receipt of unknown messages\n\
-- Receipt of unknown message in forward direction\n\
Verify that SP A is able to discard an unknown message without disrupting\n\
normal call handling."
static int
test_case_1_6_1_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_1_2 "\
Receipt of unknown messages\n\
-- Receipt of unknown message in backward direction\n\
Verify that SP A is able to discard an unknown message without disrupting\n\
normal call handling."
static int
test_case_1_6_1_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_2_1 "\
Receipt of unknown parameters\n\
-- Receipt of unknown parameters in forward direction\n\
Verify that SP A is able to discard an unknown parameter without disrupting\n\
normal call handling."
static int
test_case_1_6_2_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_2_2 "\
Receipt of unknown parameters\n\
-- Receipt of unknown parameters in backward direction\n\
Verify that SP A is able to discard an unknown parameter without disrupting\n\
normal call handling."
static int
test_case_1_6_2_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_3_1 "\
Receipt of unknown parameter values\n\
-- Receipt of unknown parameter values in forward direction\n\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_6_3_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_6_3_2 "\
Receipt of unknown parameter values\n\
-- Receipt of unknown parameter values in backward direction\n\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_6_3_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_1 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Release call\n\
Verify that SP A release the call, if indicated in the Message Compatibility\n\
Information."
static int
test_case_1_7_1_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_2 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Discard message\n\
Verify that SP A is able to discard an unknown message, if indicated in the\n\
Message Compatibility Information."
static int
test_case_1_7_1_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_3 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Pass on\n\
Verify that SP A is able to pass on an unknown message, without notification."
static int
test_case_1_7_1_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_4 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Pass on not possible, release call\n\
Verify that SP A release the call if pass on not possible and if indicated in\n\
the Message Compatibility Information."
static int
test_case_1_7_1_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_5 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Pass on not possible, discard information\n\
Verify that SP A is able to discard an unknown message if pass on not possible\n\
and if indicated in the Message Compatibility Information."
static int
test_case_1_7_1_5(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_6 "\
Receipt of unknown messages\n\
-- Message Compatibility Information: Transit interpretation\n\
Verify that SP A (Type B exchange) is able to ignore the remaining part of the\n\
Instruction indicator, if A = 0."
static int
test_case_1_7_1_6(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_1_7 "\
Receipt of unknown messages\n\
-- Message Compatibility Information not received\n\
Verify that SP A is able to discard an unknown message without Message\n\
Compatibility Information."
static int
test_case_1_7_1_7(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_1 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Release call\n\
Verify that SP A is able to release the call, if indicated in Parameter\n\
Compatibility Information."
static int
test_case_1_7_2_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_2 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Discard message\n\
Verify that SP A is able to discard a message containing an unknown parameter,\n\
if indicated in the Parameter Compatibility Information."
static int
test_case_1_7_2_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_3 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Discard parameter\n\
Verify that SP A is able to discard an unknown parameter, if indicated in the\n\
Parameter Compatibility Information."
static int
test_case_1_7_2_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_4 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Pass on\n\
Verify that SP A is able to pass on an unknown parameter, without\n\
notification."
static int
test_case_1_7_2_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_5 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Pass on not possisble, release call\n\
Verify that SP A releases call if pass on is not possible and if indicated in\n\
Parameter Compatibility Information."
static int
test_case_1_7_2_5(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_6 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Pass on not possisble, discard message\n\
Verify that SP A is able to discard a message containing an unknown parameter\n\
if pass on is not possible and if indicated in Parameter Compatibility\n\
Information."
static int
test_case_1_7_2_6(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_7 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Pass on not possisble, discard parameter\n\
Verify that SP A is able to discard an unknown parameter if pass on is not\n\
possible and if indicated in the Parameter Compatibility Information."
static int
test_case_1_7_2_7(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_8 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information: Transit interpretation\n\
Verify that SP A (Type B exchange) is able to ignore the remaining part of the\n\
Instruction indicator, if A = 0."
static int
test_case_1_7_2_8(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_9 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information not received\n\
Verify that SP A is able to handle an unknown parameter without Parameter\n\
Compatibility Information."
static int
test_case_1_7_2_9(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_2_10 "\
Receipt of unknown parameters\n\
-- Parameter Compatibility Information not received in REL\n\
Verify that SP A is able to discard an unknown parameter in a REL without\n\
Parameter Compatiblity Information."
static int
test_case_1_7_2_10(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_3_1 "\
Receipt of unknown parameter values\n\
-- Receipt of unknown parameter values in forward direction\n\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_7_3_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_1_7_3_2 "\
Receipt of unknown parameter values\n\
-- Receipt of unknown parameter values in backward direction\n\
Verify that SP A is able to handle unknown parameter values."
static int
test_case_1_7_3_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_2_1_1 "\
Both way circuit selection\n\
-- IAM sent by controlling SP\n\
Verify that SP A can initiate an outgoing call on a circuit capable of both\n\
way operation when the controlling SP is A."
static int
test_case_2_1_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_1_2 "\
Both way circuit selection\n\
-- IAM sent by non-controlling SP\n\
Verify that SP A can initiate an outgoing call on a circuit capable of both\n\
way operation when the non-controlling SP is A."
static int
test_case_2_1_2(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 12;
	if (pt_event() != RLC)
		return FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_2_1a "\
Called addresss sending\n\
-- \"en bloc\" operation\n\
-- outgoing call\n\
Verify that a call can be successfully establish (all digits included in the\n\
IAM)."
static int
test_case_2_2_1a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_2_1b "\
Called addresss sending\n\
-- \"en bloc\" operation\n\
-- incoming call\n\
Verify that a call can be successfully establish (all digits included in the\n\
IAM)."
static int
test_case_2_2_1b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_2_2a "\
Called addresss sending\n\
-- Overlap operation (with SAM)\n\
-- outgoing call\n\
Verify that SP A can initiate a call using an IAM followed by a SAM."
static int
test_case_2_2_2a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 1;
	if (pt_event() != IAM)
		return FAILURE;
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
		return FAILURE;
	state = 3;
	if (pt_event() != SAM)
		return FAILURE;
	state = 4;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 5;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 6;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 7;
	send(RINGING);
	state = 8;
	send(ANM);
	state = 9;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 12;
	if (pt_event() != REL)
		return FAILURE;
	state = 13;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 14;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_2_2b "\
Called addresss sending\n\
-- Overlap operation (with SAM)\n\
-- incoming call\n\
Verify that SP A can initiate a call using an IAM followed by a SAM."
static int
test_case_2_2_2b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "1780", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	strncpy(pmsg.subn.num, "5551212/", 24);
	pmsg.subn.len = strnlen(pmsg.subn.num, 24);
	send(SAM);
	state = 5;
	if (cpc_event() != CPC_INFORMATION_IND)
		return FAILURE;
	state = 6;
	cprim.flags =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 7;
	if (pt_event() != ACM)
		return FAILURE;
	state = 8;
	iut_cpc_signal(RINGING);
	state = 9;
	iut_cpc_signal(CONNECT_REQ);
	state = 10;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 11;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 15;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_1x "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- outgoing call\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1x(ulong bci)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | bci;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	switch (cpc_event()) {
	case CPC_ALERTING_IND:
		state = 5;
		send(RINGING);
		state = 6;
		if (!(bci & ISUP_BCI_SUBSCRIBER_FREE))
			return FAILURE;
		state = 7;
		send(ANM);
		break;
	case CPC_PROCEEDING_IND:
		state = 6;
		if ((bci & ISUP_BCI_SUBSCRIBER_FREE))
			return FAILURE;
		state = 7;
		send(ANM);
		break;
	default:
		return FAILURE;
	}
	state = 8;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 9;
	iut_cpc_signal(COMMUNICATION);
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 11;
	if (pt_event() != REL)
		return FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 13;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_1a "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- outgoing call\n\
-- \"subscriber free\" and \"terminating access isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1a(void)
{
	return test_case_2_3_1x(ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define desc_case_2_3_1b "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- outgoing call\n\
-- \"subscriber free\" and \"terminating access non-isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1b(void)
{
	return test_case_2_3_1x(ISUP_BCI_SUBSCRIBER_FREE);
}

#define desc_case_2_3_1c "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- outgoing call\n\
-- \"no indication\" and \"terminating access isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1c(void)
{
	return test_case_2_3_1x(ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define desc_case_2_3_1d "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- outgoing call\n\
-- \"no indication\" and \"terminating access non-isdn\"\n\
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = bci | ISUP_BCI_ORDINARY_SUBSCRIBER;
	cprim.opt.len = 0;
	if (bci & ISUP_BCI_SUBSCRIBER_FREE)
		iut_cpc_signal(ALERTING_REQ);
	else
		iut_cpc_signal(PROCEEDING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_1e "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- incoming call\n\
-- \"subscriber free\" and \"terminating access isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1e(void)
{
	return test_case_2_3_1y(ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define desc_case_2_3_1f "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- incoming call\n\
-- \"subscriber free\" and \"terminating access non-isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1f(void)
{
	return test_case_2_3_1y(ISUP_BCI_SUBSCRIBER_FREE);
}

#define desc_case_2_3_1g "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- incoming call\n\
-- \"no indication\" and \"terminating access isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1g(void)
{
	return test_case_2_3_1y(ISUP_BCI_TERMINATING_ACCESS_ISDN);
}

#define desc_case_2_3_1h "\
Successful Call setup\n\
-- Ordinary call (with various indications in the ACM)\n\
-- incoming call\n\
-- \"no indication\" and \"terminating access non-isdn\"\n\
Verify that a call can be successfully completed using various indications in\n\
address complete messages."
static int
test_case_2_3_1h(void)
{
	return test_case_2_3_1y(0);
}

#define desc_case_2_3_2x "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- outgoing call\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2x(ulong evnt)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_ORDINARY_SUBSCRIBER | ((evnt != ISUP_EVNT_PROGRESS) ? 0 :
					    ISUP_BCI_SUBSCRIBER_FREE);
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
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
			return FAILURE;
		}
		break;
	}
	state = 7;
	send(ANM);
	state = 8;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 9;
	iut_cpc_signal(COMMUNICATION);
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 11;
	if (pt_event() != REL)
		return FAILURE;
	state = 12;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 13;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_2a "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- outgoing call\n\
-- \"alerting\"\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2a(void)
{
	return test_case_2_3_2x(ISUP_EVNT_ALERTING);
}

#define desc_case_2_3_2b "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- outgoing call\n\
-- \"progress\"\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2b(void)
{
	return test_case_2_3_2x(ISUP_EVNT_PROGRESS);
}

#define desc_case_2_3_2c "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- outgoing call\n\
-- \"in-band information or appropriate parttern available\"\n\
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.opt.len = 0;
	switch ((cprim.event = evnt)) {
	case ISUP_EVNT_ALERTING:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER;
		iut_cpc_signal(PROCEEDING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return FAILURE;
		state = 6;
		iut_cpc_signal(ALERTING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 7;
		if (pt_event() != CPG)
			return FAILURE;
		state = 8;
		iut_cpc_signal(RINGING);
		break;
	case ISUP_EVNT_PROGRESS:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
		iut_cpc_signal(ALERTING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return FAILURE;
		state = 6;
		iut_cpc_signal(RINGING);
		state = 7;
		iut_cpc_signal(PROGRESS_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 8;
		if (pt_event() != CPG)
			return FAILURE;
		break;
	case ISUP_EVNT_IBI:
		cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER;
		iut_cpc_signal(PROCEEDING_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 5;
		if (pt_event() != ACM)
			return FAILURE;
		state = 6;
		iut_cpc_signal(IBI_REQ);
		if (cpc_event() != CPC_OK_ACK)
			return FAILURE;
		state = 7;
		if (pt_event() != CPG)
			return FAILURE;
		state = 8;
		iut_cpc_signal(RINGING);
		break;
	default:
		return SCRIPTERROR;
	}
	state = 9;
	iut_cpc_signal(CONNECT_REQ);
	state = 10;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 11;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 15;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_2d "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- incoming call\n\
-- \"alerting\"\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2d(void)
{
	return test_case_2_3_2y(ISUP_EVNT_ALERTING);
}

#define desc_case_2_3_2e "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- incoming call\n\
-- \"progress\"\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2e(void)
{
	return test_case_2_3_2y(ISUP_EVNT_PROGRESS);
}

#define desc_case_2_3_2f "\
Successful Call setup\n\
-- Ordinary call (with ACM, CPG, and ANM)\n\
-- incoming call\n\
-- \"in-band information or appropriate parttern available\"\n\
Verify that a call be successfully completed using address complete messages,\n\
call progress message and answer message."
static int
test_case_2_3_2f(void)
{
	return test_case_2_3_2y(ISUP_EVNT_IBI);
}

#define desc_case_2_3_3a "\
Successful Call setup\n\
-- Ordinary call with CON\n\
-- outgoing call\n\
Verify that a call can be successfully completed with a connect message."
static int
test_case_2_3_3a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(CON);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 5;
	iut_cpc_signal(COMMUNICATION);
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_3b "\
Successful Call setup\n\
-- Ordinary call with CON\n\
-- incoming call\n\
Verify that a call can be successfully completed with a connect message."
static int
test_case_2_3_3b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.opt.len = 0;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_TERMINATING_ACCESS_ISDN;
	iut_cpc_signal(CONNECT_REQ);
	state = 5;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 6;
	if (pt_event() != CON)
		return FAILURE;
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
		return FAILURE;
	state = 10;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 11;
	if (pt_event() != RLC)
		return FAILURE;
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_4a "\
Successful Call setup\n\
-- Call switched via a satellite\n\
-- outgoing call\n\
Verify the satellite indicator in the initial address message is correctly\n\
set."
static int
test_case_2_3_4a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_NCI_ONE_SATELLITE_CCT | ISUP_FCI_INTERNATIONAL_CALL |
	    ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY | ISUP_FCI_ORIGINATING_ACCESS_ISDN |
	    ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_4b "\
Successful Call setup\n\
-- Call switched via a satellite\n\
-- incoming call\n\
Verify the satellite indicator in the initial address message is correctly\n\
set."
static int
test_case_2_3_4b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_ONE_SATELLITE_CCT;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_5a "\
Successful Call setup\n\
-- Blocking and unblocking during a call (initiated)\n\
-- outgoing call\n\
Verify that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_5a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	send(BLA);
	state = 12;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 17;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 18;
	if (pt_event() != UBL)
		return FAILURE;
	state = 19;
	pmsg.cic = imsg.cic;
	send(UBA);
	state = 20;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return FAILURE;
	state = 21;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_5b "\
Successful Call setup\n\
-- Blocking and unblocking during a call (initiated)\n\
-- incoming call\n\
Verify that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_5b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	send(BLO);
	state = 12;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 13;
	iut_mgm_signal(BLOCKING_RES);
	state = 14;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 15;
	if (pt_event() != BLA)
		return FAILURE;
	state = 16;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
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
		return FAILURE;
	state = 20;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 21;
	if (pt_event() != RLC)
		return FAILURE;
	state = 22;
	send(UBL);
	state = 23;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 24;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 25;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 26;
	if (pt_event() != UBA)
		return FAILURE;
	state = 27;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_6a "\
Successful Call setup\n\
-- Blocking and unblocking during a call (received)\n\
-- outgoing call\n\
Verfiy that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_6a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.cic = 12;
	send(BLO);
	state = 9;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 10;
	iut_mgm_signal(BLOCKING_RES);
	state = 11;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 12;
	if (pt_event() != BLA)
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 17;
	pmsg.cic = 12;
	send(UBL);
	state = 18;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 19;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 20;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 21;
	if (pt_event() != UBA)
		return FAILURE;
	state = 22;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_3_6b "\
Successful Call setup\n\
-- Blocking and unblocking during a call (received)\n\
-- incoming call\n\
Verfiy that the circuit blocking (during a call) and unblocking (after\n\
clearing the call) can be correctly performed."
static int
test_case_2_3_6b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 13;
	send(BLA);
	state = 14;
	if (mgm_event() != MGM_BLOCKING_CON)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
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
		return FAILURE;
	state = 19;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 20;
	if (pt_event() != RLC)
		return FAILURE;
	state = 21;
	mprim.addr.add.scope = ISUP_SCOPE_CT;
	mprim.addr.add.id = 12;
	mprim.addr.add.cic = 12;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.flags = ISUP_MAINTENANCE_ORIENTED;
	iut_mgm_signal(UNBLOCKING_REQ);
	state = 22;
	if (pt_event() != UBL)
		return FAILURE;
	state = 23;
	send(UBA);
	state = 24;
	if (mgm_event() != MGM_UNBLOCKING_CON)
		return FAILURE;
	state = 25;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_2_4_1 "\
Propagation delay determination procedure\n\
-- IAM sent containing the PDC\n\
Verify that SP A is able to increase the PDC by the delay value of the\n\
outgoing route (D ms)."
static int
test_case_2_4_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_2_4_2 "\
Propagation delay determination procedure\n\
-- SP supporting the procedure to SP supporting the procedure\n\
Verify that a call can be successfully completed and the value of the call\n\
history is higher as the value of the PDC."
static int
test_case_2_4_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_2_4_3 "\
Propagation delay determination procedure\n\
-- Abnormal procedure, PDC is not recevied\n\
Verify that a call can be successfully completed and the PDC is generated in\n\
SP A."
static int
test_case_2_4_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_2_4_4 "\
Propagation delay determination procedure\n\
-- ISUP'92 supporting the procedure to Q.767\n\
Verify that a call can be successfully completed and the PDC is discarded."
static int
test_case_2_4_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_2_4_5 "\
Propagation delay determination procedure\n\
-- Q.767 to ISUP'92 supporting the procedure\n\
Verify that a call can be successfully completed and CHI is discarded if\n\
received."
static int
test_case_2_4_5(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_3_1a "\
Normal call release\n\
-- Calling party clears before address complete\n\
-- outgoing call\n\
Verify that the calling party can successfully release a call prior to receipt\n\
of any backward message."
static int
test_case_3_1a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 3;
	if (pt_event() != REL)
		return FAILURE;
	state = 4;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 5;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 6;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	state = 7;
	return SUCCESS;
}

#define desc_case_3_1b "\
Normal call release\n\
-- Calling party clears before address complete\n\
-- incoming call\n\
Verify that the calling party can successfully release a call prior to receipt\n\
of any backward message."
static int
test_case_3_1b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 5;
	if (cpc_event() != CPC_RELEASE_IND)
		return FAILURE;
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 7;
	if (pt_event() != RLC)
		return FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_2a "\
Normal call release\n\
-- Calling party clears before answer\n\
-- outgoing call\n\
Verify that the calling party can successfully release a call prior to receipt\n\
of answer."
static int
test_case_3_2a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 7;
	if (pt_event() != REL)
		return FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_2b "\
Normal call release\n\
-- Calling party clears before answer\n\
-- incoming call\n\
Verify that the calling party can successfully release a call prior to receipt\n\
of answer."
static int
test_case_3_2b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
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
		return FAILURE;
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 10;
	if (pt_event() != RLC)
		return FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_3a "\
Normal call release\n\
-- Calling party clears after answer\n\
-- outgoing call\n\
Verify that the calling party can successfully release a call after answer."
static int
test_case_3_3a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	iut_cpc_signal(COMMUNICATION);
	state = 8;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 9;
	if (pt_event() != REL)
		return FAILURE;
	state = 10;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 11;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 12;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_3b "\
Normal call release\n\
-- Calling party clears after answer\n\
-- incoming call\n\
Verify that the calling party can successfully release a call after answer."
static int
test_case_3_3b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
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
		return FAILURE;
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 14;
	if (pt_event() != RLC)
		return FAILURE;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_4a "\
Normal call release\n\
-- Called party clears after answer\n\
-- outgoing call\n\
Verify that a call can be successfully released in the backward direction."
static int
test_case_3_4a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 12;
	if (pt_event() != RLC)
		return FAILURE;
	state = 13;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_4b "\
Normal call release\n\
-- Called party clears after answer\n\
-- incoming call\n\
Verify that a call can be successfully released in the backward direction."
static int
test_case_3_4b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 12;
	if (pt_event() != REL)
		return FAILURE;
	state = 13;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(RLC);
	state = 14;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_5a "\
Normal call release\n\
-- Suspend initiated by the network\n\
-- outgoing call\n\
Verify that the called subscriber can suscessfully clear back and reanswer a\n\
call."
static int
test_case_3_5a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.sris = ISUP_SRIS_NETWORK_INITIATED;
	send(SUS);
	state = 9;
	if (cpc_event() != CPC_SUSPEND_IND)
		return FAILURE;
	state = 10;
	send(RES);
	state = 11;
	if (cpc_event() != CPC_RESUME_IND)
		return FAILURE;
	state = 12;
	iut_cpc_signal(COMMUNICATION);
	state = 13;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 14;
	if (pt_event() != REL)
		return FAILURE;
	state = 15;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 16;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_5b "\
Normal call release\n\
-- Suspend initiated by the network\n\
-- incoming call\n\
Verify that the called subscriber can suscessfully clear back and reanswer a\n\
call."
static int
test_case_3_5b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.flags = ISUP_SRIS_NETWORK_INITIATED;
	iut_cpc_signal(SUSPEND_REQ);
	state = 12;
	if (pt_event() != SUS)
		return (FAILURE);
	state = 13;
	iut_cpc_signal(RESUME_REQ);
	state = 14;
	if (pt_event() != RES)
		return (FAILURE);
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
		return FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_6a "\
Normal call release\n\
-- Suspend and resume initiated by a calling party\n\
-- outgoing call\n\
Verify that the calling subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_6a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.flags = ISUP_SRIS_USER_INITIATED;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(SUSPEND_REQ);
	state = 10;
	if (pt_event() != SUS)
		return FAILURE;
	state = 11;
	iut_cpc_signal(RESUME_REQ);
	state = 12;
	if (pt_event() != RES)
		return FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_6b "\
Normal call release\n\
-- Suspend and resume initiated by a calling party\n\
-- incoming call\n\
Verify that the calling subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_6b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	pmsg.opt.len = 0;
	send(SUS);
	state = 12;
	if (cpc_event() != CPC_SUSPEND_IND)
		return FAILURE;
	state = 13;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	pmsg.opt.len = 0;
	send(RES);
	state = 14;
	if (cpc_event() != CPC_RESUME_IND)
		return FAILURE;
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
		return FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_7a "\
Normal call release\n\
-- Suspend and resume initiated by a called party\n\
-- outgoing call\n\
Verify that the called subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_7a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	pmsg.sris = ISUP_SRIS_USER_INITIATED;
	send(SUS);
	state = 10;
	if (cpc_event() != CPC_SUSPEND_IND)
		return FAILURE;
	state = 11;
	send(RES);
	state = 12;
	if (cpc_event() != CPC_RESUME_IND)
		return FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_7b "\
Normal call release\n\
-- Suspend and resume initiated by a called party\n\
-- incoming call\n\
Verify that the called subscriber can suscessfully suspend and resume a\n\
call."
static int
test_case_3_7b(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 1;
	if (cpc_event() != CPC_SETUP_IND)
		return FAILURE;
	state = 2;
	iut_cpc_signal(SETUP_RES);
	state = 3;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 4;
	cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	cprim.opt.len = 0;
	iut_cpc_signal(ALERTING_REQ);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != ACM)
		return FAILURE;
	state = 6;
	iut_cpc_signal(RINGING);
	state = 7;
	iut_cpc_signal(CONNECT_REQ);
	state = 8;
	if (cpc_event() != CPC_SETUP_COMPLETE_IND)
		return FAILURE;
	state = 9;
	if (pt_event() != ANM)
		return FAILURE;
	state = 10;
	iut_cpc_signal(COMMUNICATION);
	state = 11;
	cprim.flags = ISUP_SRIS_USER_INITIATED;
	iut_cpc_signal(SUSPEND_REQ);
	state = 12;
	if (pt_event() != SUS)
		return (FAILURE);
	state = 13;
	iut_cpc_signal(RESUME_REQ);
	state = 14;
	if (pt_event() != RES)
		return (FAILURE);
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
		return FAILURE;
	state = 18;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 19;
	if (pt_event() != RLC)
		return FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_8a "\
Normal call release\n\
-- Collision of REL messages\n\
-- REL sent first\n\
Verify that a release message may be received at an exchange from a succeeding\n\
or preceding exchange after the release of the switch path is initiated."
static int
test_case_3_8a(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	state = 9;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 10;
	if (pt_event() != REL)
		return FAILURE;
	state = 11;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_CALL_CLEARING;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 12;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 13;
	send(RLC);
	state = 14;
	if (pt_event() != RLC)
		return FAILURE;
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_3_8b "\
Normal call release\n\
-- Collision of REL messages\n\
-- REL received first\n\
Verify that a release message may be received at an exchange from a succeeding\n\
or preceding exchange after the release of the switch path is initiated."
static int
test_case_3_8b(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 11;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	state = 12;
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 13;
	if (pt_event() != RLC)
		return FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_4_1x "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case A\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1x(ulong cause)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = cause;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 3;
	if (cpc_event() != CPC_RELEASE_IND)
		return FAILURE;
	state = 4;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 5;
	if (pt_event() != RLC)
		return FAILURE;
	state = 6;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	state = 7;
	return SUCCESS;
}

#define desc_case_4_1y "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case B\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1y(ulong cause)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_PROCEEDING_IND)
		return FAILURE;
	state = 5;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = cause;
	pmsg.caus.len = 2;
	pmsg.opt.len = 0;
	send(REL);
	state = 6;
	if (cpc_event() != CPC_RELEASE_IND)
		return FAILURE;
	state = 7;
	cprim.cause = 0;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	cprim.user_ref = 0;
	iut_cpc_signal(RELEASE_RES);
	if (cpc_event() != CPC_OK_ACK)
		return FAILURE;
	state = 8;
	if (pt_event() != RLC)
		return FAILURE;
	state = 9;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_4_1a "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case A\n\
-- \"unallocated number\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1a(void)
{
	return test_case_4_1x(CC_CAUS_UNALLOCATED_NUMBER);
}

#define desc_case_4_1b "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case A\n\
-- \"no circuit available\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1b(void)
{
	return test_case_4_1x(CC_CAUS_NO_CCT_AVAILABLE);
}

#define desc_case_4_1c "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case A\n\
-- \"switching equipment congestion\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1c(void)
{
	return test_case_4_1x(CC_CAUS_SWITCHING_EQUIP_CONGESTION);
}

#define desc_case_4_1d "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case B\n\
-- \"unallocated number\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1d(void)
{
	return test_case_4_1y(CC_CAUS_UNALLOCATED_NUMBER);
}

#define desc_case_4_1e "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case B\n\
-- \"no circuit available\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1e(void)
{
	return test_case_4_1y(CC_CAUS_NO_CCT_AVAILABLE);
}

#define desc_case_4_1f "\
Unsuccessful call setup\n\
-- Validate a set of known causes for release\n\
-- Case B\n\
-- \"switching equipment congestion\"\n\
Verify that the call will be immediately released by the outgoing signalling\n\
point if a release message with a given cause is received and the correct\n\
indication is given to the calling party."
static int
test_case_4_1f(void)
{
	return test_case_4_1y(CC_CAUS_SWITCHING_EQUIP_CONGESTION);
}

#define desc_case_5_1 "\
Abnormal situation during a call\n\
-- Inability to release in response to a REL after ANM\n\
Verify that if the SP is unable to return a circuit to the idle condition in\n\
response to a release message, the circuit will be blocked."
static int
test_case_5_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_5_2_1 "\
Timers\n\
-- T7: waiting for ACM or CON\n\
Check that at the expiry of T7 the circuit is released."
static int
test_case_5_2_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	start_st(timer[t7].hi);
	beg_time = milliseconds("  T7  ");
	state = 3;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 4;
	if (pt_event() != REL)
		return FAILURE;
	state = 5;
	if (check_time("  T7  ", now() - beg_time, timer[t7].lo, timer[t7].hi) != SUCCESS)
		goto failure;
	state = 6;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 7;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	state = 8;
	return SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return FAILURE;
}

#define desc_case_5_2_2 "\
Timers\n\
-- T9: waiting for ANM\n\
Verify that if an answer message is not received within T9 after receiving an\n\
address complete message, the connection is released by the outgoing\n\
signalling point."
static int
test_case_5_2_2(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	start_st(timer[t9].hi);
	beg_time = milliseconds("  T9  ");
	state = 7;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 8;
	if (pt_event() != REL)
		return FAILURE;
	state = 9;
	if (check_time("  T9  ", now() - beg_time, timer[t9].lo, timer[t9].hi) != SUCCESS)
		goto failure;
	state = 10;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 11;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return FAILURE;
}

#define desc_case_5_2_3 "\
Timers\n\
-- T1 and T5: failure to receive a RLC\n\
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
			pmsg.fci =
			    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
			     ISUP_FCI_ORIGINATING_ACCESS_ISDN) >> 8;
			pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
			pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
			strncpy(pmsg.cdpn.num, "17805551212/", 24);
			pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
			send(IAM);
			state = 1;
		case 1:
			if (cpc_event() != CPC_SETUP_IND)
				return FAILURE;
			state = 2;
		case 2:
			iut_cpc_signal(SETUP_RES);
			state = 3;
		case 3:
			if (cpc_event() != CPC_OK_ACK)
				return FAILURE;
			state = 4;
		case 4:
			cprim.flags = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
			cprim.opt.len = 0;
			iut_cpc_signal(ALERTING_REQ);
			if (cpc_event() != CPC_OK_ACK)
				return FAILURE;
			state = 5;
		case 5:
			if (pt_event() != ACM)
				return FAILURE;
			state = 6;
		case 6:
			iut_cpc_signal(RINGING);
			state = 7;
		case 7:
			iut_cpc_signal(CONNECT_REQ);
			state = 8;
		case 8:
			if (cpc_event() != CPC_SETUP_COMPLETE_IND)
				return FAILURE;
			state = 9;
		case 9:
			if (pt_event() != ANM)
				return FAILURE;
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
				return FAILURE;
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
				if (check_time
				    ("  T1  ", rel_time - t1_time, timer[t1].lo,
				     timer[t1].hi) != SUCCESS)
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
			if (check_time("  T5  ", now() - t5_time, timer[t5].lo, timer[t5].hi) !=
			    SUCCESS)
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
				return FAILURE;
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.caus.buf[0] = 0x80;
	pmsg.caus.buf[1] = CC_CAUS_NORMAL_UNSPECIFIED;
	pmsg.caus.len = 2;
	send(RLC);
	return FAILURE;
}

#define desc_case_5_2_4 "\
Timers\n\
-- T6 :waiting for RES (Network) message\n\
Verify that the call is released at the expiry of timer T6."
static int
test_case_5_2_4(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 7;
	iut_cpc_signal(COMMUNICATION);
	state = 8;
	pmsg.sris = ISUP_SRIS_NETWORK_INITIATED;
	send(SUS);
	state = 9;
	if (cpc_event() != CPC_SUSPEND_IND)
		return FAILURE;
	state = 10;
	start_st(timer[t6].hi);
	beg_time = milliseconds("  T6  ");
	state = 11;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 12;
	if (pt_event() != REL)
		return FAILURE;
	state = 13;
	if (check_time("  T6  ", now() - beg_time, timer[t6].lo, timer[t6].hi) != SUCCESS)
		goto failure;
	state = 14;
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	state = 15;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return FAILURE;
}

#define desc_case_5_2_5a "\
Timers\n\
-- T8: waiting for COT message if applicable\n\
-- is required\n\
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
	if (check_time("  T8  ", now() - beg_time, timer[t8].lo, timer[t8].hi) != SUCCESS)
		goto failure;
	state = 9;
	send(RLC);
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	send(RSC);
	return FAILURE;
}

#define desc_case_5_2_5b "\
Timers\n\
-- T8: waiting for COT message if applicable\n\
-- is performed on a previous circuit\n\
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
	if (check_time("  T8  ", now() - beg_time, timer[t8].lo, timer[t8].hi) != SUCCESS)
		goto failure;
	state = 6;
	send(RLC);
	state = 7;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	send(RSC);
	return FAILURE;
}

#define desc_case_5_2_6 "\
Timers\n\
-- T12 and T13: failure to receive a BLA\n\
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
				return FAILURE;
			t13_time = milliseconds(" T13  ");
			t12_time = milliseconds_2nd(" T12  ");
			start_st(timer[t12].hi);
			state = 1;
			break;
		case 1:
			switch (any_event()) {
			case BLO:
				blo_time = now();
				if (check_time
				    (" T12  ", blo_time - t12_time, timer[t12].lo,
				     timer[t12].hi) != SUCCESS) {
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
			if (check_time(" T13  ", now() - t13_time, timer[t13].lo, timer[t13].hi) !=
			    SUCCESS)
				goto failure;
			t13_time = milliseconds(" T13  ");
			start_st(timer[t13].hi);
			state = 4;
			break;
		case 4:
			switch (any_event()) {
			case BLO:
				blo_time = now();
				if (check_time
				    (" T13  ", now() - t13_time, timer[t13].lo,
				     timer[t13].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(BLA);
	iut_mgm_signal(UNBLOCKING_REQ);
	send(UBA);
	return FAILURE;
}

#define desc_case_5_2_7 "\
Timers\n\
-- T14 and T15: failure to receive UBA\n\
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
				return FAILURE;
			state = 1;
			break;
		case 1:
			pmsg.cic = imsg.cic;
			send(BLA);
			if (any_event() != MGM_BLOCKING_CON)
				return FAILURE;
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
				return FAILURE;
			t15_time = milliseconds(" T15  ");
			t14_time = milliseconds_2nd(" T14  ");
			start_st(timer[t14].hi);
			state = 3;
			break;
		case 3:
			switch (any_event()) {
			case UBL:
				ubl_time = now();
				if (check_time
				    (" T14  ", ubl_time - t14_time, timer[t14].lo,
				     timer[t14].hi) != SUCCESS) {
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
			    != SUCCESS)
				goto failure;
			t15_time = milliseconds(" T15  ");
			start_st(timer[t15].hi);
			state = 6;
			break;
		case 6:
			switch (any_event()) {
			case UBL:
				ubl_time = now();
				if (check_time
				    (" T15  ", ubl_time - t15_time, timer[t15].lo,
				     timer[t15].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(UBA);
	return FAILURE;
}

#define desc_case_5_2_8 "\
Timers\n\
-- T16 and T17: failure to receive a RLC\n\
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
				return FAILURE;
			rsc_time = now();
			t17_time = milliseconds(" T17  ");
			t16_time = milliseconds_2nd(" T16  ");
			start_st(timer[t16].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case RSC:
				rsc_time = now();
				if (check_time
				    (" T16  ", rsc_time - t16_time, timer[t16].lo,
				     timer[t16].hi) != SUCCESS) {
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
			    != SUCCESS)
				goto failure;
			t17_time = milliseconds(" T17  ");
			start_st(timer[t17].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case RSC:
				rsc_time = now();
				if (check_time
				    (" T17  ", rsc_time - t17_time, timer[t17].lo,
				     timer[t17].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
      failure:
	pmsg.cic = imsg.cic;
	pmsg.opt.len = 0;
	send(RLC);
	return FAILURE;
}

#define desc_case_5_2_9 "\
Timers\n\
-- T18 and T19: failure to receive a CGBA\n\
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
				return FAILURE;
			cgb_time = now();
			t19_time = milliseconds(" T19  ");
			t18_time = milliseconds_2nd(" T18  ");
			start_st(timer[t18].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case CGB:
				cgb_time = now();
				if (check_time
				    (" T18  ", cgb_time - t18_time, timer[t18].lo,
				     timer[t18].hi) != SUCCESS) {
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
			    != SUCCESS)
				goto failure;
			t19_time = milliseconds(" T19  ");
			start_st(timer[t19].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case CGB:
				cgb_time = now();
				if (check_time
				    (" T19  ", cgb_time - t19_time, timer[t19].lo,
				     timer[t19].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
	return SCRIPTERROR;
      failure:
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGBA);
	iut_mgm_signal(UNBLOCKING_REQ);
	send(CGUA);
	return FAILURE;
}

#define desc_case_5_2_10 "\
Timers\n\
-- T20 and T21: failure to receive a CGUA\n\
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
				return FAILURE;
			state = 1;
		case 1:
			pmsg.cic = imsg.cic;
			bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
			pmsg.rs.len = imsg.rs.len;
			pmsg.cgi = imsg.cgi;
			send(CGBA);
			if (any_event() != MGM_BLOCKING_CON)
				return FAILURE;
			state = 2;
		case 2:
			mprim.addr.add.scope = ISUP_SCOPE_CG;
			mprim.addr.add.id = 1;
			mprim.addr.add.cic = 1;
			mprim.addr.len = sizeof(mprim.addr.add);
			mprim.flags = ISUP_GROUP | ISUP_MAINTENANCE_ORIENTED;
			iut_mgm_signal(UNBLOCKING_REQ);
			if (any_event() != CGU)
				return FAILURE;
			cgu_time = now();
			t21_time = milliseconds(" T21  ");
			t20_time = milliseconds_2nd(" T20  ");
			start_st(timer[t20].hi);
			state = 3;
		case 3:
			switch (any_event()) {
			case CGU:
				cgu_time = now();
				if (check_time
				    (" T20  ", cgu_time - t20_time, timer[t20].lo,
				     timer[t20].hi) != SUCCESS) {
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
			    != SUCCESS)
				goto failure;
			t21_time = milliseconds(" T20  ");
			start_st(timer[t21].hi);
			state = 6;
		case 6:
			switch (any_event()) {
			case CGU:
				cgu_time = now();
				if (check_time
				    (" T21  ", cgu_time - t21_time, timer[t21].lo,
				     timer[t21].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
	return SCRIPTERROR;
      failure:
	pmsg.cic = imsg.cic;
	bcopy(imsg.rs.buf, pmsg.rs.buf, imsg.rs.len);
	pmsg.rs.len = imsg.rs.len;
	pmsg.cgi = imsg.cgi;
	send(CGUA);
	return FAILURE;
}

#define desc_case_5_2_11 "\
Timers\n\
-- T22 and T23: failure to receive a GRA\n\
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
				return FAILURE;
			grs_time = now();
			t23_time = milliseconds(" T23  ");
			t22_time = milliseconds_2nd(" T22  ");
			start_st(timer[t22].hi);
			state = 1;
		case 1:
			switch (any_event()) {
			case GRS:
				grs_time = now();
				if (check_time
				    (" T22  ", grs_time - t22_time, timer[t22].lo,
				     timer[t22].hi) != SUCCESS) {
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
			    != SUCCESS)
				goto failure;
			t23_time = milliseconds(" T23  ");
			start_st(timer[t23].hi);
			state = 4;
		case 4:
			switch (any_event()) {
			case GRS:
				grs_time = now();
				if (check_time
				    (" T23  ", grs_time - t23_time, timer[t23].lo,
				     timer[t23].hi) != SUCCESS)
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
			return SUCCESS;
		default:
			return SCRIPTERROR;
		}
	}
	return SCRIPTERROR;
      failure:
	pmsg.cic = imsg.cic;
	pmsg.rs.buf[0] = imsg.rs.buf[0];
	pmsg.rs.buf[1] = 0x00;
	pmsg.rs.buf[2] = 0x00;
	pmsg.rs.buf[3] = 0x00;
	pmsg.rs.buf[4] = 0x00;
	pmsg.rs.len = 5;
	send(GRA);
	return FAILURE;
}

#define desc_case_5_3_1 "\
Reset of circuits during a call\n\
-- Of an outgoing circuit\n\
Verify that on receipt of a reset message the call is immediately released --\n\
outgoing call."
static int
test_case_5_3_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 2;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 3;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 4;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 5;
	send(RINGING);
	state = 6;
	send(ANM);
	state = 7;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 8;
	iut_cpc_signal(COMMUNICATION);
	pmsg.cic = 12;
	send(RSC);
	state = 9;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 10;
	if (mgm_event() != MGM_RESET_IND)
		return FAILURE;
	state = 11;
	iut_mgm_signal(RESET_RES);
	state = 12;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 13;
	if (pt_event() != RLC)
		return FAILURE;
	state = 14;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_5_3_2 "\
Reset of circuits during a call\n\
-- Of an incoming circuit\n\
Verify that on receipt of a reset message the call is immediately released --\n\
incoming call."
static int
test_case_5_3_2(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = 0;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
		return FAILURE;
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
		return FAILURE;
	state = 13;
	iut_mgm_signal(RESET_RES);
	state = 14;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 15;
	if (cpc_event() != CPC_CALL_FAILURE_IND)
		return FAILURE;
	state = 16;
	if (pt_event() != RLC)
		return FAILURE;
	state = 17;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	send(RSC);
	return FAILURE;
}

#define desc_case_6_1_1 "\
Continuity check call\n\
-- Continuity check required\n\
Verify that a call can be set up on a circuit requiring a continuity check."
static int
test_case_6_1_1(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return FAILURE;
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
		return FAILURE;
	state = 7;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 8;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 9;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 10;
	send(RINGING);
	state = 11;
	send(ANM);
	state = 12;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_1_2 "\
Continuity check call\n\
-- COT applied on a previous circuit\n\
Verify that if a continuity check is being peformed on a previous circuit, a\n\
backward message is delayed until receipt of the COT message."
static int
test_case_6_1_2(void)
{
	state = 0;
	pmsg.cic = 12;
	pmsg.nci = ISUP_NCI_CONT_CHECK_PREVIOUS;
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
	if (any_event() != TIMEOUT)
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
		return FAILURE;
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
		return FAILURE;
	state = 18;
	if (pt_event() != RLC)
		goto failure;
	state = 19;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	send(RSC);
	return FAILURE;
}

#define desc_case_6_1_3 "\
Continuity check call\n\
-- Calling party clears during a COT\n\
Verify that the calling party can successfully clear the call during the\n\
continuity check phase."
static int
test_case_6_1_3(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return FAILURE;
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
		return FAILURE;
	state = 7;
	if (pt_event() != REL)
		return FAILURE;
	state = 8;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 9;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 10;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_1_4 "\
Continuity check call\n\
-- Delay of through connect\n\
Verify that the switching through of the speech path is delayed until the\n\
residual check-tone has propagated through the return of the speech path."
static int
test_case_6_1_4(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return FAILURE;
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
		return FAILURE;
	state = 7;
	pmsg.cic = imsg.cic;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
	send(ACM);
	state = 8;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 9;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 10;
	send(RINGING);
	state = 11;
	send(ANM);
	state = 12;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
	state = 13;
	iut_cpc_signal(COMMUNICATION);
	state = 14;
	cprim.cause = CC_CAUS_NORMAL_CALL_CLEARING;
	cprim.opt.len = 0;
	cprim.addr.len = 0;
	iut_cpc_signal(RELEASE_REQ);
	state = 15;
	if (pt_event() != REL)
		return FAILURE;
	state = 16;
	pmsg.cic = imsg.cic;
	send(RLC);
	state = 17;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 18;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_1_5 "\
Continuity check call\n\
-- COT unsuccessful\n\
Verify that a repeat attempt of the continuity check is made on the failed\n\
circuit."
static int
test_case_6_1_5(void)
{
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return FAILURE;
	state = 3;
	send(LOOPBACK);
	state = 4;
	iut_cpc_signal(TONE);
	state = 5;
	beg_time = milliseconds(" T24  ");
	start_st(timer[t24].hi + 100);
	state = 6;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return FAILURE;
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 8;
	if (pt_event() != COT)
		return FAILURE;
	state = 9;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 10;
	beg_time = milliseconds(" T25  ");
	state = 11;
	start_st(timer[t25].hi + 100);
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 12;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 13;
	if (pt_event() != CCR)
		return FAILURE;
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
		return FAILURE;
	state = 19;
	if (pt_event() != COT)
		return FAILURE;
	state = 20;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 21;
	beg_time = milliseconds(" T26  ");
	state = 22;
	start_st(timer[t26].hi + 100);
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 23;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 24;
	if (pt_event() != CCR)
		return FAILURE;
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
		return FAILURE;
	state = 30;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_2_1 "\
Automatic repeat attempt\n\
-- Dual seizure for non-controlling SP\n\
Verify that an automatic repeat attempt will be made on detection of a dual\n\
siezure."
static int
test_case_6_2_1(void)
{
	ulong cicx = 31, cicy = 30;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
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
		return FAILURE;
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
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
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
		return FAILURE;
	state = 31;
	if (pt_event() != RLC)
		goto failure;
	state = 32;
	if (wait_event(0, ANY) != NO_MSG)
		goto failure;
	return SUCCESS;
      failure:
	pmsg.cic = cicy;
	send(RSC);
	pmsg.cic = cicx;
	send(RSC);
	return FAILURE;
}

#define desc_case_6_2_2 "\
Automatic repeat attempt\n\
-- Blocking of a circuit\n\
Verify that an automatic repeat attempt will be made on receipt of the\n\
blocking message after sending an Initial address message and before any\n\
backward messsages have been received."
static int
test_case_6_2_2(void)
{
	ulong cic = 31;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 3;
	cic = pmsg.cic = imsg.cic;
	send(BLO);
	state = 4;
	if (mgm_event() != MGM_BLOCKING_IND)
		return FAILURE;
	state = 5;
	iut_mgm_signal(BLOCKING_RES);
	state = 6;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 7;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return FAILURE;
	state = 8;
	if (pt_event() != BLA)
		return FAILURE;
	state = 9;
	if (pt_event() != REL)
		return FAILURE;
	state = 10;
	send(RLC);
	state = 11;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 13;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 14;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 15;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 16;
	send(RINGING);
	state = 17;
	send(ANM);
	state = 18;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 22;
	send(RLC);
	state = 23;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 24;
	pmsg.cic = cic;
	send(UBL);
	state = 25;
	if (mgm_event() != MGM_UNBLOCKING_IND)
		return FAILURE;
	state = 26;
	iut_mgm_signal(UNBLOCKING_RES);
	state = 27;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 28;
	if (pt_event() != UBA)
		return FAILURE;
	state = 29;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_2_3 "\
Automatic repeat attempt\n\
-- Circuit reset\n\
Verify that an automatic repeat attempt will be made on receipt of circuit\n\
reset after sending of an Initial address message and before a backward\n\
message has been received."
static int
test_case_6_2_3(void)
{
	ulong cic = 31;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 3;
	cic = pmsg.cic = imsg.cic;
	send(RSC);
	state = 4;
	if (mgm_event() != MGM_RESET_IND)
		return FAILURE;
	state = 5;
	iut_mgm_signal(RESET_RES);
	state = 6;
	if (mgm_event() != MGM_OK_ACK)
		return FAILURE;
	state = 7;
	if (cpc_event() != CPC_CALL_REATTEMPT_IND)
		return FAILURE;
	state = 8;
	if (pt_event() != RLC)
		return FAILURE;
	state = 9;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 11;
	pmsg.cic = imsg.cic;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 12;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 13;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 14;
	send(RINGING);
	state = 15;
	send(ANM);
	state = 16;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 20;
	send(RLC);
	state = 21;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 22;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_2_4 "\
Automatic repeat attempt\n\
-- Continuity check failure\n\
Verify that an automatic repeat attempt will be made on continuity check\n\
failure."
static int
test_case_6_2_4(void)
{
	ulong cicx, cicy;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 2;
	if (pt_event() != IAM)
		return FAILURE;
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
		return FAILURE;
	state = 7;
	if (tst_event() != MGM_CONT_REPORT_IND)
		return FAILURE;
	state = 8;
	if (pt_event() != COT)
		return FAILURE;
	state = 9;
	check_time(" T24  ", now() - beg_time, timer[t24].lo, timer[t24].hi);
	state = 10;
	beg_time = milliseconds(" T25  ");
	state = 11;
	start_st(timer[t25].hi + 100);
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY |
	    ISUP_NCI_CONT_CHECK_REQUIRED;
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
		return FAILURE;
	state = 13;
	send(LOOPBACK);
	state = 14;
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 15;
	iut_tst_signal(TONE);
	state = 16;
	cicy = imsg.cic;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 17;
	if (pt_event() != COT)
		return FAILURE;
	state = 18;
	pmsg.cic = cicy;
	pmsg.bci = ISUP_BCI_ORDINARY_SUBSCRIBER | ISUP_BCI_SUBSCRIBER_FREE;
	send(ACM);
	state = 19;
	if (cpc_event() != CPC_SETUP_CON)
		return FAILURE;
	cprim.user_ref = 0;
	state = 20;
	if (cpc_event() != CPC_ALERTING_IND)
		return FAILURE;
	state = 21;
	send(RINGING);
	state = 22;
	send(ANM);
	state = 23;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 27;
	send(RLC);
	state = 28;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 29;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 30;
	if (tst_event() != MGM_CONT_TEST_IND)
		return FAILURE;
	state = 31;
	iut_cpc_signal(TONE);
	state = 32;
	if (pt_event() != CCR)
		return FAILURE;
	state = 33;
	send(LOOPBACK);
	state = 34;
	mprim.result = ISUP_COT_SUCCESS;
	mprim.user_ref = 0;
	iut_tst_signal(CONT_REPORT_REQ);
	state = 35;
	if (pt_event() != COT)
		return FAILURE;
	state = 36;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_6_2_5 "\
Automatic repeat attempt\n\
-- Receipt of unreasonable signalling information\n\
Verify that an automatic repeat attempt will be made on receipt of\n\
unreasonable signalling information after sending the Intitial address message\n\
and before one of the backward signals has been received."
static int
test_case_6_2_5(void)
{
	ulong cicx = 31, cicy = 30;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
		return FAILURE;
	state = 12;
	send(RINGING);
	state = 13;
	send(ANM);
	state = 14;
	if (cpc_event() != CPC_CONNECT_IND)
		return FAILURE;
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
		return FAILURE;
	state = 18;
	send(RLC);
	state = 19;
	if (cpc_event() != CPC_RELEASE_CON)
		return FAILURE;
	state = 20;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	pmsg.cic = cicy;
	send(RSC);
	pmsg.cic = cicx;
	send(RSC);
	return FAILURE;
}

static int iut_option(void);

#define desc_case_6_3_1 "\
Dual seizure\n\
-- Dual seizure for controlling SP\n\
Verify that on detection of dual siezure, the call initiated by the\n\
controlling signalling point is completed and the non-controlling signalling\n\
point is backed off."
static int
test_case_6_3_1(void)
{
	ulong cicx = 31;
	iut_tg_opt.flags |= ISUP_TGF_GLARE_PRIORITY;
	if (iut_option() != SUCCESS)
		return INCONCLUSIVE;
	state = 0;
	cprim.call_type = ISUP_CALL_TYPE_SPEECH;
	cprim.call_flags =
	    ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	    ISUP_FCI_ORIGINATING_ACCESS_ISDN | ISUP_CPC_SUBSCRIBER_ORDINARY;
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
	pmsg.fci =
	    (ISUP_FCI_INTERNATIONAL_CALL | ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY |
	     ISUP_FCI_ORIGINATING_ACCESS_ISDN)
	    >> 8;
	pmsg.cpc = (ISUP_CPC_SUBSCRIBER_ORDINARY) >> 24;
	pmsg.tmr = ISUP_CALL_TYPE_SPEECH;
	strncpy(pmsg.cdpn.num, "17805551212/", 24);
	pmsg.cdpn.len = strnlen(pmsg.cdpn.num, 24);
	send(IAM);
	state = 3;
	pmsg.bci =
	    ISUP_BCI_SUBSCRIBER_FREE | ISUP_BCI_ORDINARY_SUBSCRIBER |
	    ISUP_BCI_TERMINATING_ACCESS_ISDN;
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
	return SUCCESS;
      failure:
	pmsg.cic = cicx;
	send(RSC);
	return FAILURE;
}

#define desc_case_6_4_1 "\
Semi-automatic operation\n\
-- FOT sent following a call to a subscriber\n\
Verify that the FOT is correctly sent."
static int
test_case_6_4_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_4_2 "\
Semi-automatic operation\n\
-- FOT received following a call to a subscriber\n\
Verify that the FOT is correctly received."
static int
test_case_6_4_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_4_3 "\
Semi-automatic operation\n\
-- FOT sent following a call via codes 11 and 12\n\
Verify that the FOT is correctly sent."
static int
test_case_6_4_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_4_4 "\
Semi-automatic operation\n\
-- FOT received following a call via codes 11 and 12\n\
Verify that the FOT is correctly received."
static int
test_case_6_4_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_5_1 "\
Simple segmentation\n\
-- Sending of SGM\n\
Verify that a call can be successfully completed if segmentation is applied."
static int
test_case_6_5_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_5_2 "\
Simple segmentation\n\
-- Receipt of SGM\n\
Verify that a call can be successfully completed if segmentation is applied."
static int
test_case_6_5_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_5_3 "\
Simple segmentation\n\
-- Receipt of SGM after timer T34 expired\n\
Verify that a call can be successfully completed and the SGM will be\n\
discarded."
static int
test_case_6_5_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_5_4 "\
Simple segmentation\n\
-- Receipt of SGM in forward direction\n\
Verify that SP A is able to discard a SGM message without disrupting normal\n\
call handling."
static int
test_case_6_5_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_5_5 "\
Simple segmentation\n\
-- Receipt of SGM in backward direction\n\
Verify that SP A is able to discard a SGM message without disrupting normal\n\
call handling."
static int
test_case_6_5_5(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_6_1 "\
Fallback\n\
-- Fallback does not occur\n\
Verify that a call can be successfully completed."
static int
test_case_6_6_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_6_2 "\
Fallback\n\
-- Fallback occurs behind SP A\n\
Verify that a call can be successfully completed using Fallback that was\n\
indicated behind SP A."
static int
test_case_6_6_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_6_3 "\
Fallback\n\
-- Fallback occurs in SP A\n\
Verify that SP A is able to perform fallback."
static int
test_case_6_6_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_6_6_4 "\
Fallback\n\
-- Abnormal procedure, Fallback connection types sent to an exchange not\n\
   supporting the fallback procedure\n\
Verify that SP A is able to release the call."
static int
test_case_6_6_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_1_1 "\
64 kbit/s unrestricted\n\
-- Successful call setup\n\
Verify that a 64 kbit/s call can be successfully completed using appropriate\n\
transmission medium requirement and user service information parameters."
static int
test_case_7_1_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_1_2 "\
64 kbit/s unrestricted\n\
-- Unsuccessful call setup\n\
Verify that the call will be immediately released by the outgoing SP if a\n\
release message with a given cause is received and, for circuits equipped with\n\
echo control, the echo control device is enabled."
static int
test_case_7_1_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_1_3 "\
64 kbit/s unrestricted\n\
-- Dual siezure\n\
Verify that an automatic repeat attempt will be made on detection of dual\n\
siezure."
static int
test_case_7_1_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_2_1 "\
3.1 kHz audio\n\
-- Successful call setup\n\
Verify that a 3.1 kHz audio call can be successfully completed using\n\
appropriate transmission medium requirement and user service information\n\
parameters."
static int
test_case_7_2_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_1 "\
Multirate connection types\n\
-- Successful multirate outgoing call setup\n\
Verify that SP A is able to set up an outgoing call with a multirate bearer\n\
service."
static int
test_case_7_3_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_2 "\
Multirate connection types\n\
-- Successful multirate incoming call setup\n\
Verify that SP A is able to handle an incoming call with a multirate bearer\n\
service."
static int
test_case_7_3_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_3 "\
Multirate connection types\n\
-- Unsuccessful multirate incoming call setup -- one circuit already busy\n\
Verify that a 1920 kbit/s multirate call is rejected by SP A is one of the\n\
circuits necessary for the call is already busy."
static int
test_case_7_3_3(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_4 "\
Multirate connection types\n\
-- Dual seizure of different connection types: Controlling exchange\n\
Verify that SP A is able to detect dual seizure for calls for different\n\
multirate connection types and it completes the call involving the greater\n\
number of circuits."
static int
test_case_7_3_4(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_5 "\
Multirate connection types\n\
-- Dual seizure of connection types: Non-controlling exchange\n\
Verify that SP A is able to detect dual seizure for calls of different\n\
multirate connection types and it reattempts the call involving the smaller\n\
number of circuits."
static int
test_case_7_3_5(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_7_3_6 "\
Multirate connection types\n\
-- Abnormal procedure, Multirate connection types call sent to an exchange not\n\
   supporting the procedure\n\
Verify that SP A is able to release the call."
static int
test_case_7_3_6(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_8_1_1 "\
Automatic congestion control\n\
-- Receipt of a release message containing an automatic congestion level\n\
   parameter\n\
Verify that the adjacent exchange (SP A), after receiving a release message\n\
containing an automatic congestion level parameter reduces the traffic to the\n\
overload affected exchange (SP B)."
static int
test_case_8_1_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_8_1_2 "\
Automatic congestion control\n\
-- Sending of a release message containing an automatic congestion level\n\
   parameter\n\
Verify that the SP A is able to send a release message containing an automatic\n\
congestion level parameter."
static int
test_case_8_1_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_8_2_1 "\
ISUP availability control\n\
-- Receipt of an UPT\n\
Verify that on receipt of a user part test message SP A will respond by\n\
sending a user part available message."
static int
test_case_8_2_1(void)
{
	state = 0;
	send(UPT);
	if (any_event() != UPA)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_8_2_2 "\
ISUP availability control\n\
-- Sending of a UPT\n\
Verify that SP A is able to send a user part test message."
static int
test_case_8_2_2(void)
{
	state = 0;
	send(USER_PART_UNAVAILABLE);
	state = 1;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 2;
	if (pt_event() != UPT)
		return FAILURE;
	state = 3;
	send(UPA);
	state = 4;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 5;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
}

#define desc_case_8_2_3 "\
ISUP availability control\n\
-- T4: failure to receive a response to a UPT\n\
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
		return FAILURE;
	state = 2;
	if (pt_event() != UPT)
		return FAILURE;
	state = 3;
	upt_time = now();
	t4_time = milliseconds("  T4  ");
	state = 4;
	start_st(timer[t4].hi);
	if (pt_event() != UPT)
		return FAILURE;
	state = 5;
	upt_time = now();
	if (check_time("  T4  ", upt_time - t4_time, timer[t4].lo, timer[t4].hi) != SUCCESS)
		goto failure;
	state = 6;
	send(UPA);
	state = 7;
	if (mnt_event() != MGM_MAINT_IND)
		return FAILURE;
	state = 8;
	if (wait_event(0, ANY) != NO_MSG)
		return FAILURE;
	return SUCCESS;
      failure:
	send(UPA);
	return FAILURE;
}

#define desc_case_9_1_1 "\
Echo control procedures according to Q.767\n\
-- Q.767 echo control procedure for call set up (initiated in SP A)\n\
Verify that a call can be successfully established with the inclusion of echo\n\
control devices."
static int
test_case_9_1_1(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#define desc_case_9_1_2 "\
Echo control procedures according to Q.767\n\
-- Q.767 echo control procedure for call set up (initiated in SP B)\n\
Verify that a call can be successfully established, if SP A does not include\n\
an outgoing half echo control device."
static int
test_case_9_1_2(void)
{
	state = 0;
	return INCONCLUSIVE;
}

#ifndef HZ
#define HZ 100
#endif

static int
pt_open(void)
{
	int pfd[2];
	if (verbose > 1) {
		printf
		    ("                    .  .                            X--X---pipe()           (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((pipe(pfd) < 0)) {
		printf("                                              ****ERROR: pipe failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	pt_fd = pfd[0];
	pt_lnk_fd = pfd[1];
	if (verbose > 1) {
		printf
		    ("                    .  .                            |  |<--I_SRDOPT---------(%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_SRDOPT, RMSGD) < 0) {
		printf("                                              ****ERROR: ioctl failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("                    .  .                            |  |<--I_PUSH-----------(%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_PUSH, "pipemod") < 0) {
		printf("                                              ****ERROR: push failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
pt_close(void)
{
	if (verbose > 1) {
		printf
		    ("                    .  .                            |  |<--I_POP------------(%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_POP, 0) < 0) {
		printf("                                              ****ERROR: pop failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("                    .  .                            X--X---close()          (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (close(pt_fd) < 0) {
		printf
		    ("                                              ****ERROR: close pipe[0] failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (close(pt_lnk_fd) < 0) {
		printf
		    ("                                              ****ERROR: close pipe[1] failed\n");
		printf("                                              ****ERROR: %s; %s\n",
		       __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
pt_start(void)
{
	if (pt_open() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
pt_stop(void)
{
	if (pt_close() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

/* 
 *  IUT Intitialization and Configuration
 */
static int
iut_open(void)
{
	if (verbose > 1) {
		printf
		    ("---open()-----------X  .                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((iut_cpc_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("***************ERROR: open failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---open()-tst-------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((iut_tst_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---open()-mgm-------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((iut_mgm_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---open()-mnt-------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((iut_mnt_fd = open("/dev/isup", O_NONBLOCK | O_RDWR)) < 0) {
		printf("******************ERROR: open mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---I_SRDOPT-------->|  |                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_cpc_fd, I_SRDOPT, RMSGD) < 0) {
		printf("***************ERROR: ioctl failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---I_SRDOPT-tst-----+->|                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_tst_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---I_SRDOPT-mgm-----+->|                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---I_SRDOPT-mnt-----+->|                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mnt_fd, I_SRDOPT, RMSGD) < 0) {
		printf("******************ERROR: ioctl mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
iut_close(void)
{
	if (verbose > 1) {
		printf
		    ("---close()----------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (close(iut_mnt_fd) < 0) {
		printf("******************ERROR: close mnt failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---close()----------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (close(iut_mgm_fd) < 0) {
		printf("******************ERROR: close mgm failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---close()----------+--X                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (close(iut_tst_fd) < 0) {
		printf("******************ERROR: close tst failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose > 1) {
		printf
		    ("---close()----------X  .                            |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (close(iut_cpc_fd) < 0) {
		printf("***************ERROR: close cpc failed\n");
		printf("                    : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
		printf
		    ("---ISUP_IOCSCONFIG--+->| (ISUP_ADD sp = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding sp=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSCONFIG--+->| (ISUP_ADD sr = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding sr=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSCONFIG--+->| (ISUP_ADD tg = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding tg=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSCONFIG--+->| (ISUP_ADD cg = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: adding cg=1\n");
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
			printf
			    ("---ISUP_IOCSCONFIG--+->| (ISUP_ADD ct = %2d)         |  |                    (%d)\n",
			     state, i);
			FFLUSH(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("******************ERROR: adding ct=%d\n", i);
			printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
	}
	return SUCCESS;
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
			printf
			    ("---ISUP_IOCCCONFIG--+->| (ISUP_DEL ct = %2d)         |  |                    (%d)\n",
			     i, state);
			FFLUSH(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("******************ERROR: deleting ct=%d\n", i);
			printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
	}
	conf.type = ISUP_OBJ_TYPE_CG;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf
		    ("---ISUP_IOCCCONFIG--+->| (ISUP_DEL cg = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting cg=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_TG;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf
		    ("---ISUP_IOCCCONFIG--+->| (ISUP_DEL tg = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting tg=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_SR;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf
		    ("---ISUP_IOCCCONFIG--+->| (ISUP_DEL sr = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting sr=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	conf.type = ISUP_OBJ_TYPE_SP;
	conf.id = 1;
	conf.cmd = ISUP_DEL;
	if (verbose > 2) {
		printf
		    ("---ISUP_IOCCCONFIG--+->| (ISUP_DEL sp = 1)          |  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: deleting sp=%d\n", 1);
		printf("                       : %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
		printf
		    ("---I_LINK-----------+->|<---------------------------X  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if ((pt_lnk_id = ioctl(iut_mgm_fd, I_LINK, pt_lnk_fd)) == -1) {
		printf("***************** ERROR: link failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSCONFIG--+->| ISUP_ADD (mtp = 1)            |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: config failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
		printf
		    ("---ISUP_IOCCCONFIG--+->| ISUP_DEL (mtp = 1)            |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("******************ERROR: config failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
#endif
	if (verbose > 1) {
		printf
		    ("---I_UNLINK---------+->|<---------------------------X  |                    (%d)\n",
		     state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_UNLINK, pt_lnk_id) < 0) {
		printf("******************ERROR: unlink failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
			printf
			    ("---ISUP_IOCSOPTIONS-+->| ct id = %2d                    |                    (%d)\n",
			     i, state);
			FFLUSH(stdout);
		}
		if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
			printf("***************** ERROR: set option failed\n");
			printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| cg id = %2d                    |                    (%d)\n",
		     1, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| tg id = %2d                    |                    (%d)\n",
		     1, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| sr id = %2d                    |                    (%d)\n",
		     1, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| sp id = %2d                    |                    (%d)\n",
		     1, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| mtp id = %2d                   |                    (%d)\n",
		     1, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
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
		printf
		    ("---ISUP_IOCSOPTIONS-+->| df id = %2d                    |                    (%d)\n",
		     0, state);
		FFLUSH(stdout);
	}
	if (ioctl(iut_mgm_fd, I_STR, &ioc) < 0) {
		printf("***************** ERROR: set option failed\n");
		printf("                       : %s; %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
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
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) ||
	    mprim.addr.add.scope != iut_tst_addr.scope || mprim.addr.add.id != iut_tst_addr.id ||
	    mprim.addr.add.cic != iut_tst_addr.cic)
		return FAILURE;
	mprim.addr.add.scope = iut_mgm_addr.scope;
	mprim.addr.add.id = iut_mgm_addr.id;
	mprim.addr.add.cic = iut_mgm_addr.cic;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.bind_flags = CC_TOKEN_REQUEST | CC_MANAGEMENT;
	iut_mgm_signal(BIND_REQ);
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) ||
	    mprim.addr.add.scope != iut_mgm_addr.scope || mprim.addr.add.id != iut_mgm_addr.id ||
	    mprim.addr.add.cic != iut_mgm_addr.cic)
		return FAILURE;
	mprim.addr.add.scope = iut_mnt_addr.scope;
	mprim.addr.add.id = iut_mnt_addr.id;
	mprim.addr.add.cic = iut_mnt_addr.cic;
	mprim.addr.len = sizeof(mprim.addr.add);
	mprim.bind_flags = CC_TOKEN_REQUEST | CC_MAINTENANCE;
	iut_mnt_signal(BIND_REQ);
	if (any_event() != MGM_BIND_ACK || mprim.addr.len != sizeof(mprim.addr.add) ||
	    mprim.addr.add.scope != iut_mnt_addr.scope || mprim.addr.add.id != iut_mnt_addr.id ||
	    mprim.addr.add.cic != iut_mnt_addr.cic)
		return FAILURE;
	cprim.addr.add.scope = iut_cpc_addr.scope;
	cprim.addr.add.id = iut_cpc_addr.id;
	cprim.addr.add.cic = iut_cpc_addr.cic;
	cprim.addr.len = sizeof(cprim.addr.add);
	cprim.bind_flags = CC_TOKEN_REQUEST;
	iut_cpc_signal(BIND_REQ);
	if (any_event() != CPC_BIND_ACK || cprim.addr.len != sizeof(cprim.addr.add) ||
	    cprim.addr.add.scope != iut_cpc_addr.scope || cprim.addr.add.id != iut_cpc_addr.id ||
	    cprim.addr.add.cic != iut_cpc_addr.cic)
		return FAILURE;
#if 0
	iut_tst_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		return FAILURE;
	iut_mgm_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		return FAILURE;
	iut_mnt_signal(ADDR_REQ);
	if (any_event() != MGM_ADDR_ACK)
		return FAILURE;
	iut_cpc_signal(ADDR_REQ);
	if (any_event() != CPC_ADDR_ACK)
		return FAILURE;
#endif
	return SUCCESS;
}

static int
iut_unbind(void)
{
	iut_cpc_signal(UNBIND_REQ);
	if (any_event() != CPC_OK_ACK)
		return FAILURE;
	iut_tst_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		return FAILURE;
	iut_mgm_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		return FAILURE;
	iut_mnt_signal(UNBIND_REQ);
	if (any_event() != MGM_OK_ACK)
		return FAILURE;
	return SUCCESS;
}

static int
iut_start(void)
{
	if (iut_open() != SUCCESS)
		return FAILURE;
	if (iut_config() != SUCCESS)
		return FAILURE;
	if (iut_link() != SUCCESS)
		return FAILURE;
	if (iut_option() != SUCCESS)
		return FAILURE;
	if (iut_bind() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
iut_stop(void)
{
	if (iut_unbind() != SUCCESS)
		return FAILURE;
	if (iut_unlink() != SUCCESS)
		return FAILURE;
	if (iut_unconfig() != SUCCESS)
		return FAILURE;
	if (iut_close() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
begin_tests(void)
{
	if (pt_start() != SUCCESS)
		return FAILURE;
	if (iut_start() != SUCCESS)
		return FAILURE;
	show_acks = 1;
	return SUCCESS;
}

static int
end_tests(void)
{
	show_acks = 0;
	if (iut_stop() != SUCCESS)
		return FAILURE;
	if (pt_stop() != SUCCESS)
		return FAILURE;
	return SUCCESS;
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
	int (*preamble) (void);		/* test preamble */
	int (*testcase) (void);		/* test case */
	int (*postamble) (void);	/* test postamble */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
	"1.1.", desc_case_1_1, &preamble_0, &test_case_1_1, &postamble_0, 0, 0}, {
	"1.2.1.", desc_case_1_2_1, &preamble_0, &test_case_1_2_1, &postamble_0, 0, 0}, {
	"1.2.2.", desc_case_1_2_2, &preamble_0, &test_case_1_2_2, &postamble_0, 0, 0}, {
	"1.2.3.", desc_case_1_2_3, &preamble_0, &test_case_1_2_3, &postamble_1, 0, 0}, {
	"1.2.4.", desc_case_1_2_4, &preamble_0, &test_case_1_2_4, &postamble_2, 0, 0}, {
	"1.2.5a.", desc_case_1_2_5a, &preamble_1, &test_case_1_2_5a, &postamble_1, 0, 0}, {
	"1.2.5b.", desc_case_1_2_5b, &preamble_0, &test_case_1_2_5b, &postamble_0, 0, 0}, {
	"1.2.5c.", desc_case_1_2_5c, &preamble_0, &test_case_1_2_5c, &postamble_0, 0, 0}, {
	"1.2.6.", desc_case_1_2_6, &preamble_0, &test_case_1_2_6, &postamble_0, 0, 0}, {
	"1.2.7.", desc_case_1_2_7, &preamble_0, &test_case_1_2_7, &postamble_0, 0, 0}, {
	"1.3.1.1a.", desc_case_1_3_1_1a, &preamble_0, &test_case_1_3_1_1a, &postamble_0, 0, 0},
	{
	"1.3.1.1b.", desc_case_1_3_1_1b, &preamble_0, &test_case_1_3_1_1b, &postamble_0, 0, 0},
	{
	"1.3.1.1c.", desc_case_1_3_1_1c, &preamble_0, &test_case_1_3_1_1c, &postamble_0, 0, 0},
	{
	"1.3.1.1d.", desc_case_1_3_1_1d, &preamble_0, &test_case_1_3_1_1d, &postamble_0, 0, 0},
	{
	"1.3.1.1e.", desc_case_1_3_1_1e, &preamble_0, &test_case_1_3_1_1e, &postamble_0, 0, 0},
	{
	"1.3.1.1f.", desc_case_1_3_1_1f, &preamble_0, &test_case_1_3_1_1f, &postamble_0, 0, 0},
	{
	"1.3.1.2a.", desc_case_1_3_1_2a, &preamble_0, &test_case_1_3_1_2a, &postamble_0, 0, 0},
	{
	"1.3.1.2b.", desc_case_1_3_1_2b, &preamble_0, &test_case_1_3_1_2b, &postamble_0, 0, 0},
	{
	"1.3.2.1.", desc_case_1_3_2_1, &preamble_0, &test_case_1_3_2_1, &postamble_0, 0, 0}, {
	"1.3.2.2.", desc_case_1_3_2_2, &preamble_0, &test_case_1_3_2_2, &postamble_0, 0, 0}, {
	"1.3.2.3.", desc_case_1_3_2_3, &preamble_0, &test_case_1_3_2_3, &postamble_0, 0, 0}, {
	"1.3.2.4.", desc_case_1_3_2_4, &preamble_0, &test_case_1_3_2_4, &postamble_0, 0, 0}, {
	"1.3.2.5.", desc_case_1_3_2_5, &preamble_0, &test_case_1_3_2_5, &postamble_0, 0, 0}, {
	"1.4.1.", desc_case_1_4_1, &preamble_0, &test_case_1_4_1, &postamble_0, 0, 0}, {
	"1.4.2.", desc_case_1_4_2, &preamble_0, &test_case_1_4_2, &postamble_0, 0, 0}, {
	"1.4.3.", desc_case_1_4_3, &preamble_0, &test_case_1_4_3, &postamble_0, 0, 0}, {
	"1.4.4.", desc_case_1_4_4, &preamble_0, &test_case_1_4_4, &postamble_0, 0, 0}, {
	"1.4.5.", desc_case_1_4_5, &preamble_0, &test_case_1_4_5, &postamble_0, 0, 0}, {
	"1.5.1.", desc_case_1_5_1, &preamble_0, &test_case_1_5_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"1.5.2.", desc_case_1_5_2, &preamble_0, &test_case_1_5_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"1.5.3.", desc_case_1_5_3, &preamble_0, &test_case_1_5_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"1.6.1.1.", desc_case_1_6_1_1, &preamble_0, &test_case_1_6_1_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.6.1.2.", desc_case_1_6_1_2, &preamble_0, &test_case_1_6_1_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.6.2.1.", desc_case_1_6_2_1, &preamble_0, &test_case_1_6_2_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.6.2.2.", desc_case_1_6_2_2, &preamble_0, &test_case_1_6_2_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.6.3.1.", desc_case_1_6_3_1, &preamble_0, &test_case_1_6_3_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.6.3.2.", desc_case_1_6_3_2, &preamble_0, &test_case_1_6_3_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.1.", desc_case_1_7_1_1, &preamble_0, &test_case_1_7_1_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.2.", desc_case_1_7_1_2, &preamble_0, &test_case_1_7_1_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.3.", desc_case_1_7_1_3, &preamble_0, &test_case_1_7_1_3, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.4.", desc_case_1_7_1_4, &preamble_0, &test_case_1_7_1_4, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.5.", desc_case_1_7_1_5, &preamble_0, &test_case_1_7_1_5, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.6.", desc_case_1_7_1_6, &preamble_0, &test_case_1_7_1_6, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.1.7.", desc_case_1_7_1_7, &preamble_0, &test_case_1_7_1_7, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.1.", desc_case_1_7_2_1, &preamble_0, &test_case_1_7_2_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.2.", desc_case_1_7_2_2, &preamble_0, &test_case_1_7_2_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.3.", desc_case_1_7_2_3, &preamble_0, &test_case_1_7_2_3, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.4.", desc_case_1_7_2_4, &preamble_0, &test_case_1_7_2_4, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.5.", desc_case_1_7_2_5, &preamble_0, &test_case_1_7_2_5, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.6.", desc_case_1_7_2_6, &preamble_0, &test_case_1_7_2_6, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.7.", desc_case_1_7_2_7, &preamble_0, &test_case_1_7_2_7, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.8.", desc_case_1_7_2_8, &preamble_0, &test_case_1_7_2_8, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.9.", desc_case_1_7_2_9, &preamble_0, &test_case_1_7_2_9, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.2.10.", desc_case_1_7_2_10, &preamble_0, &test_case_1_7_2_10, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.3.1.", desc_case_1_7_3_1, &preamble_0, &test_case_1_7_3_1, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"1.7.3.2.", desc_case_1_7_3_2, &preamble_0, &test_case_1_7_3_2, &postamble_0, 0,
		    INCONCLUSIVE}, {
	"2.1.1.", desc_case_2_1_1, &preamble_0, &test_case_2_1_1, &postamble_0, 0, 0}, {
	"2.1.2.", desc_case_2_1_2, &preamble_0, &test_case_2_1_2, &postamble_0, 0, 0}, {
	"2.2.1a.", desc_case_2_2_1a, &preamble_0, &test_case_2_2_1a, &postamble_0, 0, 0}, {
	"2.2.1b.", desc_case_2_2_1b, &preamble_0, &test_case_2_2_1b, &postamble_0, 0, 0}, {
	"2.2.2a.", desc_case_2_2_2a, &preamble_0, &test_case_2_2_2a, &postamble_0, 0, 0}, {
	"2.2.2b.", desc_case_2_2_2b, &preamble_0, &test_case_2_2_2b, &postamble_0, 0, 0}, {
	"2.3.1a.", desc_case_2_3_1a, &preamble_0, &test_case_2_3_1a, &postamble_0, 0, 0}, {
	"2.3.1b.", desc_case_2_3_1b, &preamble_0, &test_case_2_3_1b, &postamble_0, 0, 0}, {
	"2.3.1c.", desc_case_2_3_1c, &preamble_0, &test_case_2_3_1c, &postamble_0, 0, 0}, {
	"2.3.1d.", desc_case_2_3_1d, &preamble_0, &test_case_2_3_1d, &postamble_0, 0, 0}, {
	"2.3.1e.", desc_case_2_3_1e, &preamble_0, &test_case_2_3_1e, &postamble_0, 0, 0}, {
	"2.3.1f.", desc_case_2_3_1f, &preamble_0, &test_case_2_3_1f, &postamble_0, 0, 0}, {
	"2.3.1g.", desc_case_2_3_1g, &preamble_0, &test_case_2_3_1g, &postamble_0, 0, 0}, {
	"2.3.1h.", desc_case_2_3_1h, &preamble_0, &test_case_2_3_1h, &postamble_0, 0, 0}, {
	"2.3.2a.", desc_case_2_3_2a, &preamble_0, &test_case_2_3_2a, &postamble_0, 0, 0}, {
	"2.3.2b.", desc_case_2_3_2b, &preamble_0, &test_case_2_3_2b, &postamble_0, 0, 0}, {
	"2.3.2c.", desc_case_2_3_2c, &preamble_0, &test_case_2_3_2c, &postamble_0, 0, 0}, {
	"2.3.2d.", desc_case_2_3_2d, &preamble_0, &test_case_2_3_2d, &postamble_0, 0, 0}, {
	"2.3.2e.", desc_case_2_3_2e, &preamble_0, &test_case_2_3_2e, &postamble_0, 0, 0}, {
	"2.3.2f.", desc_case_2_3_2f, &preamble_0, &test_case_2_3_2f, &postamble_0, 0, 0}, {
	"2.3.3a.", desc_case_2_3_3a, &preamble_0, &test_case_2_3_3a, &postamble_0, 0, 0}, {
	"2.3.3b.", desc_case_2_3_3b, &preamble_0, &test_case_2_3_3b, &postamble_0, 0, 0}, {
	"2.3.4a.", desc_case_2_3_4a, &preamble_0, &test_case_2_3_4a, &postamble_0, 0, 0}, {
	"2.3.4b.", desc_case_2_3_4b, &preamble_0, &test_case_2_3_4b, &postamble_0, 0, 0}, {
	"2.3.5a.", desc_case_2_3_5a, &preamble_0, &test_case_2_3_5a, &postamble_0, 0, 0}, {
	"2.3.5b.", desc_case_2_3_5b, &preamble_0, &test_case_2_3_5b, &postamble_0, 0, 0}, {
	"2.3.6a.", desc_case_2_3_6a, &preamble_0, &test_case_2_3_6a, &postamble_0, 0, 0}, {
	"2.3.6b.", desc_case_2_3_6b, &preamble_0, &test_case_2_3_6b, &postamble_0, 0, 0}, {
	"2.4.1.", desc_case_2_4_1, &preamble_0, &test_case_2_4_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"2.4.2.", desc_case_2_4_2, &preamble_0, &test_case_2_4_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"2.4.3.", desc_case_2_4_3, &preamble_0, &test_case_2_4_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"2.4.4.", desc_case_2_4_4, &preamble_0, &test_case_2_4_4, &postamble_0, 0, INCONCLUSIVE},
	{
	"2.4.5.", desc_case_2_4_5, &preamble_0, &test_case_2_4_5, &postamble_0, 0, INCONCLUSIVE},
	{
	"3.1a.", desc_case_3_1a, &preamble_0, &test_case_3_1a, &postamble_0, 0, 0}, {
	"3.1b.", desc_case_3_1b, &preamble_0, &test_case_3_1b, &postamble_0, 0, 0}, {
	"3.2a.", desc_case_3_2a, &preamble_0, &test_case_3_2a, &postamble_0, 0, 0}, {
	"3.2b.", desc_case_3_2b, &preamble_0, &test_case_3_2b, &postamble_0, 0, 0}, {
	"3.3a.", desc_case_3_3a, &preamble_0, &test_case_3_3a, &postamble_0, 0, 0}, {
	"3.3b.", desc_case_3_3b, &preamble_0, &test_case_3_3b, &postamble_0, 0, 0}, {
	"3.4a.", desc_case_3_4a, &preamble_0, &test_case_3_4a, &postamble_0, 0, 0}, {
	"3.4b.", desc_case_3_4b, &preamble_0, &test_case_3_4b, &postamble_0, 0, 0}, {
	"3.5a.", desc_case_3_5a, &preamble_0, &test_case_3_5a, &postamble_0, 0, 0}, {
	"3.5b.", desc_case_3_5b, &preamble_0, &test_case_3_5b, &postamble_0, 0, 0}, {
	"3.6a.", desc_case_3_6a, &preamble_0, &test_case_3_6a, &postamble_0, 0, 0}, {
	"3.6b.", desc_case_3_6b, &preamble_0, &test_case_3_6b, &postamble_0, 0, 0}, {
	"3.7a.", desc_case_3_7a, &preamble_0, &test_case_3_7a, &postamble_0, 0, 0}, {
	"3.7b.", desc_case_3_7b, &preamble_0, &test_case_3_7b, &postamble_0, 0, 0}, {
	"3.8a.", desc_case_3_8a, &preamble_0, &test_case_3_8a, &postamble_0, 0, 0}, {
	"3.8b.", desc_case_3_8b, &preamble_0, &test_case_3_8b, &postamble_0, 0, 0}, {
	"4.1a.", desc_case_4_1a, &preamble_0, &test_case_4_1a, &postamble_0, 0, 0}, {
	"4.1b.", desc_case_4_1b, &preamble_0, &test_case_4_1b, &postamble_0, 0, 0}, {
	"4.1c.", desc_case_4_1c, &preamble_0, &test_case_4_1c, &postamble_0, 0, 0}, {
	"4.1d.", desc_case_4_1d, &preamble_0, &test_case_4_1d, &postamble_0, 0, 0}, {
	"4.1e.", desc_case_4_1e, &preamble_0, &test_case_4_1e, &postamble_0, 0, 0}, {
	"4.1f.", desc_case_4_1f, &preamble_0, &test_case_4_1f, &postamble_0, 0, 0}, {
	"5.1.", desc_case_5_1, &preamble_0, &test_case_5_1, &postamble_0, 0, INCONCLUSIVE}, {
	"5.2.1.", desc_case_5_2_1, &preamble_0, &test_case_5_2_1, &postamble_0, 0, 0}, {
	"5.2.2.", desc_case_5_2_2, &preamble_0, &test_case_5_2_2, &postamble_0, 0, 0}, {
	"5.2.3.", desc_case_5_2_3, &preamble_0, &test_case_5_2_3, &postamble_0, 0, 0}, {
	"5.2.4.", desc_case_5_2_4, &preamble_0, &test_case_5_2_4, &postamble_0, 0, 0}, {
	"5.2.5a.", desc_case_5_2_5a, &preamble_0, &test_case_5_2_5a, &postamble_0, 0, 0}, {
	"5.2.5b.", desc_case_5_2_5b, &preamble_0, &test_case_5_2_5b, &postamble_0, 0, 0}, {
	"5.2.6.", desc_case_5_2_6, &preamble_0, &test_case_5_2_6, &postamble_0, 0, 0}, {
	"5.2.7.", desc_case_5_2_7, &preamble_0, &test_case_5_2_7, &postamble_0, 0, 0}, {
	"5.2.8.", desc_case_5_2_8, &preamble_0, &test_case_5_2_8, &postamble_0, 0, 0}, {
	"5.2.9.", desc_case_5_2_9, &preamble_0, &test_case_5_2_9, &postamble_0, 0, 0}, {
	"5.2.10.", desc_case_5_2_10, &preamble_0, &test_case_5_2_10, &postamble_0, 0, 0}, {
	"5.2.11.", desc_case_5_2_11, &preamble_0, &test_case_5_2_11, &postamble_0, 0, 0}, {
	"5.3.1.", desc_case_5_3_1, &preamble_0, &test_case_5_3_1, &postamble_0, 0, 0}, {
	"5.3.2.", desc_case_5_3_2, &preamble_0, &test_case_5_3_2, &postamble_0, 0, 0}, {
	"6.1.1.", desc_case_6_1_1, &preamble_0, &test_case_6_1_1, &postamble_0, 0, 0}, {
	"6.1.2.", desc_case_6_1_2, &preamble_0, &test_case_6_1_2, &postamble_0, 0, 0}, {
	"6.1.3.", desc_case_6_1_3, &preamble_0, &test_case_6_1_3, &postamble_0, 0, 0}, {
	"6.1.4.", desc_case_6_1_4, &preamble_0, &test_case_6_1_4, &postamble_0, 0, 0}, {
	"6.1.5.", desc_case_6_1_5, &preamble_0, &test_case_6_1_5, &postamble_0, 0, 0}, {
	"6.2.1.", desc_case_6_2_1, &preamble_0, &test_case_6_2_1, &postamble_0, 0, 0}, {
	"6.2.2.", desc_case_6_2_2, &preamble_0, &test_case_6_2_2, &postamble_0, 0, 0}, {
	"6.2.3.", desc_case_6_2_3, &preamble_0, &test_case_6_2_3, &postamble_0, 0, 0}, {
	"6.2.4.", desc_case_6_2_4, &preamble_0, &test_case_6_2_4, &postamble_0, 0, 0}, {
	"6.2.5.", desc_case_6_2_5, &preamble_0, &test_case_6_2_5, &postamble_0, 0, 0}, {
	"6.3.1.", desc_case_6_3_1, &preamble_0, &test_case_6_3_1, &postamble_0, 0, 0}, {
	"6.4.1.", desc_case_6_4_1, &preamble_0, &test_case_6_4_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.4.2.", desc_case_6_4_2, &preamble_0, &test_case_6_4_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.4.3.", desc_case_6_4_3, &preamble_0, &test_case_6_4_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.4.3.", desc_case_6_4_4, &preamble_0, &test_case_6_4_4, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.5.1.", desc_case_6_5_1, &preamble_0, &test_case_6_5_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.5.2.", desc_case_6_5_2, &preamble_0, &test_case_6_5_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.5.3.", desc_case_6_5_3, &preamble_0, &test_case_6_5_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.5.4.", desc_case_6_5_4, &preamble_0, &test_case_6_5_4, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.5.4.", desc_case_6_5_5, &preamble_0, &test_case_6_5_5, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.6.1.", desc_case_6_6_1, &preamble_0, &test_case_6_6_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.6.2.", desc_case_6_6_2, &preamble_0, &test_case_6_6_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.6.3.", desc_case_6_6_3, &preamble_0, &test_case_6_6_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"6.6.4.", desc_case_6_6_4, &preamble_0, &test_case_6_6_4, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.1.1.", desc_case_7_1_1, &preamble_0, &test_case_7_1_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.1.2.", desc_case_7_1_2, &preamble_0, &test_case_7_1_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.1.3.", desc_case_7_1_3, &preamble_0, &test_case_7_1_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.2.1.", desc_case_7_2_1, &preamble_0, &test_case_7_2_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.1.", desc_case_7_3_1, &preamble_0, &test_case_7_3_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.2.", desc_case_7_3_2, &preamble_0, &test_case_7_3_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.3.", desc_case_7_3_3, &preamble_0, &test_case_7_3_3, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.4.", desc_case_7_3_4, &preamble_0, &test_case_7_3_4, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.5.", desc_case_7_3_5, &preamble_0, &test_case_7_3_5, &postamble_0, 0, INCONCLUSIVE},
	{
	"7.3.6.", desc_case_7_3_6, &preamble_0, &test_case_7_3_6, &postamble_0, 0, INCONCLUSIVE},
	{
	"8.1.1.", desc_case_8_1_1, &preamble_0, &test_case_8_1_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"8.1.2.", desc_case_8_1_2, &preamble_0, &test_case_8_1_2, &postamble_0, 0, INCONCLUSIVE},
	{
	"8.2.1.", desc_case_8_2_1, &preamble_0, &test_case_8_2_1, &postamble_0, 0, 0}, {
	"8.2.2.", desc_case_8_2_2, &preamble_0, &test_case_8_2_2, &postamble_0, 0, 0}, {
	"8.2.3.", desc_case_8_2_3, &preamble_0, &test_case_8_2_3, &postamble_0, 0, 0}, {
	"9.1.1.", desc_case_9_1_1, &preamble_0, &test_case_9_1_1, &postamble_0, 0, INCONCLUSIVE},
	{
	"9.1.2.", desc_case_9_1_2, &preamble_0, &test_case_9_1_2, &postamble_0, 0, INCONCLUSIVE},
	{
	NULL,}
};

static int summary = 0;

int
do_tests(void)
{
	int i;
	int result = INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	printf("\n\nQ.784 conformance test program for streams-isup driver.\n");
	FFLUSH(stdout);
	if (begin_tests() == SUCCESS) {
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = INCONCLUSIVE;
				continue;
			}
			printf("\nTest Case Q.784/%s:\n%s\n", tests[i].numb, tests[i].name);
			printf
			    ("--------------------+--+---------Preamble--------------+--------------------\n");
			FFLUSH(stdout);
			if (tests[i].preamble() != SUCCESS) {
				printf
				    ("????????????????????|??|????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n",
				     state);
				FFLUSH(stdout);
				result = INCONCLUSIVE;
			} else {
				printf
				    ("--------------------|--|-----------Test----------------|--------------------\n");
				FFLUSH(stdout);
				switch (tests[i].testcase()) {
				default:
				case INCONCLUSIVE:
					printf
					    ("????????????????????|??|????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n",
					     state);
					FFLUSH(stdout);
					result = INCONCLUSIVE;
					break;
				case FAILURE:
					printf
					    ("XXXXXXXXXXXXXXXXXXXX|XX|XXXXXXXXX FAILED XXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX(%d)\n",
					     state);
					FFLUSH(stdout);
					result = FAILURE;
					break;
				case SCRIPTERROR:
					printf
					    ("####################|##|####### SCRIPT ERROR ###########|###################(%d)\n",
					     state);
					FFLUSH(stdout);
					result = SCRIPTERROR;
					break;
				case SUCCESS:
					printf
					    ("********************|**|********* PASSED **************|********************(%d)\n",
					     state);
					FFLUSH(stdout);
					result = SUCCESS;
					break;
				}
			}
			printf
			    ("--------------------|--|----------Postamble------------|--------------------\n");
			FFLUSH(stdout);
			if (tests[i].postamble() != SUCCESS) {
				printf
				    ("????????????????????|??|????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n",
				     state);
				FFLUSH(stdout);
				if (result == SUCCESS)
					result = INCONCLUSIVE;
			}
			printf
			    ("--------------------+--+-------------------------------+--------------------\n");
			FFLUSH(stdout);
			switch (result) {
			case SUCCESS:
				successes++;
				printf("*********\n");
				printf("********* Test Case SUCCESSFUL\n");
				printf("*********\n\n");
				FFLUSH(stdout);
				break;
			case FAILURE:
				failures++;
				printf("XXXXXXXXX\n");
				printf("XXXXXXXXX Test Case FAILED\n");
				printf("XXXXXXXXX\n\n");
				FFLUSH(stdout);
				break;
			default:
			case INCONCLUSIVE:
				inconclusive++;
				printf("?????????\n");
				printf("????????? Test Case INCONCLUSIVE\n");
				printf("?????????\n\n");
				FFLUSH(stdout);
				break;
			}
			tests[i].result = result;
		}
		end_tests();
		if (summary) {
			printf("\n\n");
			FFLUSH(stdout);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb;
			     i++) {
				if (tests[i].run) {
					printf("Test Case Q.784/%-10s ", tests[i].numb);
					FFLUSH(stdout);
					switch (tests[i].result) {
					case SUCCESS:
						printf("SUCCESS\n");
						FFLUSH(stdout);
						break;
					case FAILURE:
						printf("FAILURE\n");
						FFLUSH(stdout);
						break;
					default:
					case INCONCLUSIVE:
						printf("INCONCLUSIVE\n");
						FFLUSH(stdout);
						break;
					}
				}
			}
		}
		printf("Done.\n\n");
		printf("========= %2d successes   \n", successes);
		printf("========= %2d failures    \n", failures);
		printf("========= %2d inconclusive\n", inconclusive);
		FFLUSH(stdout);
		return (0);
	} else {
		printf("Test setup failed!\n");
		FFLUSH(stdout);
		end_tests();
		return (2);
	}
}

void
copying(int argc, char *argvp[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
Q.784 ISUP Basic Call Test Specification - Conformance Test Program\n\
\n\
Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompliation.\n\
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
various agencies, utilization of advances in the state of  the technical ars, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
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
rights in the Software are defined in paragraph 52.227-19 of the Federal Acquis-\n\
ition Regulations (\"FAR\") (or any success regulations) or, in the cases of NASA,\n\
in paragraph  18.52.227-86 of the NASA  Supplement  to the FAR (or any successor\n\
regulations).\n\
");
	FFLUSH(stdout);
}

void
version(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
", argv[0], ident);
	FFLUSH(stdout);
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
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -m, --messages\n\
        enables message output\n\
    -s, --summary\n\
        print summary\n\
    -f, --fast [SCALE]\n\
        run tests fast with optional timer SCALE factor (default 100)\n\
    -t, --tests SUBSTRING\n\
        specifies the initial SUBSTRING of the test number to run\n\
    -o, --onetest STRING\n\
        specifies the test number STRING to run\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
", argv[0]);
}

int
main(int argc, char **argv)
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
			{"fast",	optional_argument,	NULL, 'f'},
			{"summary",	no_argument,		NULL, 's'},
			{"onetest",	required_argument,	NULL, 'o'},
			{"tests",	required_argument,	NULL, 't'},
			{"messages",	no_argument,		NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	no_argument,		NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'f':
			if (optarg)
				timer_scale = atoi(optarg);
			else
				timer_scale = 50;
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n",
				timer_scale);
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
				fprintf(stderr, "WARNING: specification `%s' matched no test\n",
					optarg);
				FFLUSH(stderr);
			}
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
				fprintf(stderr, "WARNING: specification `%s' matched no test\n",
					optarg);
				FFLUSH(stderr);
			}
			break;
		case 'm':
			show_msg = 1;
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
	if (optind < argc)
		goto bad_nonopt;
	if (!tests_to_run) {
		fprintf(stderr, "ERROR: no tests to run\n");
		FFLUSH(stderr);
		exit(1);
	}
	copying(argc, argv);
	return do_tests();
}
