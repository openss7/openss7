/*****************************************************************************

 @(#) $RCSfile: test-q781-pipe.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/19 11:57:20 $

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
 General Public License (GPL) Version 3, so long as the software is distributed
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

 Last Modified $Date: 2007/08/19 11:57:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-q781-pipe.c,v $
 Revision 0.9.2.8  2007/08/19 11:57:20  brian
 - move stdbool.h, obviate need for YFLAGS, general workup

 Revision 0.9.2.7  2007/08/14 12:20:05  brian
 - GPLv3 header updates

 Revision 0.9.2.6  2006/10/31 21:04:46  brian
 - changes for 32-bit compatibility and remove HZ dependency

 Revision 0.9.2.5  2006/03/04 13:00:38  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 Revision 0.9.2.4  2005/05/14 08:31:34  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-q781-pipe.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/19 11:57:20 $"

static char const ident[] = "$RCSfile: test-q781-pipe.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/19 11:57:20 $";

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
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define SUCCESS       0
#define FAILURE       1
#define INCONCLUSIVE -1
#define SCRIPTERROR  -2

#define FFLUSH(stream)

/* 
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static struct {
	lmi_option_t opt;
	sdt_config_t sdt;
	sdl_config_t sdl;
} ptconf;

static struct {
	lmi_option_t opt;
	sl_config_t sl;
	sdt_config_t sdt;
	sdl_config_t sdl;
} iutconf;

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

#define TEST_TIMEOUT 5000

typedef struct timer_range {
	long lo;
	long hi;
} timer_range_t;

enum { t1 = 0, t2, t3, t4n, t4e, t5, t6, t7, tmax };

#if 0
static timer_range_t timer[tmax] = {
	{40000, 50000},		/* Timer T1 30000 */
	{5000, 150000},		/* Timer T2 5000 */
	{1000, 1500},		/* Timer T3 100 */
	{7500, 9500},		/* Timer T4n 3000 */
	{400, 600},		/* Timer T4e 50 */
	{125, 125},		/* Timer T5 10 */
	{3000, 6000},		/* Timer T6 300 */
	{500, 2000}		/* Timer T7 50 */
};
#endif

long test_start = 0;

#if 0
/* 
 *  Return the current time in milliseconds.
 */
static long
milliseconds(char *t)
{
	long ret;
	struct timeval now;

	printf("                                     |\n");
	printf("                                     | %s\n", t);
	printf("                                     |\n");
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
		printf("    ****ERROR: couldn't get time!\n");
		printf("               %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	i = (now.tv_sec - test_start) * 1000;
	i += (now.tv_usec + 500L) / 1000;
	i -= beg;
	printf("                 check time:       %4s (%ld <= %ld <= %ld)\n", t, lo - 100, i, hi + 100);
	FFLUSH(stdout);
	if (lo - 100 <= i && i <= hi + 100)
		return SUCCESS;
	else
		return FAILURE;
}
#endif

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

#define SIO             LSSU_SIO	/* PT events and signals */
#define SIN             LSSU_SIN
#define SIE             LSSU_SIE
#define SIOS            LSSU_SIOS
#define SIPO            LSSU_SIPO
#define SIB             LSSU_SIB
#define SIX             6

#define SIO2            100 + LSSU_SIO	/* PT events and signals */
#define SIN2            100 + LSSU_SIN
#define SIE2            100 + LSSU_SIE
#define SIOS2           100 + LSSU_SIOS
#define SIPO2           100 + LSSU_SIPO
#define SIB2            100 + LSSU_SIB
#define SIX2            100 + 6

#define FISU            20	/* PT signals and events */
#define FISU_S          21
#define FISU_BAD_FIB    22
#define FISU_CORRUPT    23
#define FISU_CORRUPT_S  24
#define LSSU_CORRUPT    25
#define LSSU_CORRUPT_S  32
#define MSU             26
#define MSU_SEVEN_ONES  27
#define MSU_TOO_LONG    28
#define MSU_TOO_SHORT   29
#define TX_BREAK        30
#define TX_MAKE         31
#define FISU_FISU_1FLAG 60
#define FISU_FISU_2FLAG 61
#define MSU_MSU_1FLAG   62
#define MSU_MSU_2FLAG   63

#define TIMEOUT         40	/* TIMER events */
#define COUNT           41
#define TRIES           39
#define ETC             49
#define SIB_S           42

#define IN_SERVICE      43	/* IUT events */
#define OUT_OF_SERVICE  44
#define RPO             45
#define RPR             46
#define IUT_MSU         47

#define POWER_ON        200	/* IUT signals */
#define START           201
#define STOP            202
#define LPO             203
#define LPR             204
#define EMERG           205
#define CEASE           206
#define SEND_MSU        207
#define SEND_MSU_S      208
#define CONG_A          209
#define CONG_D          210
#define NO_CONG         211
#define CLEARB          212

#define UNKNOWN         48

#define NO_MSG          -1

static const char *
event_string(int e)
{
	switch (e) {
	case SIO:
		return ("SIO");
	case SIN:
		return ("SIN");
	case SIE:
		return ("SIE");
	case SIOS:
		return ("SIOS");
	case SIPO:
		return ("SIPO");
	case SIB:
		return ("SIB");
	case SIX:
		return ("SIX");
	case SIO2:
		return ("SIO2");
	case SIN2:
		return ("SIN2");
	case SIE2:
		return ("SIE2");
	case SIOS2:
		return ("SIOS2");
	case SIPO2:
		return ("SIPO2");
	case SIB2:
		return ("SIB2");
	case SIX2:
		return ("SIX2");
	case FISU:
		return ("FISU");
	case FISU_S:
		return ("FISU_S");
	case FISU_BAD_FIB:
		return ("FISU_BAD_FIB");
	case FISU_CORRUPT:
		return ("FISU_CORRUPT");
	case FISU_CORRUPT_S:
		return ("FISU_CORRUPT_S");
	case LSSU_CORRUPT:
		return ("LSSU_CORRUPT");
	case LSSU_CORRUPT_S:
		return ("LSSU_CORRUPT_S");
	case MSU:
		return ("MSU");
	case MSU_SEVEN_ONES:
		return ("MSU_SEVEN_ONES");
	case MSU_TOO_LONG:
		return ("MSU_TOO_LONG");
	case MSU_TOO_SHORT:
		return ("MSU_TOO_SHORT");
	case TX_BREAK:
		return ("TX_BREAK");
	case TX_MAKE:
		return ("TX_MAKE");
	case FISU_FISU_1FLAG:
		return ("FISU_FISU_1FLAG");
	case FISU_FISU_2FLAG:
		return ("FISU_FISU_2FLAG");
	case MSU_MSU_1FLAG:
		return ("MSU_MSU_1FLAG");
	case MSU_MSU_2FLAG:
		return ("MSU_MSU_2FLAG");
	case TIMEOUT:
		return ("TIMEOUT");
	case COUNT:
		return ("COUNT");
	case TRIES:
		return ("TRIES");
	case ETC:
		return ("ETC");
	case SIB_S:
		return ("SIB_S");
	case IN_SERVICE:
		return ("IN_SERVICE");
	case OUT_OF_SERVICE:
		return ("OUT_OF_SERVICE");
	case RPO:
		return ("RPO");
	case RPR:
		return ("RPR");
	case IUT_MSU:
		return ("IUT_MSU");
	case POWER_ON:
		return ("POWER_ON");
	case START:
		return ("START");
	case STOP:
		return ("STOP");
	case LPO:
		return ("LPO");
	case LPR:
		return ("LPR");
	case EMERG:
		return ("EMERG");
	case CEASE:
		return ("CEASE");
	case SEND_MSU:
		return ("SEND_MSU");
	case SEND_MSU_S:
		return ("SEND_MSU_S");
	case CONG_A:
		return ("CONG_A");
	case CONG_D:
		return ("CONG_D");
	case NO_CONG:
		return ("NO_CONG");
	case CLEARB:
		return ("CLEARB");
	case UNKNOWN:
		return ("UNKNOWN");
	case NO_MSG:
		return ("NO_MSG");
	default:
		return ("??????");
	}
}

#define BUFSIZE 300

static int state = 0;
static int count = 0;
static int tries = 0;
static int expand = 0;
static long beg_time = 0;

static unsigned long iut_options = 0;

int pipefd[2] = { 0, 0 };

#define pt_fd (pipefd[0])
unsigned char pt_buf[BUFSIZE];
unsigned char pt_fib = 0x80;
unsigned char pt_fsn = 0x7f;
unsigned char pt_bib = 0x80;
unsigned char pt_bsn = 0x7f;
unsigned char pt_li = 0;
unsigned char pt_sio = 0;

#define iut_fd (pipefd[1])
unsigned char iut_buf[BUFSIZE];
unsigned char iut_fib = 0x80;
unsigned char iut_fsn = 0x7f;
unsigned char iut_bib = 0x80;
unsigned char iut_bsn = 0x7f;
unsigned char iut_li = 0;
unsigned char iut_sio = 0;
unsigned char iut_len = 0;

#define MSU_LEN 35
int msu_len = MSU_LEN;

static int oldpsb = 0;
static int oldmsg = 0;
static int cntmsg = 0;
static int oldact = 0;
static int cntact = 0;
static int oldisb = 0;
static int oldret = 0;
static int cntret = 0;
static int oldprm = 0;
static int cntprm = 0;

static int verbose = 1;

#define send pt_send

static int
send(int msg)
{
	int ret = SUCCESS;
	int len;
	char *label = NULL;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*pt_buf), 0, (char *) pt_buf };
	struct strioctl ioc;
	union SDT_primitives *p = (union SDT_primitives *) cbuf;

	if (msg != oldmsg || oldpsb != (((pt_bib | pt_bsn) << 8) | (pt_fib | pt_fsn))) {
		oldmsg = msg;
		oldpsb = ((pt_bib | pt_bsn) << 8) | (pt_fib | pt_fsn);
		// if ( cntmsg ) {
		// printf(" Ct=%d\n", cntmsg+1);
		// FFLUSH(stdout);
		// }
		cntmsg = 0;
	} else if (!expand)
		cntmsg++;
	switch (msg) {
	case SIO:
	case SIN:
	case SIE:
	case SIOS:
	case SIPO:
	case SIB:
	case SIX:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 1;
		pt_buf[3] = msg;
		len = 4;
		switch (msg) {
		case SIO:
			if (!cntmsg || verbose)
				printf("    SIO  (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIN:
			if (!cntmsg || verbose)
				printf("    SIN  (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIE:
			if (!cntmsg || verbose)
				printf("    SIE  (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIOS:
			if (!cntmsg || verbose)
				printf("    SIOS (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIPO:
			if (!cntmsg || verbose)
				printf("    SIPO (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIB:
			if (!cntmsg || verbose)
				printf("    SIB  (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIX:
			if (!cntmsg || verbose)
				printf("    LSSU (%02x/%02x) (corrupt)------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		}
		FFLUSH(stdout);
		break;
	case SIO2:
	case SIN2:
	case SIE2:
	case SIOS2:
	case SIPO2:
	case SIB2:
	case SIX2:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 2;
		pt_buf[3] = 0;
		pt_buf[4] = msg - 100;
		len = 5;
		switch (msg) {
		case SIO2:
			if (!cntmsg || verbose)
				printf("    SIO  (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIN2:
			if (!cntmsg || verbose)
				printf("    SIN  (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIE2:
			if (!cntmsg || verbose)
				printf("    SIE  (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIOS2:
			if (!cntmsg || verbose)
				printf("    SIOS (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIPO2:
			if (!cntmsg || verbose)
				printf("    SIPO (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIB2:
			if (!cntmsg || verbose)
				printf("    SIB  (%02x/%02x)[2]-------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		case SIX2:
			if (!cntmsg || verbose)
				printf("    LSSU (%02x/%02x)[2](corrupt)----->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			break;
		}
		FFLUSH(stdout);
		break;
	case SIB_S:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 1;
		pt_buf[3] = SIB;
		len = 4;
		break;
	case COUNT:
		printf("    Ct=%d\n", count);
		FFLUSH(stdout);
		return SUCCESS;
	case TRIES:
		printf("    %d iterations\n", tries);
		FFLUSH(stdout);
		return SUCCESS;
	case ETC:
		printf("      .\n");
		printf("      .\n");
		printf("      .\n");
		FFLUSH(stdout);
		return SUCCESS;
	case FISU:
		if (!cntmsg || verbose) {
			printf("    FISU (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
	case FISU_S:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 0;
		len = 3;
		break;
	case FISU_BAD_FIB:
		if (!cntmsg || verbose) {
			printf("    FISU (%02x/%02x) (bad fib)------->\n", pt_bib | pt_bsn, (pt_fib | pt_fsn) ^ 0x80);
			FFLUSH(stdout);
		}
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = (pt_fib | pt_fsn) ^ 0x80;
		pt_buf[2] = 0;
		len = 3;
		break;
	case LSSU_CORRUPT:
		if (!cntmsg || verbose) {
			printf("    LSSU (%02x/%02x) (corrupt)------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
	case LSSU_CORRUPT_S:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 0xff;
		pt_buf[3] = 0xff;
		len = 4;
		break;
	case FISU_CORRUPT:
		if (!cntmsg || verbose) {
			printf("    FISU (%02x/%02x) (corrupt)------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
	case FISU_CORRUPT_S:
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 0xff;
		len = 3;
		break;
	case MSU:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		pt_fsn = (pt_fsn + 1) & 0x7f;
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = msu_len;
		memset(&pt_buf[3], 'B', msu_len);
		len = msu_len + 3;
		if (!cntmsg || verbose) {
			printf("    MSU  (%02x/%02x) ---------------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
		break;
	case MSU_TOO_LONG:
		pt_fsn = (pt_fsn + 1) & 0x7f;
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		pt_buf[2] = 63;
		memset(&pt_buf[3], 'A', 280);
		len = 283;
		if (!cntmsg || verbose) {
			printf("    MSU  (%02x/%02x) (too long)------>\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
		break;
	case MSU_SEVEN_ONES:
		pt_fsn = (pt_fsn + 1) & 0x7f;
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		len = 2;
		if (!cntmsg || verbose) {
			printf("    MSU  (%02x/%02x) (7 ones)-------->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
		return FAILURE;	/* can't do this */
	case MSU_TOO_SHORT:
		pt_fsn = (pt_fsn + 1) & 0x7f;
		pt_buf[0] = pt_bib | pt_bsn;
		pt_buf[1] = pt_fib | pt_fsn;
		len = 2;
		if (!cntmsg || verbose) {
			printf("    MSU  (%02x/%02x) (too short)----->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
			FFLUSH(stdout);
		}
		break;
	case TX_BREAK:
		printf("   :break Tx\n");
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
		ioc.ic_cmd = DEV_IOCCDISCTX;
		ioc.ic_timout = 0;
		ioc.ic_len = 0;
		ioc.ic_dp = NULL;
		if (ioctl(pt_fd, I_STR, &ioc) < 0) {
			printf("    ****ERROR: ioctl failed!\n");
			printf("               %s: %s\n", __FUNCTION__, strerror(errno));
			return FAILURE;
		}
		return FAILURE;	/* can't do this */
	case TX_MAKE:
		printf("   :reconnect Tx\n");
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
		ioc.ic_cmd = DEV_IOCCCONNTX;
		ioc.ic_timout = 0;
		ioc.ic_len = 0;
		ioc.ic_dp = NULL;
		if (ioctl(pt_fd, I_STR, &ioc) < 0) {
			printf("    ****ERROR: ioctl failed!\n");
			printf("               %s: %s\n", __FUNCTION__, strerror(errno));
			return FAILURE;
		}
		return FAILURE;	/* can't do this */
	case FISU_FISU_1FLAG:
		printf("    FISU (%02x/%02x) ---FISU-F-FISU-->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
	case FISU_FISU_2FLAG:
		printf("    FISU (%02x/%02x) --FISU-F-F-FISU->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
	case MSU_MSU_1FLAG:
		printf("    MSU  (%02x/%02x) ----MSU-F-MSU--->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
	case MSU_MSU_2FLAG:
		printf("    MSU  (%02x/%02x) ---MSU-F-F-MSU-->\n", pt_bib | pt_bsn, pt_fib | pt_fsn);
		FFLUSH(stdout);
		return FAILURE;	/* can't do this */
	case POWER_ON:
		label = "power on";
		goto dummy_command;
	case START:
		label = "start";
		goto dummy_command;
	case STOP:
		label = "stop";
		goto dummy_command;
	case LPO:
		label = "set lpo";
		goto dummy_command;
	case LPR:
		label = "clear lpo";
		goto dummy_command;
	case EMERG:
		label = "set emerg";
		goto dummy_command;
	case CEASE:
		label = "clear emerg";
		goto dummy_command;
	case CONG_A:
		label = "make congestion state";
		goto dummy_command;
	case CONG_D:
		label = "make congestion state";
		goto dummy_command;
	case NO_CONG:
		label = "clear congestion state";
		goto dummy_command;
	      dummy_command:
		printf("   :%s\n", label);
		FFLUSH(stdout);
		return SUCCESS;
	default:
		if (!cntmsg || verbose) {
			printf("    ???? (--/--) ---------------->\n");
			FFLUSH(stdout);
		}
		return FAILURE;
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->daedt_transmission_req);
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.len = len;
	data.buf = (char *) pt_buf;
	p->daedt_transmission_req.sdt_primitive = SDT_DAEDT_TRANSMISSION_REQ;
	if (putmsg(pt_fd, NULL, &data, 0) < 0) {
		if (errno == EAGAIN || errno == EINTR)
			return FAILURE;
		printf("    ****ERROR: putmsg failed!\n");
		printf("               %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	return ret;
}

#define signal iut_signal

static int
signal(int action)
{
	int ret;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union SL_primitives *p = (union SL_primitives *) cbuf;

	ctrl.maxlen = BUFSIZE;
	ctrl.buf = cbuf;
	if (action != oldact) {
		oldact = action;
		// if ( cntact ) {
		// printf(" Ct=%d\n", cntact+1);
		// FFLUSH(stdout);
		// }
		cntact = 0;
	} else if (!expand)
		cntact++;
	switch (action) {
	case POWER_ON:
		printf("                                  :power on\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_POWER_ON_REQ;
		ctrl.len = SL_POWER_ON_REQ_SIZE;
		goto signal_iut_putmsg;
	case START:
		printf("                                  :start\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_START_REQ;
		ctrl.len = SL_START_REQ_SIZE;
		goto signal_iut_putmsg;
	case STOP:
		printf("                                  :stop\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_STOP_REQ;
		ctrl.len = SL_STOP_REQ_SIZE;
		goto signal_iut_putmsg;
	case LPO:
		printf("                                  :set lpo\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		ctrl.len = SL_LOCAL_PROC_OUTAGE_REQ_SIZE;
		goto signal_iut_putmsg;
	case LPR:
		printf("                                  :clear lpo\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_RESUME_REQ;
		ctrl.len = SL_RESUME_REQ_SIZE;
		goto signal_iut_putmsg;
	case CONG_A:
		printf("                                  :make congestion state\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		ctrl.len = SL_CONG_ACCEPT_REQ_SIZE;
		goto signal_iut_putmsg;
	case CONG_D:
		printf("                                  :make congestion state\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		ctrl.len = SL_CONG_DISCARD_REQ_SIZE;
		goto signal_iut_putmsg;
	case NO_CONG:
		printf("                                  :clear congestion state\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		ctrl.len = SL_NO_CONG_REQ_SIZE;
		goto signal_iut_putmsg;
	case CLEARB:
		printf("                                  :clear buffers\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		ctrl.len = SL_CLEAR_BUFFERS_REQ_SIZE;
		goto signal_iut_putmsg;
	case EMERG:
		printf("                                  :set emerg\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_EMERGENCY_REQ;
		ctrl.len = SL_EMERGENCY_REQ_SIZE;
		goto signal_iut_putmsg;
	case CEASE:
		printf("                                  :clear emerg\n");
		FFLUSH(stdout);
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		ctrl.len = SL_EMERGENCY_CEASES_REQ_SIZE;
	      signal_iut_putmsg:
		if ((ret = putmsg(iut_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
			printf("                                   ****ERROR: putmsg failed!\n");
			printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
			return FAILURE;
		}
		return SUCCESS;
	case COUNT:
		printf("                                   Ct=%d\n", count);
		FFLUSH(stdout);
		return SUCCESS;
	case TRIES:
		printf("                                   %d iterations\n", tries);
		FFLUSH(stdout);
		return SUCCESS;
	case ETC:
		printf("                                     .\n");
		printf("                                     .\n");
		printf("                                     .\n");
		FFLUSH(stdout);
		return SUCCESS;
	case SEND_MSU:
		if (!cntact || verbose) {
			printf("                                  :msu\n");
			FFLUSH(stdout);
		}
	case SEND_MSU_S:
		if (msu_len > BUFSIZE - 10)
			msu_len = BUFSIZE - 10;
		p->sl_primitive = SL_PDU_REQ;
		p->pdu_req.sl_mp = 0;
		ctrl.len = SL_PDU_REQ_SIZE;
		memset(dbuf, 'B', msu_len);
		data.len = msu_len;
		if ((ret = putmsg(iut_fd, NULL, &data, 0)) < 0) {
			printf("                                   ****ERROR: putmsg failed!\n");
			printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
			return FAILURE;
		}
		return SUCCESS;
	default:
		if (!cntact || verbose) {
			printf("                                  :????\n");
			FFLUSH(stdout);
		}
		return FAILURE;
	}
}

static int show_msus = 1;
static int show_fisus = 1;

static int
pt_decode_data(void)
{
	int ret;

	// printf("pt decode data:\n"); FFLUSH(stdout);
	// FFLUSH(stdout);
	iut_bib = pt_buf[0] & 0x80;
	iut_bsn = pt_buf[0] & 0x7f;
	iut_fib = pt_buf[1] & 0x80;
	iut_fsn = pt_buf[1] & 0x7f;
	iut_li = pt_buf[2] & 0x3f;
	iut_sio = pt_buf[3] & 0x7;
	pt_bsn = iut_fsn;
	switch (iut_li) {
	case 0:
		ret = FISU;
		break;
	case 1:
		ret = iut_sio = pt_buf[3] & 0x7;
		switch (iut_sio) {
		case SIO:
		case SIN:
		case SIE:
		case SIOS:
		case SIPO:
		case SIB:
			break;
		default:
			ret = SIX;
			break;
		}
		break;
	case 2:
		ret = iut_sio = pt_buf[4] & 0x7;
		switch (iut_sio) {
		case SIO:
		case SIN:
		case SIE:
		case SIOS:
		case SIPO:
		case SIB:
			break;
		default:
			ret = SIX2;
			break;
		}
		break;
	default:
		ret = MSU;
		break;
	}
	if (show_fisus || ret != FISU) {
		if (ret != oldret || oldisb != (((iut_bib | iut_bsn) << 8) | (iut_fib | iut_fsn))) {
			// if (oldisb == (((iut_bib | iut_bsn) << 8) | (iut_fib | iut_fsn)) &&
			// ((ret == FISU && oldret == MSU) || (ret == MSU && oldret == FISU))) {
			// if (ret == MSU && !expand)
			// cntmsg++;
			// } else
			// cntret = 0;
			oldret = ret;
			oldisb = ((iut_bib | iut_bsn) << 8) | (iut_fib | iut_fsn);
			// if (cntret) {
			// printf(" Ct=%d\n", cntret + 1);
			// FFLUSH(stdout);
			// }
			cntret = 0;
		} else if (!expand)
			cntret++;
	}
	if (!cntret || verbose) {
		char *label = NULL;

		switch (ret) {
		case FISU:
			if (show_fisus || verbose) {
				printf("                 <---------------- FISU (%02x/%02x)\n", iut_bib | iut_bsn, iut_fib | iut_fsn);
				FFLUSH(stdout);
			}
			break;
		case SIO:
			label = "SIO ";
			goto show_lssu;
		case SIN:
			label = "SIN ";
			goto show_lssu;
		case SIE:
			label = "SIE ";
			goto show_lssu;
		case SIOS:
			label = "SIOS";
			goto show_lssu;
		case SIPO:
			label = "SIPO";
			goto show_lssu;
		case SIB:
			label = "SIB ";
		      show_lssu:
			switch (iut_li) {
			case 1:
				printf("                 <---------------- %s (%02x/%02x)\n", label, iut_bib | iut_bsn, iut_fib | iut_fsn);
				break;
			case 2:
				printf("                 <---------------- %s (%02x/%02x)[%d]\n", label, iut_bib | iut_bsn, iut_fib | iut_fsn, iut_li);
				break;
			}
			FFLUSH(stdout);
			break;
		case SIX:
			printf("                 <-------(corrupt) LSSU (%02x/%02x)[%d]\n", iut_bib | iut_bsn, iut_fib | iut_fsn, iut_li);
			FFLUSH(stdout);
			break;
		case SIX2:
			printf("                 <-------(corrupt) LSSU (%02x/%02x)[%d]\n", iut_bib | iut_bsn, iut_fib | iut_fsn, iut_li);
			FFLUSH(stdout);
			break;
		case MSU:
			if (show_msus || verbose) {
				printf("                 <---------------- MSU  (%02x/%02x)[%d]\n", iut_bib | iut_bsn, iut_fib | iut_fsn, iut_len - 3);
				FFLUSH(stdout);
			}
			break;
		}
	}
	return ret;
}

static int
pt_decode_msg(unsigned char *buf)
{
	union SDT_primitives *p = (union SDT_primitives *) buf;

	if (verbose) {
		printf("pt decode msg:\n");
		FFLUSH(stdout);
		FFLUSH(stdout);
	}
	switch (p->sdt_primitive) {
	case SDT_RC_SIGNAL_UNIT_IND:
		return pt_decode_data();
	case SDT_IAC_CORRECT_SU_IND:
		printf("   !correct su\n");
		FFLUSH(stdout);
		break;
	case SDT_IAC_ABORT_PROVING_IND:
		printf("   !abort proving\n");
		FFLUSH(stdout);
		break;
	case SDT_LSC_LINK_FAILURE_IND:
		printf("   !link failure\n");
		FFLUSH(stdout);
		break;
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		if (verbose) {
			printf("   (tx wakeup)\n");
			FFLUSH(stdout);
		}
		break;
	case SDT_RC_CONGESTION_ACCEPT_IND:
		printf("   !congestion accept\n");
		FFLUSH(stdout);
		break;
	case SDT_RC_CONGESTION_DISCARD_IND:
		printf("   !congestion discard\n");
		FFLUSH(stdout);
		break;
	case SDT_RC_NO_CONGESTION_IND:
		printf("   !no congestion\n");
		FFLUSH(stdout);
		break;
	case LMI_INFO_ACK:
		printf("   !(info ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_OK_ACK:
		printf("   !(ok ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_ERROR_ACK:
		printf("   !(error ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_ENABLE_CON:
		printf("   !(enable con)\n");
		FFLUSH(stdout);
		break;
	case LMI_DISABLE_CON:
		printf("   !(disable con)\n");
		FFLUSH(stdout);
		break;
	case LMI_ERROR_IND:
		printf("   !(error ind)\n");
		FFLUSH(stdout);
		break;
	case LMI_STATS_IND:
		printf("   !(stats ind)\n");
		FFLUSH(stdout);
		break;
	case LMI_EVENT_IND:
		printf("   !(event ind)\n");
		FFLUSH(stdout);
		break;
	default:
		printf("   !(unknown %ld)\n", p->sdt_primitive);
		FFLUSH(stdout);
		break;
	}
	return UNKNOWN;
}

static int
iut_decode_data(void)
{
	printf("                                  !msu [%d bytes]\n", iut_len);
	if (verbose) {
		int i;

		printf("                                 ");
		for (i = 0; i < iut_len; i++)
			printf(" %02x", iut_buf[i]);
		printf("\n");
	}
	FFLUSH(stdout);
	return IUT_MSU;
}

static int
iut_decode_msg(unsigned char *buf)
{
	char *reason;
	union SL_primitives *p = (union SL_primitives *) buf;

	if (p->sl_primitive != oldprm) {
		oldprm = p->sl_primitive;
		cntprm = 0;
	} else if (!expand)
		cntprm++;
	switch (p->sl_primitive) {
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		if (!cntprm || verbose) {
			printf("                                  !rpo\n");
			FFLUSH(stdout);
		}
		return RPO;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		if (!cntprm || verbose) {
			printf("                                  !rpr\n");
			FFLUSH(stdout);
		}
		return RPR;
	case SL_IN_SERVICE_IND:
		printf("                                  !in service\n");
		FFLUSH(stdout);
		return IN_SERVICE;
	case SL_OUT_OF_SERVICE_IND:
		switch (p->out_of_service_ind.sl_reason) {
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
		printf("                                  !out of service (%s)\n", reason);
		FFLUSH(stdout);
		return OUT_OF_SERVICE;
	case SL_PDU_IND:
		printf("                                  !pdu\n");
		printf("                                  !msu [%d bytes]\n", iut_len);
		FFLUSH(stdout);
		break;
	case SL_LINK_CONGESTED_IND:
		printf("                                  !congested\n");
		FFLUSH(stdout);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		printf("                                  !congestion ceased\n");
		FFLUSH(stdout);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		printf("                                  !retrieved msg\n");
		FFLUSH(stdout);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		printf("                                  !retrieval complete\n");
		FFLUSH(stdout);
		break;
	case SL_RB_CLEARED_IND:
		printf("                                  !rb cleared\n");
		FFLUSH(stdout);
		break;
	case SL_BSNT_IND:
		printf("                                  !bsnt\n");
		FFLUSH(stdout);
		break;
	case SL_RTB_CLEARED_IND:
		printf("                                  !rtb cleared\n");
		FFLUSH(stdout);
		break;
	case LMI_INFO_ACK:
		printf("                                  !(info ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_OK_ACK:
		printf("                                  !(ok ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_ERROR_ACK:
		printf("                                  !(error ack)\n");
		FFLUSH(stdout);
		break;
	case LMI_ENABLE_CON:
		printf("                                  !(enable con)\n");
		FFLUSH(stdout);
		break;
	case LMI_DISABLE_CON:
		printf("                                  !(disable con)\n");
		FFLUSH(stdout);
		break;
	case LMI_ERROR_IND:
		printf("                                  !(error ind)\n");
		FFLUSH(stdout);
		break;
	case LMI_STATS_IND:
		printf("                                  !(stats ind)\n");
		FFLUSH(stdout);
		break;
	case LMI_EVENT_IND:
		printf("                                  !(event ind)\n");
		FFLUSH(stdout);
		break;
	default:
		printf("                                  !(unknown %ld)\n", p->sl_primitive);
		FFLUSH(stdout);
		break;
	}
	return UNKNOWN;
}

static int show_timeout = 0;
static int pt_event = 0;

static int
time_event(int event)
{
	if (verbose) {
		unsigned long msec;
		struct timeval now;

		gettimeofday(&now, NULL);
		if (!test_start)
			test_start = now.tv_sec;
		msec = (now.tv_sec - test_start) * 1000;
		msec += (now.tv_usec + 500UL) / 1000;
		printf("time: %lu msec\n", msec);
		FFLUSH(stdout);
	}
	return (event);
}

static int
wait_event(int wait)
{
	while (1) {
		struct pollfd pfd[] = {
			{pt_fd, POLLIN | POLLPRI, 0},
			{iut_fd, POLLIN | POLLPRI, 0}
		};

		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose) {
				printf("                 |----timeout----|\n");
				FFLUSH(stdout);
				show_timeout--;
			}
			pt_event = TIMEOUT;
			return time_event(TIMEOUT);
		}
		// printf("polling:\n");
		// FFLUSH(stdout);
		pfd[0].fd = pt_fd;
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		pfd[1].fd = iut_fd;
		pfd[1].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[1].revents = 0;
		switch (poll(pfd, 2, wait)) {
		case -1:
			// printf(" = = = ERROR = = =\n");
			// FFLUSH(stdout);
			break;
		case 0:
			// printf(" < + + + + + + + + (nothing)\n");
			// FFLUSH(stdout);
			pt_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
		case 2:
			// printf("polled:\n");
			// FFLUSH(stdout);
			if (pfd[0].revents) {
				int flags = 0;
				unsigned char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, (char *) cbuf };
				struct strbuf data = { BUFSIZE, 0, (char *) pt_buf };

				// printf("getmsg from pt:\n");
				// FFLUSH(stdout);
				if (getmsg(pt_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from pt [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (data.len > 0)
						iut_len = data.len;
					if (ctrl.len > 0) {
						if ((pt_event = pt_decode_msg((unsigned char *) ctrl.buf)) != UNKNOWN)
							return time_event(pt_event);
					} else if (data.len > 0) {
						if ((pt_event = pt_decode_data()) != UNKNOWN)
							return time_event(pt_event);
					}
				}
			}
			if (pfd[1].revents) {
				int flags = 0;
				unsigned char cbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, (char *) cbuf };
				struct strbuf data = { BUFSIZE, 0, (char *) iut_buf };

				// printf("getmsg from iut:\n");
				// FFLUSH(stdout);
				if (getmsg(iut_fd, &ctrl, &data, &flags) == 0) {
					// printf("gotmsg from iut [%d,%d]:\n",ctrl.len,data.len);
					// FFLUSH(stdout);
					if (data.len > 0)
						iut_len = data.len;
					if (ctrl.len > 0) {
						if ((pt_event = iut_decode_msg((unsigned char *) ctrl.buf)) != UNKNOWN)
							return time_event(pt_event);
					} else if (data.len > 0) {
						if ((pt_event = iut_decode_data()) != UNKNOWN)
							return time_event(pt_event);
					}
				}
			}
		default:
			break;
		}
	}
}

static int
event(void)
{
	return wait_event(-1);
}

static int
check_snibs(unsigned char bsnib, unsigned char fsnib)
{
	int ret = FAILURE;

	if ((iut_bib | iut_bsn) == bsnib && (iut_fib | iut_fsn) == fsnib)
		ret = SUCCESS;
	printf("                 check b/f sn/ib:  ---> (%02x/%02x)\n", bsnib, fsnib);
	FFLUSH(stdout);
	return ret;
}

static int
test_1_1a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(SIOS);
			signal(POWER_ON);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case SIOS:
				return check_snibs(0xff, 0xff);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

#if 0
static int
test_1_1b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(POWER_ON);
				send(SIOS);
				return check_snibs(0xff, 0xff);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_1_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIOS);
				start_tt(iutconf.sl.t2 * 10 + 200);
				beg_time = milliseconds("T2");
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case OUT_OF_SERVICE:
				break;
			case SIO:
				send(SIOS);
				break;
			case SIOS:
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

static int
test_1_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIO);
				start_tt(iutconf.sl.t3 * 10 + 200);
				beg_time = milliseconds("T3");
				state = 4;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 4:
			switch (event()) {
			case SIN:
				send(SIO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			state = 1;
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				beg_time = milliseconds("T4(Pn)");
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				if (check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi))
					return FAILURE;
				send(SIN);
				start_tt(iutconf.sl.t1 * 10 + 200);
				beg_time = milliseconds("T1");
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case FISU:
				send(SIN);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_5a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_1_5b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS2);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS2);
				break;
			case SIO:
				send(SIO2);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO2);
				break;
			case SIN:
				send(SIN2);
				break;
			case FISU:
				send(FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_1_6(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(MSU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
			case IUT_MSU:
			case FISU:
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

static int
test_1_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 / 2);
				show_timeout = 1;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				start_tt(iutconf.sl.t4n * 10 + 200);
				send(SIO);
				send(SIN);
				beg_time = milliseconds("T4(Pn)");
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_1_8a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;
			case SIO:
				send(SIO);
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;
			case SIPO:
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
}
static int
test_1_8b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(SIPO);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPO:
				break;
			case FISU:
				send(SIPO);
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

static int
test_1_9a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case SIPO:
				start_tt(1000);
				send(MSU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;
			case SIPO:
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
}
static int
test_1_9b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
			case SIN:
				signal(SEND_MSU);
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPO:
				break;
			case FISU:
				start_tt(1000);
				send(SIPO);
				break;
			case SIN:
				send(SIN);
				break;
			case MSU:
				send(SIPO);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case RPO:
				break;
			case MSU:
			case FISU:
				send(SIPO);
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

static int
test_1_10(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				start_tt(1000);
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_1_11(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(SIPO);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPO:
				break;
			case SIPO:
				send(SIPO);
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

static int
test_1_12a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(STOP);
				start_tt(1000);
				send(SIOS);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIPO:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(SIOS);
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
static int
test_1_12b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case FISU:
				signal(STOP);
				send(FISU);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(SIOS);
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

static int
test_1_13(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case SIPO:
				start_tt(1000);
				send(SIO);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIPO:
				send(SIO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(SIOS);
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

static int
test_1_14(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				signal(LPO);
				start_tt(iutconf.sl.t4n * 10 / 2);
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				signal(LPR);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
			case SIN:
				send(SIN);
				break;
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
}

static int
test_1_15(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				signal(LPO);
				start_tt(1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(FISU);
				start_tt(5000);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
				send(FISU);
				break;
			case TIMEOUT:
				signal(LPR);
				send(FISU);
				start_tt(5000);
				state = 6;
				break;
			default:
				return FAILURE;
			}
			break;
		case 6:
			switch (event()) {
			case SIPO:
				send(FISU);
				break;
			case FISU:
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

static int
test_1_16(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(SIN);
				start_tt(iutconf.sl.t1 * 10 + 200);
				beg_time = milliseconds("T1");
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IN_SERVICE:
				break;
			case SIPO:
				send(SIN);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				return check_time("T1  ", beg_time, timer[t1].lo, timer[t1].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
	return FAILURE;
}

static int
test_1_17(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				beg_time = milliseconds("T3+T4(Pn)");
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				return check_time("T3,4", beg_time, timer[t4n].lo, timer[t3].hi + timer[t4n].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_1_18(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
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
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				beg_time = milliseconds("T4(Pn)");
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				return check_time("T4  ", beg_time, timer[t4n].lo, timer[t4n].hi);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_1_19(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				signal(EMERG);
				send(SIO);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIE:
				send(SIE);
				beg_time = milliseconds("T4(Pe)");
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIE:
				send(SIE);
				break;
			case FISU:
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

static int
test_1_20(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				signal(EMERG);
				send(SIO);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIO);
				break;
			case SIE:
				send(SIN);
				beg_time = milliseconds("T4(Pn)");
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIE:
				send(SIN);
				break;
			case FISU:
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

static int
test_1_21(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(EMERG);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIE:
				beg_time = milliseconds("T4(Pe)");
				send(SIE);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIE:
				send(SIE);
				break;
			case FISU:
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

static int
test_1_22(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				send(SIO);
				signal(START);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIE);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				beg_time = milliseconds("T4(Pe)");
				send(SIE);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIE);
				break;
			case FISU:
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

static int
test_1_23(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				signal(EMERG);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case SIE:
				beg_time = milliseconds("T4(Pe)");
				send(SIN);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIE:
				send(SIN);
				break;
			case FISU:
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

static int
test_1_24(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(EMERG);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIE);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIE:
				beg_time = milliseconds("T4(Pe)");
				send(SIE);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIE:
				send(SIE);
				break;
			case FISU:
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

static int
test_1_25(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIOS);
				show_timeout = 1;
				start_tt(iutconf.sl.t2 * 10 - 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIOS:
			case SIO:
				send(SIOS);
				break;
			case TIMEOUT:
				signal(STOP);
				start_tt(1000);
				state = 3;
				break;
			case OUT_OF_SERVICE:
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIO:
			case SIN:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_26(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t3 * 8);	/* 80% of T3 */
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIN:
			case SIO:
				send(SIO);
				break;
			case TIMEOUT:
				signal(STOP);
				start_tt(iutconf.sl.t4n * 10);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_27(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case SIPO:
				signal(STOP);
				send(SIN);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIPO:
				send(SIN);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_28(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(SIO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(SIO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_29a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(STOP);
			send(SIOS);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_29b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(STOP);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(SIOS);
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

static int
test_1_30a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPO:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(FISU);
				signal(STOP);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIPO:
				send(FISU);
				break;
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_30b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(LPO);
			send(SIPO);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case NO_MSG:
			case FISU:
				send(STOP);
				send(SIOS);
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				return SUCCESS;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_1_31a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(SIPO);
			signal(STOP);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPO:
				break;
			case FISU:
				send(SIPO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_31b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPO:
				break;
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(STOP);
				send(SIOS);
				return SUCCESS;
			case RPR:
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

static int
test_1_32a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 / 2);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				send(STOP);
				send(SIOS);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIN:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_32b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(SIOS);
			send(START);
			signal(START);
			send(SIO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIO);
				break;
			case SIO:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIN);
				break;
			case SIN:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 / 2);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				signal(STOP);
				start_tt(iutconf.sl.t4n * 10 / 2 + 1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(SIOS);
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

static int
test_1_33(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(SIO);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case FISU:
				send(SIO);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_34(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(STOP);
				send(SIOS);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case FISU:
				send(SIOS);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_1_35(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(LPO);
				send(SIPO);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case RPO:
				break;
			case FISU:
				send(SIPO);
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

static int
test_2_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				send(SIO);
				send(SIN);
				send(SIE);
				send(SIPO);
				send(SIB);
				send(SIX);
				send(SIX2);
				send(FISU);
				send(MSU);
				signal(STOP);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				oldmsg = 0;
				cntmsg = 0;	/* force display SIOS */
				send(SIOS);
				send(SIPO);
				send(SIB);
				send(SIX);
				send(FISU);
				send(MSU);
				signal(CEASE);
				signal(START);
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				oldmsg = 0;
				cntmsg = 0;	/* force display SIO */
				send(SIO);
				send(SIPO);
				send(SIB);
				send(SIX);
				send(FISU);
				send(MSU);
				signal(CEASE);
				signal(START);
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				send(SIPO);
				send(SIB);
				send(SIX);
				send(FISU);
				send(MSU);
				signal(CEASE);
				signal(START);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_5(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(SIB);
				send(SIX);
				signal(EMERG);
				signal(CEASE);
				signal(LPR);
				signal(START);
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_6(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(SIB);
				send(SIX);
				signal(EMERG);
				signal(CEASE);
				// signal(LPR);
				// spec says LPR, but it doesn't make sense...
				signal(LPO);
				signal(START);
				send(FISU);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case IN_SERVICE:
			case RPR:
				break;
			case SIPO:
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
}

static int
test_2_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(SIX);
			signal(EMERG);
			signal(CEASE);
			signal(LPR);
			signal(START);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IN_SERVICE:
				break;
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
}

static int
test_2_8(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(SIB);
				send(SIX);
				signal(EMERG);
				signal(CEASE);
				signal(START);
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
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
}

static int
test_3_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case FISU:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU_BAD_FIB);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(FISU_BAD_FIB);
				start_tt(1000);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case SIPO:
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIPO:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(LPO);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case SIPO:
				send(FISU_BAD_FIB);
				start_tt(1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (wait_event(0)) {
			case SIPO:
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(FISU_BAD_FIB);
				start_tt(1000);
				state = 5;
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case SIPO:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_5(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU);
			if (send(TX_BREAK))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_6(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU_BAD_FIB);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(FISU_BAD_FIB);
				start_tt(1000);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				send(FISU);
				break;
			case SIPO:
				if (send(TX_BREAK))
					return INCONCLUSIVE;
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_3_8(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(FISU_BAD_FIB);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (wait_event(0)) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
			case NO_MSG:
				oldmsg = 0;
				cntmsg = 0;	/* force display */
				send(FISU_BAD_FIB);
				start_tt(1000);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_4_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IN_SERVICE:
				break;
			case MSU:
				pt_fsn = pt_bsn = 0x7f;
				if (++count == 2) {
					signal(LPO);
					pt_bsn = 0x00;
					send(MSU);
					start_tt(iutconf.sl.t7 * 10 / 2);
					state = 2;
					break;
				}
				send(FISU);
				break;
			case FISU:
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				pt_bsn = 0x00;
				send(FISU);
				break;
			case SIPO:
				pt_bsn = 0x00;
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPR:
				break;
			case SIPO:
				pt_bsn = 0x00;
				send(FISU);
				break;
			case TIMEOUT:
				signal(CLEARB);
				signal(SEND_MSU);
				start_tt(1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case RPR:
				break;
			case SIPO:
				if (iut_fsn != 0x01) {
					if (check_snibs(0xff, 0x80))
						return FAILURE;
					send(FISU);
					signal(LPR);
					state = 5;
					break;
				}
				pt_bsn = 0x00;
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case SIPO:
				send(FISU);
				break;
			case FISU:
				send(FISU);
				break;
			case MSU:
				return check_snibs(0xff, 0x81);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_4_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case RPR:
				break;	/* stupid ITU-T SDLs */
			case FISU:
				send(FISU);
				break;
			case SIPO:
				send(SIPO);
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case RPR:
			case RPO:
				break;
			case SIPO:
				send(LPR);
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case RPO:
				break;
			case SIPO:
				send(FISU);
				break;
			case RPR:
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

static int
test_4_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(LPO);
			send(LPO);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case RPO:
			case RPR:
				break;
			case NO_MSG:
			case FISU:
				send(SIPO);
				break;
			case SIPO:
				send(SIPO);
				signal(LPR);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (wait_event(0)) {
			case RPO:
			case RPR:
				break;
			case NO_MSG:
			case SIPO:
				send(SIPO);
				break;
			case FISU:
				send(LPR);
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (wait_event(0)) {
			case RPO:
			case RPR:
				break;
			case NO_MSG:
			case FISU:
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

static int
test_5_1(void)
{
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
			switch (event()) {
			case IN_SERVICE:
				break;
			case FISU:
				if (iut_bsn != old_bsn)
					return FAILURE;
				send(FISU);
				break;
			case TIMEOUT:
				return SUCCESS;
			case IUT_MSU:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_5_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(MSU_TOO_LONG);
			send(FISU);
			start_tt(2000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IN_SERVICE:
				break;
			case FISU:
				if (iut_bsn != 0x7f)
					return check_snibs(0x7f, 0xff);
				send(FISU);
				break;
			case TIMEOUT:
				return check_snibs(0x7f, 0xff);
			case IUT_MSU:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_5_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(MSU_TOO_SHORT);
			send(FISU);
			start_tt(2000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IN_SERVICE:
				break;
			case FISU:
				if (iut_bsn != 0x7f)
					return check_snibs(0x7f, 0xff);
				send(FISU);
				break;
			case TIMEOUT:
				return check_snibs(0x7f, 0xff);
			case IUT_MSU:
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_5_4a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(FISU_FISU_1FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch (event()) {
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
}

static int
test_5_4b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(FISU_FISU_2FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch (event()) {
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
}

static int
test_5_5a(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(MSU_MSU_1FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
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
}

static int
test_5_5b(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(MSU_MSU_2FLAG))
				return INCONCLUSIVE;
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
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
}

#if 0
static int
test_6_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU_CORRUPT);
			stop_tt();
			count = 255;
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
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
}

static int
test_6_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			stop_tt();
			count = 253;
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
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
			case SIOS:
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
}
#endif

static int
test_6_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			count++;
			send(FISU_CORRUPT);
			for (; count < iutconf.sdt.T + 1; count++)
				send(FISU_CORRUPT_S);
			start_tt(2000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				count++;
				send(FISU_CORRUPT);
				if (count > 128) {
					send(COUNT);
					return FAILURE;
				}
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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
}

static int
test_6_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			if (send(TX_BREAK))
				return INCONCLUSIVE;
			start_tt(100);
			state = 1;
			break;
		case 1:
			switch (event()) {
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
			switch (event()) {
			case IN_SERVICE:
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
}

static int
test_7_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				beg_time = milliseconds("T4");
				start_tt(iutconf.sl.t4n * 10 / 2);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (wait_event(0)) {
			case NO_MSG:
			case SIN:
				if (count < iutconf.sdt.Tin - 1) {
					send(LSSU_CORRUPT);
					count++;
				} else {
					send(COUNT);
					send(SIN);
					start_tt(iutconf.sl.t4n * 10 / 2 + 200);
					state = 5;
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case FISU:
				send(FISU);
				return SUCCESS;
			case TIMEOUT:
				return FAILURE;
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_7_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 / 2);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (wait_event(0)) {
			case NO_MSG:
			case SIN:
				if (count < iutconf.sdt.Tin) {
					send(LSSU_CORRUPT);
					count++;
				} else {
					send(COUNT);
					send(SIN);
					beg_time = milliseconds("T4");
					start_tt(iutconf.sl.t4n * 10 + 200);
					state = 5;
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case IN_SERVICE:
				break;
			case FISU:
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

static int
test_7_3(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				start_tt(iutconf.sl.t4n * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIN);
				start_tt(iutconf.sl.t4n * 10 / 2);
				tries = 1;
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case TIMEOUT:
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (wait_event(0)) {
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				send(COUNT);
				if (tries == iutconf.sl.M)
					return SUCCESS;
				return FAILURE;
			case NO_MSG:
			case SIN:
				if (count <= iutconf.sdt.Tin) {
					send(LSSU_CORRUPT);
					count++;
				} else {
					send(COUNT);
					count = 0;
					send(SIN);
					if (tries < iutconf.sl.M) {
						start_tt(iutconf.sl.t4n * 10 / 2);
						state = 3;
						tries++;
					} else {
						start_tt(iutconf.sl.t4n * 10 + 200);
						state = 5;
					}
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case SIN:
				send(SIN);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_7_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
				send(SIO);
				break;
			case SIN:
				send(SIE);
				start_tt(iutconf.sl.t4e * 10 / 2);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIE);
				break;
			case TIMEOUT:
				send(LSSU_CORRUPT);
				for (count = 1; count < iutconf.sdt.Tie; count++)
					send(LSSU_CORRUPT_S);
				send(COUNT);
				send(SIE);
				beg_time = milliseconds("T4(Pe)");
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case SIN:
				send(SIE);
				break;
			case IN_SERVICE:
				break;
			case FISU:
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

static int
test_8_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				send(MSU);
				start_tt(5000);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x80, 0xff))
						return FAILURE;
					send(FISU);
					signal(SEND_MSU);
					state = 2;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if (check_snibs(0x80, 0xff))
					return FAILURE;
				send(FISU);
				break;
			case MSU:
				if (check_snibs(0x80, 0x80))
					return FAILURE;
				send(FISU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				return check_snibs(0x80, 0x80);
			default:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;

		}
	}
}

static int
test_8_2(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case MSU:
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
			switch (event()) {
			case MSU:
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
			switch (event()) {
			case MSU:
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
			switch (event()) {
			case MSU:
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
}

static int
test_8_3(void)
{
	if (msu_len > 12)
		msu_len = 12;
	for (;;) {
		int i;
		int n = iutconf.sl.N1;

		stop_tt();
		switch (state) {
		case 0:
			for (i = 0; i < n; i++)
				signal(SEND_MSU);
			show_fisus = 0;
			show_timeout = 1;
			start_tt(iutconf.sl.t7 * 10 / 2 - 100);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case MSU:
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 2)
					signal(ETC);
				if (iut_fsn != n - 1) {
					count++;
					pt_bsn = 0x7f;
					send(FISU);
					break;
				}
				count++;
			case TIMEOUT:
				signal(COUNT);
				count = 0;
				pt_bsn = pt_fsn = 0x7f;
				pt_bib = 0x00;
				pt_fib = 0x80;
				send(FISU);
				start_tt(iutconf.sl.t7 * 10 / 2 + 200);
				show_timeout = 1;
				show_fisus = 1;
				oldisb = 0;
				state = 2;
				break;
			default:
				signal(COUNT);
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case MSU:
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 2)
					signal(ETC);
				if (iut_fsn != n - 1) {
					count++;
					pt_bsn = 0x7f;
					send(FISU);
					break;
				}
				count++;
				signal(COUNT);
				send(FISU);
				return SUCCESS;
			default:
				signal(COUNT);
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_8_4(void)
{
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			pt_fib = 0x00;
			send(MSU);
			pt_fib = 0x80;
			send(FISU);
			oldmsg = 0;
			cntmsg = 0;
			send(FISU);
			start_tt(iutconf.sl.t7 * 10 + 200);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x7f;
					send(MSU);
					send(FISU);
					start_tt(iutconf.sl.t7 * 10 + 200);
					state = 2;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case IUT_MSU:
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
}

static int
test_8_5(void)
{
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			send(MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x80, 0xff))
						return FAILURE;
					pt_fsn = 0x7f;
					send(MSU);
					pt_fsn = 0x01;
					send(FISU);
					state = 2;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0x80 || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x00, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x00;
					send(MSU);
					state = 3;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0x00 || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x01, 0xff))
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
}

static int
test_8_6(void)
{
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			pt_fib = 0x00;
			send(MSU);
			pt_fib = 0x80;
			send(FISU);
			oldmsg = 0;
			cntmsg = 0;
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x7f;
					oldmsg = 0;
					cntmsg = 0;
					send(MSU);
					send(FISU);
					state = 2;
					break;
				}
				send(FISU);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case IUT_MSU:
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
}

static int
test_8_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
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
			switch (wait_event(0)) {
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
			switch (wait_event(0)) {
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
			switch (event()) {
			case FISU:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_8_8(void)
{
	for (;;) {
		switch (state) {
		case 0:
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
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
			switch (wait_event(0)) {
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
			switch (event()) {
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
}

static int
test_8_9(void)
{
	for (;;) {
		switch (state) {
		case 0:
			pt_fib = 0x00;
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				send(SIPO);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (wait_event(0)) {
			case RPO:
			case RPR:
				break;
			case FISU:
			case NO_MSG:
				pt_fib = 0x80;
				send(MSU);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (wait_event(0)) {
			case RPO:
			case RPR:
				break;
			case FISU:
			case NO_MSG:
				send(FISU);
				send(FISU);
				start_tt(1000);
				state = 4;
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
			case RPO:
			case RPR:
				break;
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x7f;
					send(MSU);
					state = 5;
				} else {
					send(FISU);
				}
				break;
			default:
				return FAILURE;
			}
			break;
		case 5:
			switch (event()) {
			case RPO:
			case RPR:
				break;
			case IUT_MSU:
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
}

static int
test_8_10(void)
{
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				pt_bsn = 0x3f;
				send(MSU);
				pt_bsn = 0x7f;
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (wait_event(0)) {
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
			switch (event()) {
			case FISU:
				if ((iut_bib | iut_bsn) != 0xff || (iut_fib | iut_fsn) != 0xff) {
					if (check_snibs(0x7f, 0xff))
						return FAILURE;
					pt_fib = 0x00;
					pt_fsn = 0x7f;
					send(MSU);
					send(FISU);
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
			switch (event()) {
			case IUT_MSU:
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
}

static int
test_8_11(void)
{
	for (;;) {
		switch (state) {
		case 0:
			wait_event(0);
			send(FISU);
			state = 1;
			break;
		case 1:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				pt_bsn = 0x3f;
				send(FISU);
				oldmsg = 0;
				cntmsg = 0;
				send(FISU);
				pt_bsn = 0x7f;
				send(FISU);
				send(FISU);
				state = 2;
				start_tt(1000);
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_8_12(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (wait_event(0)) {
			case FISU:
			case NO_MSG:
				send(FISU);
				signal(SEND_MSU);
				state = 1;
				break;
			default:
				return FAILURE;
			}
			break;
		case 1:
			switch (event()) {
			case FISU:
				pt_bsn = pt_fsn = 0x7f;
				send(FISU);
				break;
			case MSU:
				start_tt(iutconf.sl.t7 * 10 + 200);
				beg_time = milliseconds("T7");
				pt_bsn = pt_fsn = 0x7f;
				send(FISU);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				pt_bsn = pt_fsn = 0x7f;
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_8_13(void)
{
	for (;;) {
		switch (state) {
		case 0:
			switch (wait_event(0)) {
			case FISU:
				send(FISU);
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
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_9_1(void)
{
	if (msu_len > 12)
		msu_len = 12;
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			milliseconds("T7");
			state = 1;
			break;
		case 1:
			switch (event()) {
			case NO_MSG:
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case MSU:
				if (count < 4) {
					cntret = -1;
					pt_bsn = 0x7f;
					send(FISU);
					count++;
					break;
				}
				send(FISU);
				send(FISU);
				state = 2;
				break;
			default:
				check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
				send(FISU);
				break;
			case NO_MSG:
			case FISU:
				if (check_snibs(0xff, 0x80))
					return FAILURE;
				send(MSU);
				state = 3;
				break;
			default:
				check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IUT_MSU:
				break;
			case NO_MSG:
			case FISU:
				if ((iut_bsn | iut_bib) != 0xff || (iut_fsn | iut_fib) != 0x80)
					return check_snibs(0x80, 0x80);
				send(FISU);
				break;
			default:
				check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_2(void)
{
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
			switch (event()) {
			case FISU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case FISU:
				if (iut_fsn == 2)
					return SUCCESS;
				return FAILURE;
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_3(void)
{
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;

	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3)
		return INCONCLUSIVE;
	if (msu_len > 12)
		msu_len = 12;
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
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_4(void)
{
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1 - 1;

	msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
	n = iutconf.sl.N2 / (msu_len + h) + 1;
	if (msu_len > iutconf.sdt.m)
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
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = 0x7f;
				send(FISU);
				count = 0;
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_5(void)
{
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;

	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3) {
		n = iutconf.sl.N1 - 1;
		msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
		n = iutconf.sl.N2 / (msu_len + h) + 1;
		if (msu_len > iutconf.sdt.m)
			return INCONCLUSIVE;
	}
	if (msu_len > 12)
		msu_len = 12;
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
			switch (event()) {
			case MSU:
				count = 0;
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
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
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case MSU:
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
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_6(void)
{
	int i;
	int h = (iutconf.opt.popt & SS7_POPT_HSL) ? 6 : 3;
	int n = iutconf.sl.N1;

	msu_len = iutconf.sl.N2 / iutconf.sl.N1 - h - 1;
	if (msu_len < 3) {
		n = iutconf.sl.N1 - 1;
		msu_len = iutconf.sl.N2 / (iutconf.sl.N1 - 1) - h + 1;
		n = iutconf.sl.N2 / (msu_len + h) + 1;
		if (msu_len > iutconf.sdt.m)
			return INCONCLUSIVE;
	}
	if (msu_len > 12)
		msu_len = 12;
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
			switch (event()) {
			case MSU:
				count = 0;
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
				if (iut_fsn == n)
					return FAILURE;
				if (iut_fsn == n - 1)
					count++;
				if (iut_fsn > 1 && iut_fsn < n - 3)
					oldisb++;
				if (iut_fsn == 3 && count == 2) {
					pt_bsn = n - 1;
					return SUCCESS;
				}
				if (iut_fsn == 2)
					signal(ETC);
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_7(void)
{
	for (;;) {
		switch (state) {
		case 0:
			start_tt(5000);
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = 0x7f;
				send(LPO);
				send(SIPO);
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case RPO:
			case RPR:
				break;
			case MSU:
				pt_bsn = 0x7f;
				send(SIPO);
				break;
			case FISU:
				if (!count++)
					if (check_snibs(0xff, 0x80))
						return FAILURE;
				pt_bsn = 0x7f;
				send(SIPO);
				break;
			case TIMEOUT:
				signal(CLEARB);
				send(LPR);
				pt_bsn = 0x7f;
				send(MSU);
				start_tt(500);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case IUT_MSU:
			case RPO:
			case RPR:
				break;
			case FISU:
				if (iut_bsn != 0) {
					pt_bsn = 0x7f;
					pt_fsn = 0x7f;
					send(MSU);
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
}

static int
test_9_8(void)
{
	for (;;) {
		switch (state) {
		case 0:
			pt_bsn = 0x00;
			pt_fsn = 0x7f;
			send(MSU);
			oldmsg = 0;
			cntmsg = 0;
			pt_bsn = 0x7f;
			pt_fsn = 0x7f;
			send(MSU);
			oldmsg = 0;
			cntmsg = 0;
			pt_bsn = 0x7f;
			pt_fsn = 0x7f;
			send(MSU);
			count = 2;
			send(COUNT);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
				break;
			case FISU:
				if (iut_bsn == 0)
					return SUCCESS;
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
}

static int
test_9_9(void)
{
	for (;;) {
		switch (state) {
		case 0:
			pt_bsn = 0x7e;
			pt_fsn = 0x7f;
			send(MSU);
			oldmsg = 0;
			cntmsg = 0;
			pt_bsn = 0x7f;
			pt_fsn = 0x7f;
			send(MSU);
			oldmsg = 0;
			cntmsg = 0;
			pt_bsn = 0x7e;
			pt_fsn = 0x7f;
			send(MSU);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case OUT_OF_SERVICE:
				break;
			case IUT_MSU:
				return FAILURE;
			case FISU:
				oldmsg = 0;
				cntmsg = 0;
				pt_bsn = 0x7f;
				pt_fsn = 0x7f;
				send(MSU);
				break;
			case SIOS:
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

static int
test_9_10(void)
{
	for (;;) {
		switch (state) {
		case 0:
			pt_fsn = 0x7f;
			send(MSU);
			pt_fsn = 0x01;
			oldmsg = 0;
			cntmsg = 0;
			send(MSU);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case IUT_MSU:
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
				send(MSU);
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

static int
test_9_11(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = 0x7f;
				send(FISU);
				beg_time = milliseconds("T7");
				start_tt(iutconf.sl.t7 * 10 + 200);
				state = 2;
				break;
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case MSU:
				pt_bsn = 0x7f;
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				return check_time("T7  ", beg_time, timer[t7].lo, timer[t7].hi);
			default:
				break;
			case TIMEOUT:
				return FAILURE;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_12(void)
{
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
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case TIMEOUT:
				return check_snibs(0xff, 0xff);
			default:
				break;
			}
			break;
		default:
			return SCRIPTERROR;
		}
	}
}

static int
test_9_13(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(STOP);
			start_tt(1000);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
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

static int
test_10_1(void)
{
	for (;;) {
		switch (state) {
		case 0:
			signal(CONG_D);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case SIB:
				send(FISU);
				beg_time = milliseconds("T5");
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case SIB:
				send(FISU);
				if (check_time("T5  ", beg_time, timer[t5].lo, timer[t5].hi))
					return FAILURE;
				signal(NO_CONG);
				start_tt(iutconf.sl.t5 * 10 + 200);
				state = 3;
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case SIB:
				return FAILURE;
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

static int
test_10_2(void)
{
	int n = (iutconf.sl.t6 - iutconf.sl.t7) / iutconf.sl.t5;

	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = pt_fsn = 0x7f;
				start_tt(iutconf.sl.t5 * 10);
				send(SIB);
				send(FISU_S);
				beg_time = milliseconds("T6");
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case TIMEOUT:
				send(SIB_S);
				count++;
				if (count == n) {
					send(COUNT);
					start_tt(iutconf.sl.t7 * 10 - 100);
					state = 3;
					break;
				}
				start_tt(iutconf.sl.t5 * 10);
			case FISU:
				pt_bsn = pt_fsn = 0x7f;
				send(FISU_S);
				break;
			default:
				return FAILURE;
			}
			break;
		case 3:
			switch (event()) {
			case FISU:
				pt_bsn = pt_fsn = 0x7f;
				send(FISU);
				break;
			case TIMEOUT:
				pt_bsn = 0x00;
				send(FISU);
				state = 4;
				start_tt(1000);
				break;
			default:
				return FAILURE;
			}
			break;
		case 4:
			switch (event()) {
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
}

static int
test_10_3(void)
{
	int n = (iutconf.sl.t6 + 20) / iutconf.sl.t5;

	for (;;) {
		switch (state) {
		case 0:
			signal(SEND_MSU);
			state = 1;
			break;
		case 1:
			switch (event()) {
			case FISU:
				send(FISU);
				break;
			case MSU:
				pt_bsn = pt_fsn = 0x7f;
				start_tt(iutconf.sl.t5 * 10);
				send(SIB);
				beg_time = milliseconds("T6");
				state = 2;
				break;
			default:
				return FAILURE;
			}
			break;
		case 2:
			switch (event()) {
			case TIMEOUT:
				send(SIB_S);
				count++;
				if (count == n) {
					if (count > 1)
						send(COUNT);
					return FAILURE;
				}
				start_tt(iutconf.sl.t5 * 10);
			case FISU:
				pt_bsn = pt_fsn = 0x7f;
				send(FISU_S);
				break;
			case OUT_OF_SERVICE:
				break;
			case SIOS:
				if (count > 1)
					send(COUNT);
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
#endif

int iut_showmsg(struct strbuf *ctrl, struct strbuf *data);

typedef struct ppa {
	struct sockaddr_in loc;
	struct sockaddr_in rem;
} ppa_t;

void
print_ppa(ppa_t * ppa, int len)
{
	if (len >= sizeof(ppa->loc)) {
		printf("PPA loc Address family = %d\n", ppa->loc.sin_family);
		printf("PPA loc Port = %d\n", ppa->loc.sin_port);
		printf("PPA loc Address = %s\n", inet_ntoa(ppa->loc.sin_addr));
		FFLUSH(stdout);
	}
	if (len >= sizeof(ppa->loc) + sizeof(ppa->rem)) {
		printf("PPA rem Address family = %d\n", ppa->rem.sin_family);
		printf("PPA rem Port = %d\n", ppa->rem.sin_port);
		printf("PPA rem Address = %s\n", inet_ntoa(ppa->rem.sin_addr));
		FFLUSH(stdout);
	}
}

static int
pt_open(void)
{
	if (verbose) {
		printf("     :pipe\n");
		FFLUSH(stdout);
	}
	if (pipe(&pt_fd) < 0) {
		printf("****ERROR: pipe failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("     :fcntl\n");
		FFLUSH(stdout);
	}
	if (fcntl(pt_fd, F_SETFL, O_NONBLOCK) < 0) {
		printf("****ERROR: fcntl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("     :ioctl\n");
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_SRDOPT, RMSGD | RPROTNORM) < 0) {
		printf("****ERROR: ioctl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (ioctl(pt_fd, I_SWROPT, 0) < 0) {
		printf("****ERROR: ioctl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("     :push pipemod\n");
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_PUSH, "pipemod") < 0) {
		printf("****ERROR: push pipemod failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("     :push sdl\n");
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_PUSH, "sdl") < 0) {
		printf("****ERROR: push sdl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("     :push sdt\n");
		FFLUSH(stdout);
	}
	if (ioctl(pt_fd, I_PUSH, "sdt") < 0) {
		printf("****ERROR: push sdt failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
pt_close(void)
{
	if (verbose) {
		printf("     :close\n");
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

static int
pt_attach(void)
{
#if 0
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct sockaddr_in loc = { AF_INET, htons(10001), {INADDR_ANY} };

	// inet_aton("127.0.0.1", &loc.sin_addr);
	if (verbose) {
		printf("     :attach\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->attach_req) + sizeof(loc);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&loc, p->attach_req.lmi_ppa, sizeof(loc));
	if ((ret = putmsg(pt_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(pt_fd, &ctrl, &data, &flags)) < 0) {
			printf("****ERROR: getmsg failed\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		if (p->lmi_primitive != LMI_OK_ACK) {
			iut_showmsg(&ctrl, &data);
			return FAILURE;
		}
		return SUCCESS;
	}
#endif
	return SUCCESS;
}

static int
pt_detach(void)
{
#if 0
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	if (verbose) {
		printf("     :detach\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(pt_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(pt_fd, &ctrl, &data, &flags)) < 0) {
			printf("****ERROR: getmsg failed\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		if (p->lmi_primitive != LMI_OK_ACK) {
			iut_showmsg(&ctrl, &data);
			return FAILURE;
		}
		return SUCCESS;
	}
#endif
	return SUCCESS;
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
	struct sockaddr_in rem = { AF_INET, htons(10002), {INADDR_ANY} };

	inet_aton("127.0.0.1", &rem.sin_addr);
	if (verbose) {
		printf("     :enable\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->enable_req) + sizeof(rem);
	ctrl.buf = cbuf;
	p->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	bcopy(&rem, p->enable_req.lmi_rem, sizeof(rem));
	if ((ret = putmsg(pt_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(pt_fd, &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf("****ERROR: getmsg failed\n");
				printf("           %s: %s\n", __FUNCTION__, strerror(errno));
				FFLUSH(stdout);
				return FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_ENABLE_CON) {
			iut_showmsg(&ctrl, &data);
			return FAILURE;
		} else
			return SUCCESS;
	}
	return SUCCESS;
}

static int
pt_disable(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct strbuf ctrl = { sizeof(*cbuf), sizeof(p->disable_req), cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };

	if (verbose) {
		printf("     :disable\n");
		FFLUSH(stdout);
	}
	p->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	if (putmsg(pt_fd, &ctrl, NULL, RS_HIPRI) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(pt_fd, &ctrl, &data, &flags)) < 0) {
			printf("****ERROR: getmsg failed\n");
			printf("           %s: %s\n", __FUNCTION__, strerror(errno));
			FFLUSH(stdout);
			return FAILURE;
		}
		if (p->lmi_primitive != LMI_DISABLE_CON) {
			iut_showmsg(&ctrl, &data);
			return FAILURE;
		}
		return SUCCESS;
	}
	return SUCCESS;
}

static int
pt_config(void)
{
	struct strioctl ctl;

	if (verbose) {
		printf("     :config\n");
		FFLUSH(stdout);
	}
	if (verbose) {
		printf("     :options\n");
		FFLUSH(stdout);
	}
	ptconf.opt.pvar = SS7_PVAR_ITUT_96;
	ptconf.opt.popt = 0;
	ctl.ic_cmd = LMI_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(ptconf.opt);
	ctl.ic_dp = (char *) &ptconf.opt;
	if (ioctl(pt_fd, I_STR, &ctl) < 0) {
		printf("****ERROR: options failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		exit(2);
	}
	if (verbose) {
		printf("     :config sdl\n");
		FFLUSH(stdout);
	}
	ptconf.sdl.ifflags = 0;
	ptconf.sdl.iftype = SDL_TYPE_PACKET;
	ptconf.sdl.ifrate = 64000;
	ptconf.sdl.ifgtype = SDL_GTYPE_UDP;
	ptconf.sdl.ifgrate = 10000000;
	ptconf.sdl.ifmode = SDL_MODE_PEER;
	ptconf.sdl.ifgmode = SDL_GMODE_NONE;
	ptconf.sdl.ifgcrc = SDL_GCRC_NONE;
	ptconf.sdl.ifclock = SDL_CLOCK_NONE;
	ptconf.sdl.ifcoding = SDL_CODING_NONE;
	ptconf.sdl.ifframing = SDL_FRAMING_NONE;
	ptconf.sdl.ifleads = 0;
	ptconf.sdl.ifalarms = 0;
	ptconf.sdl.ifrxlevel = 0;
	ptconf.sdl.iftxlevel = 0;
	ptconf.sdl.ifsync = 0;
	ptconf.sdl.ifsyncsrc[0] = 0;
	ptconf.sdl.ifsyncsrc[1] = 0;
	ptconf.sdl.ifsyncsrc[2] = 0;
	ptconf.sdl.ifsyncsrc[3] = 0;
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(ptconf.sdl);
	ctl.ic_dp = (char *) &ptconf.sdl;
	if (ioctl(pt_fd, I_STR, &ctl) < 0) {
		printf("****ERROR: config sdl failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	if (verbose) {
		printf("     :config sdt\n");
		FFLUSH(stdout);
	}
	ptconf.sdt.N = 16;
	ptconf.sdt.m = 272;
	ptconf.sdt.t8 = 100;
	ptconf.sdt.Tin = 4;
	ptconf.sdt.Tie = 1;
	ptconf.sdt.D = 256;
	ptconf.sdt.Te = 577169;
	ptconf.sdt.De = 9308000;
	ptconf.sdt.Ue = 144292000;
	ptconf.sdt.b = 8;
	ptconf.sdt.f = 0;
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(ptconf.sdt);
	ctl.ic_dp = (char *) &ptconf.sdt;
	if (ioctl(pt_fd, I_STR, &ctl) < 0) {
		printf("****ERROR: config sdt failed\n");
		printf("%s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	return SUCCESS;
}

int
pt_power_on(void)
{
	int ret;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	union SDT_primitives *d = (union SDT_primitives *) cbuf;

	if (verbose) {
		printf("     :power on\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(d->daedt_start_req);
	ctrl.buf = cbuf;
	d->daedt_start_req.sdt_primitive = SDT_DAEDT_START_REQ;
	if ((ret = putmsg(pt_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(d->daedr_start_req);
	ctrl.buf = cbuf;
	d->daedr_start_req.sdt_primitive = SDT_DAEDR_START_REQ;
	if ((ret = putmsg(pt_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("****ERROR: putmsg failed\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
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
	if (pt_attach() != SUCCESS)
		return FAILURE;
	if (pt_config() != SUCCESS)
		return FAILURE;
	if (pt_enable() != SUCCESS)
		return FAILURE;
	if (pt_power_on() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
pt_end(void)
{
	if (pt_disable() != SUCCESS)
		return FAILURE;
	if (pt_detach() != SUCCESS)
		return FAILURE;
	ioctl(pt_fd, I_FLUSH, FLUSHRW);	/* flush IUT */
	if (pt_close() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
iut_open(void)
{
	if (verbose) {
		printf("                                  :fcntl\n");
		FFLUSH(stdout);
	}
	if (fcntl(iut_fd, F_SETFL, O_NONBLOCK) < 0) {
		printf("                                   ****ERROR: fcntl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("                                  :ioctl\n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_SRDOPT, RMSGD | RPROTNORM) < 0) {
		printf("                                   ****ERROR: ioctl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (ioctl(iut_fd, I_SWROPT, 0) < 0) {
		printf("                                   ****ERROR: ioctl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("                                  :push sdl\n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_PUSH, "sdl") < 0) {
		printf("                                   ****ERROR: push sdl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("                                  :push sdt\n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_PUSH, "sdt") < 0) {
		printf("                                   ****ERROR: push sdt failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	if (verbose) {
		printf("                                  :push sl\n");
		FFLUSH(stdout);
	}
	if (ioctl(iut_fd, I_PUSH, "sl") < 0) {
		printf("                                   ****ERROR: push sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
iut_close(void)
{
	if (verbose) {
		printf("                                  :close\n");
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

static int
iut_attach(void)
{
#if 0
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct sockaddr_in loc = { AF_INET, htons(10002), {INADDR_ANY}, };

	// inet_aton("127.0.0.1", &loc.sin_addr);
	if (verbose) {
		printf("                                  :attach\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->attach_req) + sizeof(loc);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&loc, p->attach_req.lmi_ppa, sizeof(loc));
	if ((ret = putmsg(iut_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(iut_fd, &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf("                                   ****ERROR: getmsg failed\n");
				printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
				FFLUSH(stdout);
				return FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_OK_ACK) {
			printf("                                   ****ERROR: getmsg failed\n");
			FFLUSH(stdout);
			printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
			iut_showmsg(&ctrl, &data);
		} else
			return SUCCESS;
	}
#endif
	return SUCCESS;
}

static int
iut_detach(void)
{
#if 0
	int ret;
	char cbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;

	if (verbose) {
		printf("                                  :detach\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(iut_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
#endif
	return SUCCESS;
}

static int
iut_enable(void)
{
	int ret, flags = 0;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct sockaddr_in rem = { AF_INET, htons(10001), {INADDR_ANY}, };

	inet_aton("127.0.0.1", &rem.sin_addr);
	if (verbose) {
		printf("                                  :enable\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->enable_req) + sizeof(rem);
	ctrl.buf = cbuf;
	p->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	bcopy(&rem, p->enable_req.lmi_rem, sizeof(rem));
	if ((ret = putmsg(iut_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	for (;;) {
		flags = 0;
		ctrl.maxlen = BUFSIZE;
		ctrl.len = 0;
		ctrl.buf = cbuf;
		data.maxlen = BUFSIZE;
		data.len = 0;
		data.buf = dbuf;
		if ((ret = getmsg(iut_fd, &ctrl, &data, &flags)) < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf("                                   ****ERROR: getmsg failed\n");
				printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
				FFLUSH(stdout);
				return FAILURE;
			}
			continue;
		}
		if (p->lmi_primitive != LMI_ENABLE_CON && p->lmi_primitive != LMI_OK_ACK) {
			printf("                                   ****ERROR: getmsg failed\n");
			FFLUSH(stdout);
			printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
			iut_showmsg(&ctrl, &data);
		} else
			return SUCCESS;
	}
	return SUCCESS;
}

static int
iut_disable(void)
{
	char cbuf[BUFSIZE];
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	struct strbuf ctrl = { sizeof(*cbuf), sizeof(p->disable_req), cbuf };

	if (verbose) {
		printf("                                  :disable\n");
		FFLUSH(stdout);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	ctrl.buf = cbuf;
	p->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	ctrl.len = LMI_DISABLE_REQ_SIZE;
	if (putmsg(iut_fd, &ctrl, NULL, RS_HIPRI) < 0) {
		printf("                                   ****ERROR: putmsg failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	return SUCCESS;
}

static int
iut_config(void)
{
	struct strioctl ctl;

	if (verbose) {
		printf("                                  :config\n");
		FFLUSH(stdout);
	}
	if (verbose) {
		printf("                                  :options\n");
		FFLUSH(stdout);
	}
	iutconf.opt.pvar = SS7_PVAR_ITUT_96;
	iutconf.opt.popt = iut_options;
	ctl.ic_cmd = LMI_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.opt);
	ctl.ic_dp = (char *) &iutconf.opt;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
	      option_sdl_failed:
		printf("                                   ****ERROR: options failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	ctl.ic_cmd = LMI_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.opt);
	ctl.ic_dp = (char *) &iutconf.opt;
	if (ioctl(iut_fd, I_STR, &ctl) < 0)
		goto option_sdl_failed;
	if (iutconf.opt.pvar != SS7_PVAR_ITUT_96 || iutconf.opt.popt != iut_options)
		goto option_sdl_failed;
	if (verbose) {
		printf("                                  :config sdl\n");
		FFLUSH(stdout);
	}
	iutconf.sdl.ifflags = 0;
	iutconf.sdl.iftype = SDL_TYPE_PACKET;
	iutconf.sdl.ifrate = 64000;
	iutconf.sdl.ifgtype = SDL_GTYPE_NONE;
	iutconf.sdl.ifgrate = 10000000;
	iutconf.sdl.ifmode = SDL_MODE_PEER;
	iutconf.sdl.ifgcrc = SDL_GCRC_NONE;
	iutconf.sdl.ifclock = SDL_CLOCK_SHAPER;
	iutconf.sdl.ifcoding = SDL_CODING_NONE;
	iutconf.sdl.ifframing = SDL_FRAMING_NONE;
	iutconf.sdl.ifleads = 0;
	iutconf.sdl.ifleads = 0;
	iutconf.sdl.ifalarms = 0;
	iutconf.sdl.ifrxlevel = 0;
	iutconf.sdl.iftxlevel = 0;
	iutconf.sdl.ifsync = 0;
	iutconf.sdl.ifsyncsrc[0] = 0;
	iutconf.sdl.ifsyncsrc[1] = 0;
	iutconf.sdl.ifsyncsrc[2] = 0;
	iutconf.sdl.ifsyncsrc[3] = 0;
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdl);
	ctl.ic_dp = (char *) &iutconf.sdl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
	      config_sdl_failed:
		printf("                                   ****ERROR: config sdl failed\n");
		FFLUSH(stdout);
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		return FAILURE;
	}
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdl);
	ctl.ic_dp = (char *) &iutconf.sdl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0)
		goto config_sdl_failed;
	if (iutconf.sdl.ifflags != 0 || iutconf.sdl.iftype != SDL_TYPE_PACKET || iutconf.sdl.ifrate != 64000 || iutconf.sdl.ifgtype != SDL_GTYPE_NONE || iutconf.sdl.ifgrate != 10000000 || iutconf.sdl.ifmode != SDL_MODE_PEER || iutconf.sdl.ifgcrc != SDL_GCRC_NONE || iutconf.sdl.ifclock != SDL_CLOCK_SHAPER || iutconf.sdl.ifcoding != SDL_CODING_NONE || iutconf.sdl.ifframing != SDL_FRAMING_NONE || iutconf.sdl.ifleads != 0 || iutconf.sdl.ifleads != 0 || iutconf.sdl.ifalarms != 0 || iutconf.sdl.ifrxlevel != 0 || iutconf.sdl.iftxlevel != 0 || iutconf.sdl.ifsync != 0 || iutconf.sdl.ifsyncsrc[0] != 0 || iutconf.sdl.ifsyncsrc[1] != 0 || iutconf.sdl.ifsyncsrc[2] != 0 || iutconf.sdl.ifsyncsrc[3] != 0)
		goto config_sdl_failed;
	if (verbose) {
		printf("                                  :config sdt\n");
		FFLUSH(stdout);
	}
	iutconf.sdt.N = 16;
	iutconf.sdt.m = 272;
	iutconf.sdt.t8 = 100;
	iutconf.sdt.Tin = 4;
	iutconf.sdt.Tie = 1;
	iutconf.sdt.T = 64;
	iutconf.sdt.D = 256;
	iutconf.sdt.Te = 577169;
	iutconf.sdt.De = 9308000;
	iutconf.sdt.Ue = 144292000;
	iutconf.sdt.b = 8;
	iutconf.sdt.f = 0;
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdt);
	ctl.ic_dp = (char *) &iutconf.sdt;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
	      config_sdt_failed:
		printf("                                   ****ERROR: config sdt failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	ctl.ic_cmd = SDT_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sdt);
	ctl.ic_dp = (char *) &iutconf.sdt;
	if (ioctl(iut_fd, I_STR, &ctl) < 0)
		goto config_sdt_failed;
	if (iutconf.sdt.N != 16 || iutconf.sdt.m != 272 || iutconf.sdt.t8 != 100 || iutconf.sdt.Tin != 4 || iutconf.sdt.Tie != 1 || iutconf.sdt.T != 64 || iutconf.sdt.D != 256 || iutconf.sdt.Te != 577169 || iutconf.sdt.De != 9308000 || iutconf.sdt.Ue != 144292000 || iutconf.sdt.b != 8 || iutconf.sdt.f != 0)
		goto config_sdt_failed;
	if (verbose) {
		printf("                                  :config sl\n");
		FFLUSH(stdout);
	}
	iutconf.sl.t1 = 45 * 1000;	/* milliseconds */
	iutconf.sl.t2 = 5 * 1000;	/* milliseconds */
	iutconf.sl.t2l = 20 * 1000;	/* milliseconds */
	iutconf.sl.t2h = 100 * 1000;	/* milliseconds */
	iutconf.sl.t3 = 1 * 1000;	/* milliseconds */
	iutconf.sl.t4n = 8 * 1000;	/* milliseconds */
	iutconf.sl.t4e = 500;	/* milliseconds */
	iutconf.sl.t5 = 100;	/* milliseconds */
	iutconf.sl.t6 = 4 * 1000;	/* milliseconds */
	iutconf.sl.t7 = 1 * 1000;	/* milliseconds */
	iutconf.sl.rb_abate = 3;	/* messages */
	iutconf.sl.rb_accept = 6;	/* messages */
	iutconf.sl.rb_discard = 9;	/* messages */
	iutconf.sl.tb_abate_1 = 128 * 272;	/* octets */
	iutconf.sl.tb_onset_1 = 256 * 272;	/* octets */
	iutconf.sl.tb_discd_1 = 384 * 272;	/* octets */
	iutconf.sl.tb_abate_2 = 512 * 272;	/* octets */
	iutconf.sl.tb_onset_2 = 640 * 272;	/* octets */
	iutconf.sl.tb_discd_2 = 768 * 272;	/* octets */
	iutconf.sl.tb_abate_3 = 896 * 272;	/* octets */
	iutconf.sl.tb_onset_3 = 1024 * 272;	/* octets */
	iutconf.sl.tb_discd_3 = 1152 * 272;	/* octets */
	iutconf.sl.N1 = 127;	/* messages */
	iutconf.sl.N2 = 8192;	/* octets */
	iutconf.sl.M = 5;
	ctl.ic_cmd = SL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sl);
	ctl.ic_dp = (char *) &iutconf.sl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0) {
	      config_sl_failed:
		printf("                                   ****ERROR: config sl failed\n");
		printf("                                              %s: %s\n", __FUNCTION__, strerror(errno));
		FFLUSH(stdout);
		return FAILURE;
	}
	ctl.ic_cmd = SL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(iutconf.sl);
	ctl.ic_dp = (char *) &iutconf.sl;
	if (ioctl(iut_fd, I_STR, &ctl) < 0)
		goto config_sl_failed;
	if (iutconf.sl.t1 != 45 * 1000 || iutconf.sl.t2 != 5 * 1000 || iutconf.sl.t2l != 20 * 1000 || iutconf.sl.t2h != 100 * 1000 || iutconf.sl.t3 != 1 * 1000 || iutconf.sl.t4n != 8 * 1000 || iutconf.sl.t4e != 500 || iutconf.sl.t5 != 100 || iutconf.sl.t6 != 4 * 1000 || iutconf.sl.t7 != 1 * 1000 || iutconf.sl.rb_abate != 3 || iutconf.sl.rb_accept != 6 || iutconf.sl.rb_discard != 9 || iutconf.sl.tb_abate_1 != 128 * 272 || iutconf.sl.tb_onset_1 != 256 * 272 || iutconf.sl.tb_discd_1 != 384 * 272 || iutconf.sl.tb_abate_2 != 512 * 272 || iutconf.sl.tb_onset_2 != 640 * 272 || iutconf.sl.tb_discd_2 != 768 * 272 || iutconf.sl.tb_abate_3 != 896 * 272 || iutconf.sl.tb_onset_3 != 1024 * 272 || iutconf.sl.tb_discd_3 != 1152 * 272 || iutconf.sl.N1 != 127 || iutconf.sl.N2 != 8192 || iutconf.sl.M != 5)
		goto config_sl_failed;
	return SUCCESS;
}

static int
iut_power_off(void)
{
	signal(STOP);
	stop_tt();
	show_msus = 0;
	show_fisus = 1;
	while (wait_event(0) != NO_MSG) ;
	iut_disable();
	iut_detach();
	ioctl(iut_fd, I_FLUSH, FLUSHRW);	/* flush IUT */
	iut_close();
	return SUCCESS;
}

static int
link_power_off(void)
{
	int ret = SUCCESS;

	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	pt_bib = pt_fib = iut_bib = iut_fib = 0x80;
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x7f;
	if ((ret = iut_open()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_attach()) != SUCCESS)
		return INCONCLUSIVE;
	iut_options = 0;
	if ((ret = iut_config()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_enable()) != SUCCESS)
		return INCONCLUSIVE;
	start_tt(TEST_TIMEOUT);
	return SUCCESS;
}

#if 0
static int
link_out_of_service(void)
{
	int ret = SUCCESS;

	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	pt_bib = pt_fib = iut_bib = iut_fib = 0x80;
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x7f;
	if ((ret = iut_open()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_attach()) != SUCCESS)
		return INCONCLUSIVE;
	iut_options = 0;
	if ((ret = iut_config()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_enable()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = signal(POWER_ON)) != SUCCESS)
		return INCONCLUSIVE;
	start_tt(TEST_TIMEOUT);
	return SUCCESS;
}

static int
link_in_service(void)
{
	int ret = SUCCESS;

	show_msus = 1;
	show_fisus = 1;
	show_timeout = 0;
	pt_bib = pt_fib = iut_bib = iut_fib = 0x80;
	pt_bsn = pt_fsn = iut_bsn = iut_fsn = 0x7f;
	if ((ret = iut_open()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_attach()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_config()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = iut_enable()) != SUCCESS)
		return INCONCLUSIVE;
	if ((ret = signal(POWER_ON)) != SUCCESS)
		return INCONCLUSIVE;
	start_tt(TEST_TIMEOUT);
	for (;;) {
		switch (state) {
		case 0:
			switch (event()) {
			case SIOS:
				send(SIOS);
				signal(START);
				state = 1;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 1:
			switch (event()) {
			case SIOS:
				send(SIOS);
				break;
			case SIO:
				send(SIO);
				state = 2;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 2:
			switch (event()) {
			case SIO:
			case SIN:
				send(SIE);
				start_tt(iutconf.sl.t4e * 10 + 200);
				state = 3;
				break;
			default:
				return INCONCLUSIVE;
			}
			break;
		case 3:
			switch (event()) {
			case SIN:
				send(SIE);
				break;
			case FISU:
				send(FISU);
				break;
			case IN_SERVICE:
				start_tt(TEST_TIMEOUT);
				return SUCCESS;
			default:
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
	iut_options = SS7_POPT_PCR;
	return link_in_service();
}
#endif

typedef struct test_case {
	int (*test) (void);		/* test case function */
	int (*precond) (void);		/* test preconditions */
	char *title;			/* title of the test case */
} test_case_t;

static test_case_t test_suite[] = {
	{test_1_1a, link_power_off,
	 "Q.781/Test 1.1(a)\n" "Link State Control - Expected signal units/orders\n" "Initialization (Power-up)\n"}
#if 0
	,
	{test_1_1b, link_out_of_service,
	 "Q.781/Test 1.1(b)\n" "Link State Control - Expected signal units/orders\n" "Initialization (Power-up)\n"}
	,
	{test_1_2, link_out_of_service,
	 "Q.781/Test 1.2\n" "Link State Control - Expected signal units/orders\n" "Timer T2\n"}
	,
	{test_1_3, link_out_of_service,
	 "Q.781/Test 1.3\n" "Link State Control - Expected signal units/orders\n" "Timer T3\n"}
	,
	{test_1_4, link_out_of_service,
	 "Q.781/Test 1.4\n" "Link State Control - Expected signal units/orders\n" "Timer T1 & Timer T4 (Normal)\n"}
	,
	{test_1_5a, link_out_of_service,
	 "Q.781/Test 1.5(a)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment procedure\n"}
	,
	{test_1_5b, link_out_of_service,
	 "Q.781/Test 1.5(b)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment procedure\n"}
	,
	{test_1_6, link_out_of_service,
	 "Q.781/Test 1.6\n" "Link State Control - Expected signal units/orders\n" "Normal alignment procedure - correct procedure (MSU)\n"}
	,
	{test_1_7, link_out_of_service,
	 "Q.781/Test 1.7\n" "Link State Control - Expected signal units/orders\n" "SIO received during normal proving period\n"}
	,
	{test_1_8a, link_out_of_service,
	 "Q.781/Test 1.8(a)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment with PO set (FISU)\n"}
	,
	{test_1_8b, link_out_of_service,
	 "Q.781/Test 1.8(b)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment with PO set (FISU)\n"}
	,
	{test_1_9a, link_out_of_service,
	 "Q.781/Test 1.9(a)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment with PO set (MSU)\n"}
	,
	{test_1_9b, link_out_of_service,
	 "Q.781/Test 1.9(b)\n" "Link State Control - Expected signal units/orders\n" "Normal alignment with PO set (MSU)\n"}
	,
	{test_1_10, link_out_of_service,
	 "Q.781/Test 1.10\n" "Link State Control - Expected signal units/orders\n" "Normal alignment with PO set and cleared\n"}
	,
	{test_1_11, link_out_of_service,
	 "Q.781/Test 1.11\n" "Link State Control - Expected signal units/orders\n" "Set RPO when \"Aligned not ready\"\n"}
	,
	{test_1_12a, link_out_of_service,
	 "Q.781/Test 1.12(a)\n" "Link State Control - Expected signal units/orders\n" "SIOS received when \"Aligned not ready\"\n"}
	,
	{test_1_12b, link_out_of_service,
	 "Q.781/Test 1.12(b)\n" "Link State Control - Expected signal units/orders\n" "SIOS received when \"Aligned not ready\"\n"}
	,
	{test_1_13, link_out_of_service,
	 "Q.781/Test 1.13\n" "Link State Control - Expected signal units/orders\n" "SIO received when \"Aligned not ready\"\n"}
	,
	{test_1_14, link_out_of_service,
	 "Q.781/Test 1.14\n" "Link State Control - Expected signal units/orders\n" "Set and clear LPO when \"Initial alignment\"\n"}
	,
	{test_1_15, link_out_of_service,
	 "Q.781/Test 1.15\n" "Link State Control - Expected signal units/orders\n" "Set and clear LPO when \"Aligned ready\"\n"}
	,
	{test_1_16, link_out_of_service,
	 "Q.781/Test 1.16\n" "Link State Control - Expected signal units/orders\n" "Timer T1 in \"Aligned not ready\" state\n"}
	,
	{test_1_17, link_out_of_service,
	 "Q.781/Test 1.17\n" "Link State Control - Expected signal units/orders\n" "No SIO sent during normal proving period\n"}
	,
	{test_1_18, link_out_of_service,
	 "Q.781/Test 1.18\n" "Link State Control - Expected signal units/orders\n" "Set and cease emergency prior to \"start alignment\"\n"}
	,
	{test_1_19, link_out_of_service,
	 "Q.781/Test 1.19\n" "Link State Control - Expected signal units/orders\n" "Set emergency while in \"not aligned state\"\n"}
	,
	{test_1_20, link_out_of_service,
	 "Q.781/Test 1.20\n" "Link State Control - Expected signal units/orders\n" "Set emergency when \"aligned\"\n"}
	,
	{test_1_21, link_out_of_service,
	 "Q.781/Test 1.21\n" "Link State Control - Expected signal units/orders\n" "Both ends set emergency.\n"}
	,
	{test_1_22, link_out_of_service,
	 "Q.781/Test 1.22\n" "Link State Control - Expected signal units/orders\n" "Individual end sets emergency\n"}
	,
	{test_1_23, link_out_of_service,
	 "Q.781/Test 1.23\n" "Link State Control - Expected signal units/orders\n" "Set emergency during normal proving\n"}
	,
	{test_1_24, link_out_of_service,
	 "Q.781/Test 1.24\n" "Link State Control - Expected signal units/orders\n" "No SIO send during emergency alignment\n"}
	,
	{test_1_25, link_out_of_service,
	 "Q.781/Test 1.25\n" "Link State Control - Expected signal units/orders\n" "Deactivation duing intial alignment\n"}
	,
	{test_1_26, link_out_of_service,
	 "Q.781/Test 1.26\n" "Link State Control - Expected signal units/orders\n" "Deactivation during aligned state\n"}
	,
	{test_1_27, link_out_of_service,
	 "Q.781/Test 1.27\n" "Link State Control - Expected signal units/orders\n" "Deactivation during aligned not ready\n"}
	,
	{test_1_28, link_in_service_basic,
	 "Q.781/Test 1.28\n" "Link State Control - Expected signal units/orders\n" "SIO received during link in service\n"}
	,
	{test_1_29a, link_in_service_basic,
	 "Q.781/Test 1.29(a)\n" "Link State Control - Expected signal units/orders\n" "SIO received during link in service\n"}
	,
	{test_1_29b, link_in_service_basic,
	 "Q.781/Test 1.29(b)\n" "Link State Control - Expected signal units/orders\n" "SIO received during link in service\n"}
	,
	{test_1_30a, link_in_service_basic,
	 "Q.781/Test 1.30(a)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during LPO\n"}
	,
	{test_1_30b, link_in_service_basic,
	 "Q.781/Test 1.30(b)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during LPO\n"}
	,
	{test_1_31a, link_in_service_basic,
	 "Q.781/Test 1.31(a)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during RPO\n"}
	,
	{test_1_31b, link_in_service_basic,
	 "Q.781/Test 1.31(b)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during RPO\n"}
	,
	{test_1_32a, link_out_of_service,
	 "Q.781/Test 1.32(a)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during the proving period\n"}
	,
	{test_1_32b, link_out_of_service,
	 "Q.781/Test 1.32(b)\n" "Link State Control - Expected signal units/orders\n" "Deactivation during the proving period\n"}
	,
	{test_1_33, link_out_of_service,
	 "Q.781/Test 1.33\n" "Link State Control - Expected signal units/orders\n" "SIO received instead of FISUs\n"}
	,
	{test_1_34, link_out_of_service,
	 "Q.781/Test 1.34\n" "Link State Control - Expected signal units/orders\n" "SIO received instead of FISUs\n"}
	,
	{test_1_35, link_out_of_service,
	 "Q.781/Test 1.35\n" "Link State Control - Expected signal units/orders\n" "SIPO received instead of FISUs\n"}
	,
	{test_2_1, link_out_of_service,
	 "Q.781/Test 2.1\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Out of service\" state\n"}
	,
	{test_2_2, link_out_of_service,
	 "Q.781/Test 2.2\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Not Aligned\" state\n"}
	,
	{test_2_3, link_out_of_service,
	 "Q.781/Test 2.3\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Aligned\" state\n"}
	,
	{test_2_4, link_out_of_service,
	 "Q.781/Test 2.4\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Proving\" state\n"}
	,
	{test_2_5, link_out_of_service,
	 "Q.781/Test 2.5\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Aligned Ready\" state\n"}
	,
	{test_2_6, link_out_of_service,
	 "Q.781/Test 2.6\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Aligned Not Ready\" state\n"}
	,
	{test_2_7, link_in_service_basic,
	 "Q.781/Test 2.7\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"In Service\" state\n"}
	,
	{test_2_8, link_in_service_basic,
	 "Q.781/Test 2.8\n" "Link State Control - Unexpected signal units/orders\n" "Unexpected signal units/orders in \"Processor Outage\" state\n"}
	,
	{test_3_1, link_out_of_service,
	 "Q.781/Test 3.1\n" "Transmission Failure\n" "Link aligned ready (Break Tx path)\n"}
	,
	{test_3_2, link_in_service_basic,
	 "Q.781/Test 3.2\n" "Transmission Failure\n" "Link aligned ready (Corrupt FIBs - Basic)\n"}
	,
	{test_3_3, link_out_of_service,
	 "Q.781/Test 3.3\n" "Transmission Failure\n" "Link aligned not ready (Break Tx path)\n"}
	,
	{test_3_4, link_out_of_service,
	 "Q.781/Test 3.4\n" "Transmission Failure\n" "Link aligned not ready (Corrupt FIBs - Basic)\n"}
	,
	{test_3_5, link_in_service_basic,
	 "Q.781/Test 3.5\n" "Transmission Failure\n" "Link in service (Break Tx path)\n"}
	,
	{test_3_6, link_in_service_basic,
	 "Q.781/Test 3.6\n" "Transmission Failure\n" "Link in service (Corrupt FIBs - Basic)\n"}
	,
	{test_3_7, link_in_service_basic,
	 "Q.781/Test 3.7\n" "Transmission Failure\n" "Link in processor outage (Break Tx path)\n"}
	,
	{test_3_8, link_in_service_basic,
	 "Q.781/Test 3.8\n" "Transmission Failure\n" "Link in processor outage (Corrupt FIBs - Basic)\n"}
	,
	{test_4_1, link_in_service_basic,
	 "Q.781/Test 4.1\n" "Processor Outage Control\n" "Set and clear LPO while link in service\n"}
	,
	{test_4_2, link_in_service_basic,
	 "Q.781/Test 4.2\n" "Processor Outage Control\n" "RPO during LPO\n"}
	,
	{test_4_3, link_in_service_basic,
	 "Q.781/Test 4.3\n" "Processor Outage Control\n" "Clear LPO when \"Both processor outage\"\n"}
	,
	{test_5_1, link_in_service_basic,
	 "Q.781/Test 5.1\n" "SU delimitation, alignment, error detection and correction\n" "More than 7 ones between MSU opening and closing flags\n"}
	,
	{test_5_2, link_in_service_basic,
	 "Q.781/Test 5.2\n" "SU delimitation, alignment, error detection and correction\n" "Greater than maximum signal unit length\n"}
	,
	{test_5_3, link_in_service_basic,
	 "Q.781/Test 5.3\n" "SU delimitation, alignment, error detection and correction\n" "Below minimum signal unit length\n"}
	,
	{test_5_4a, link_in_service_basic,
	 "Q.781/Test 5.4(a)\n" "SU delimitation, alignment, error detection and correction\n" "Reception of single and multiple flags between FISUs\n"}
	,
	{test_5_4b, link_in_service_basic,
	 "Q.781/Test 5.4(b)\n" "SU delimitation, alignment, error detection and correction\n" "Reception of single and multiple flags between FISUs\n"}
	,
	{test_5_5a, link_in_service_basic,
	 "Q.781/Test 5.5(a)\n" "SU delimitation, alignment, error detection and correction\n" "Reception of single and multiple flags between MSUs\n"}
	,
	{test_5_5b, link_in_service_basic,
	 "Q.781/Test 5.5(b)\n" "SU delimitation, alignment, error detection and correction\n" "Reception of single and multiple flags between MSUs\n"}
#if 0
	,
	{test_6_1, link_in_service_basic,
	 "Q.781/Test 6.1\n" "SUERM check\n" "Error rate of 1 in 256 - Link remains in service\n"}
	,
	{test_6_2, link_in_service_basic,
	 "Q.781/Test 6.2\n" "SUERM check\n" "Error rate of 1 in 254 - Link out of service\n"}
#endif
	,
	{test_6_3, link_in_service_basic,
	 "Q.781/Test 6.3\n" "SUERM check\n" "Consequtive corrupt SUs\n"}
	,
	{test_6_4, link_in_service_basic,
	 "Q.781/Test 6.4\n" "SUERM check\n" "Time controlled break of the link\n"}
	,
	{test_7_1, link_out_of_service,
	 "Q.781/Test 7.1\n" "AERM check\n" "Error rate below the normal threshold\n"}
	,
	{test_7_2, link_out_of_service,
	 "Q.781/Test 7.2\n" "AERM check\n" "Error rate at the normal threshold\n"}
	,
	{test_7_3, link_out_of_service,
	 "Q.781/Test 7.3\n" "AERM check\n" "Error rate above the normal threshold\n"}
	,
	{test_7_4, link_out_of_service,
	 "Q.781/Test 7.4\n" "AERM check\n" "Error rate at the emergency threshold\n"}
	,
	{test_8_1, link_in_service_basic,
	 "Q.781/Test 8.1\n" "Transmission and reception control (Basic)\n" "MSU transmission and reception\n"}
	,
	{test_8_2, link_in_service_basic,
	 "Q.781/Test 8.2\n" "Transmission and reception control (Basic)\n" "Negative acknowledgement of an MSU\n"}
	,
	{test_8_3, link_in_service_basic,
	 "Q.781/Test 8.3\n" "Transmission and reception control (Basic)\n" "Check RTB full\n"}
	,
	{test_8_4, link_in_service_basic,
	 "Q.781/Test 8.4\n" "Transmission and reception control (Basic)\n" "Single MSU with erroneous FIB\n"}
	,
	{test_8_5, link_in_service_basic,
	 "Q.781/Test 8.5\n" "Transmission and reception control (Basic)\n" "Duplicated FSN\n"}
	,
	{test_8_6, link_in_service_basic,
	 "Q.781/Test 8.6\n" "Transmission and reception control (Basic)\n" "Erroneous retransmission - Single MSU\n"}
	,
	{test_8_7, link_in_service_basic,
	 "Q.781/Test 8.7\n" "Transmission and reception control (Basic)\n" "Erroneous retransmission - Multiple FISUs\n"}
	,
	{test_8_8, link_in_service_basic,
	 "Q.781/Test 8.8\n" "Transmission and reception control (Basic)\n" "Single FISU with corrupt FIB\n"}
	,
	{test_8_9, link_in_service_basic,
	 "Q.781/Test 8.9\n" "Transmission and reception control (Basic)\n" "Single FISU prior to RPO being set\n"}
	,
	{test_8_10, link_in_service_basic,
	 "Q.781/Test 8.10\n" "Transmission and reception control (Basic)\n" "Abnormal BSN - single MSU\n"}
	,
	{test_8_11, link_in_service_basic,
	 "Q.781/Test 8.11\n" "Transmission and reception control (Basic)\n" "Abnormal BSN - two consecutive FISUs\n"}
	,
	{test_8_12, link_in_service_basic,
	 "Q.781/Test 8.12\n" "Transmission and reception control (Basic)\n" "Excessive delay of acknowledgement\n"}
	,
	{test_8_13, link_in_service_basic,
	 "Q.781/Test 8.13\n" "Transmission and reception control (Basic)\n" "Level 3 Stop command\n"}
	,
	{test_9_1, link_in_service_pcr,
	 "Q.781/Test 9.1\n" "Transmission and reception control (PCR)\n" "MSU transmission and reception\n"}
	,
	{test_9_2, link_in_service_pcr,
	 "Q.781/Test 9.2\n" "Transmission and reception control (PCR)\n" "Priority control\n"}
	,
	{test_9_3, link_in_service_pcr,
	 "Q.781/Test 9.3\n" "Transmission and reception control (PCR)\n" "Forced retransmission with the value N1\n"}
	,
	{test_9_4, link_in_service_pcr,
	 "Q.781/Test 9.4\n" "Transmission and reception control (PCR)\n" "Forced retransmission with the value N2\n"}
	,
	{test_9_5, link_in_service_pcr,
	 "Q.781/Test 9.5\n" "Transmission and reception control (PCR)\n" "Forced retransmission cancel\n"}
	,
	{test_9_6, link_in_service_pcr,
	 "Q.781/Test 9.6\n" "Transmission and reception control (PCR)\n" "Reception of forced retransmission\n"}
	,
	{test_9_7, link_in_service_pcr,
	 "Q.781/Test 9.7\n" "Transmission and reception control (PCR)\n" "MSU transmission while RPO set\n"}
	,
	{test_9_8, link_in_service_pcr,
	 "Q.781/Test 9.8\n" "Transmission and reception control (PCR)\n" "Abnormal BSN - Single MSU\n"}
	,
	{test_9_9, link_in_service_pcr,
	 "Q.781/Test 9.9\n" "Transmission and reception control (PCR)\n" "Abnormal BSN - Two MSUs\n"}
	,
	{test_9_10, link_in_service_pcr,
	 "Q.781/Test 9.10\n" "Transmission and reception control (PCR)\n" "Unexpected FSN\n"}
	,
	{test_9_11, link_in_service_pcr,
	 "Q.781/Test 9.11\n" "Transmission and reception control (PCR)\n" "Excessive delay of acknowledgement\n"}
	,
	{test_9_12, link_in_service_pcr,
	 "Q.781/Test 9.12\n" "Transmission and reception control (PCR)\n" "FISU with FSN expected for MSU\n"}
	,
	{test_9_13, link_in_service_pcr,
	 "Q.781/Test 9.13\n" "Transmission and reception control (PCR)\n" "Level 3 Stop command\n"}
	,
	{test_10_1, link_in_service_basic,
	 "Q.781/Test 10.1\n" "Congestion Control\n" "Congestion abatement\n"}
	,
	{test_10_2, link_in_service_basic,
	 "Q.781/Test 10.2\n" "Congestion Control\n" "Timer T7\n"}
	,
	{test_10_3, link_in_service_basic,
	 "Q.781/Test 10.3\n" "Congestion Control\n" "Timer T6\n"}
#endif
};

static int failed_state = 0;
static int failed_event = 0;

static int
run_test(test_case_t * tcase)
{
	int ret = 0;

	printf(tcase->title);
	fflush(stdout);
	// ioctl(pt_fd, I_FLUSH, FLUSHRW); /* flush PT */
	state = 0;
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
	failed_state = -1;
	failed_event = -1;
	if (pt_start() == SUCCESS) {
		printf("\nPrecondition:\n");
		FFLUSH(stdout);
		if ((ret = (*tcase->precond) ()) == SUCCESS) {
			state = 0;
			count = 0;
			tries = 0;
			beg_time = 0;
			expand = 0;
			printf("\nTest case:\n");
			FFLUSH(stdout);
			ret = (*tcase->test) ();
		}
	} else
		ret = INCONCLUSIVE;
	switch (ret) {
	case SUCCESS:
		printf("                 =====SUCCESS=====\n");
		failed_state = 0;
		failed_event = 0;
		break;
	case FAILURE:
		printf("                 +++++FAILURE+++++\n");
		failed_state = state;
		failed_event = pt_event;
		break;
	case INCONCLUSIVE:
		printf("                 ??INCONCLUSIVE???\n");
		failed_state = state;
		failed_event = pt_event;
		break;
	default:
		printf("                 =+=+=+ERROR+=+=+=\n");
		failed_state = state;
		failed_event = pt_event;
		break;
	}
	printf("\nPostcondition:\n");
	FFLUSH(stdout);
	iut_power_off();
	pt_end();
	return ret;
}

const char *
lmi_strreason(unsigned int reason)
{
	const char *r;

	switch (reason) {
	default:
	case LMI_UNSPEC:
		r = "Unknown or unspecified";
		break;
	case LMI_BADADDRESS:
		r = "Address was invalid";
		break;
	case LMI_BADADDRTYPE:
		r = "Invalid address type";
		break;
	case LMI_BADDIAL:
		r = "(not used)";
		break;
	case LMI_BADDIALTYPE:
		r = "(not used)";
		break;
	case LMI_BADDISPOSAL:
		r = "Invalid disposal parameter";
		break;
	case LMI_BADFRAME:
		r = "Defective SDU received";
		break;
	case LMI_BADPPA:
		r = "Invalid PPA identifier";
		break;
	case LMI_BADPRIM:
		r = "Unregognized primitive";
		break;
	case LMI_DISC:
		r = "Disconnected";
		break;
	case LMI_EVENT:
		r = "Protocol-specific event ocurred";
		break;
	case LMI_FATALERR:
		r = "Device has become unusable";
		break;
	case LMI_INITFAILED:
		r = "Link initialization failed";
		break;
	case LMI_NOTSUPP:
		r = "Primitive not supported by this device";
		break;
	case LMI_OUTSTATE:
		r = "Primitive was issued from invalid state";
		break;
	case LMI_PROTOSHORT:
		r = "M_PROTO block too short";
		break;
	case LMI_SYSERR:
		r = "UNIX system error";
		break;
	case LMI_WRITEFAIL:
		r = "Unitdata request failed";
		break;
	case LMI_CRCERR:
		r = "CRC or FCS error";
		break;
	case LMI_DLE_EOT:
		r = "DLE EOT detected";
		break;
	case LMI_FORMAT:
		r = "Format error detected";
		break;
	case LMI_HDLC_ABORT:
		r = "Aborted frame detected";
		break;
	case LMI_OVERRUN:
		r = "Input overrun";
		break;
	case LMI_TOOSHORT:
		r = "Frame too short";
		break;
	case LMI_INCOMPLETE:
		r = "Partial frame received";
		break;
	case LMI_BUSY:
		r = "Telephone was busy";
		break;
	case LMI_NOANSWER:
		r = "Connection went unanswered";
		break;
	case LMI_CALLREJECT:
		r = "Connection rejected";
		break;
	case LMI_HDLC_IDLE:
		r = "HDLC line went idle";
		break;
	case LMI_HDLC_NOTIDLE:
		r = "HDLC link no longer idle";
		break;
	case LMI_QUIESCENT:
		r = "Line being reassigned";
		break;
	case LMI_RESUMED:
		r = "Line has been reassigned";
		break;
	case LMI_DSRTIMEOUT:
		r = "Did not see DSR in time";
		break;
	case LMI_LAN_COLLISIONS:
		r = "LAN excessive collisions";
		break;
	case LMI_LAN_REFUSED:
		r = "LAN message refused";
		break;
	case LMI_LAN_NOSTATION:
		r = "LAN no such station";
		break;
	case LMI_LOSTCTS:
		r = "Lost Clear to Send signal";
		break;
	case LMI_DEVERR:
		r = "Start of device-specific error codes";
		break;
	}
	return r;
}

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
			printf("Version = 0x%08lx\n", p->info_ack.lmi_version);
			printf("State = %lu\n", p->info_ack.lmi_state);
			printf("Max sdu = %lu\n", p->info_ack.lmi_max_sdu);
			printf("Min sdu = %lu\n", p->info_ack.lmi_min_sdu);
			printf("Header len = %lu\n", p->info_ack.lmi_header_len);
			printf("PPA style = %lu\n", p->info_ack.lmi_ppa_style);
			printf("PPA length = %u\n", ppalen);
			FFLUSH(stdout);
			print_ppa((ppa_t *) p->info_ack.lmi_ppa_addr, ppalen);
		}
			return (p->lmi_primitive);
		case LMI_OK_ACK:
		{
			printf("LMI_OK_ACK:\n");
			printf("Correct primitive = %lu\n", p->ok_ack.lmi_correct_primitive);
			printf("State = %lu\n", p->ok_ack.lmi_state);
			FFLUSH(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ERROR_ACK:
		{
			printf("LMI_ERROR_ACK:\n");
			printf("Error number = %lu\n", p->error_ack.lmi_errno);
			printf("Error string = %s\n", strerror(p->error_ack.lmi_errno));
			printf("Reason number = %lu\n", p->error_ack.lmi_reason);
			printf("Reason string = %s\n", lmi_strreason(p->error_ack.lmi_reason));
			printf("Error primitive = %lu\n", p->error_ack.lmi_error_primitive);
			printf("State = %lu\n", p->error_ack.lmi_state);
			FFLUSH(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ERROR_IND:
		{
			printf("LMI_ERROR_IND:\n");
			printf("Error number = %lu\n", p->error_ind.lmi_errno);
			printf("Error string = %s\n", strerror(p->error_ind.lmi_errno));
			printf("Reason number = %lu\n", p->error_ind.lmi_reason);
			printf("Reason string = %s\n", lmi_strreason(p->error_ind.lmi_reason));
			printf("State = %lu\n", p->error_ind.lmi_state);
			FFLUSH(stdout);
		}
			return (p->lmi_primitive);
		case LMI_ENABLE_CON:
		{
			printf("LMI_ENABLE_CON:\n");
			printf("State = %lu\n", p->enable_con.lmi_state);
			FFLUSH(stdout);
		}
			return (p->lmi_primitive);
		case LMI_DISABLE_CON:
		{
			printf("LMI_DISABLE_CON:\n");
			printf("State = %lu\n", p->enable_con.lmi_state);
			FFLUSH(stdout);
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
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_LINK_CONGESTED_IND:
			{
				printf("SL_LINK_CONGESTED_IND:\n");
				printf("  timestamp = %lu\n", l->link_cong_ind.sl_timestamp);
				printf("  cong stat = %lu\n", l->link_cong_ind.sl_cong_status);
				printf("  disc stat = %lu\n", l->link_cong_ind.sl_disc_status);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_LINK_CONGESTION_CEASED_IND:
			{
				printf("SL_LINK_CONGESTION_CEASED_IND:\n");
				printf("  timestamp = %lu\n", l->link_cong_ceased_ind.sl_timestamp);
				printf("  cong stat = %lu\n", l->link_cong_ceased_ind.sl_cong_status);
				printf("  disc stat = %lu\n", l->link_cong_ceased_ind.sl_disc_status);
				FFLUSH(stdout);
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
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_RB_CLEARED_IND:
			{
				printf("SL_RB_CLEARED_IND\n");
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_BSNT_IND:
			{
				printf("SL_BSNT_IND:\n");
				printf("  bsnt = %lu\n", l->bsnt_ind.sl_bsnt);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_IN_SERVICE_IND:
			{
				printf("SL_IN_SERVICE_IND\n");
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_OUT_OF_SERVICE_IND:
			{
				printf("SL_OUT_OF_SERVICE_IND:\n");
				printf("  timestamp = %lu\n", l->out_of_service_ind.sl_timestamp);
				printf("  reason    = %lu\n", l->out_of_service_ind.sl_reason);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			{
				printf("SL_REMOTE_PROCESSOR_OUTAGE_IND:\n");
				printf("  timestamp = %lu\n", l->rem_proc_out_ind.sl_timestamp);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			{
				printf("SL_REMOTE_PROCESSOR_RECOVERED_IND:\n");
				printf("  timestamp = %lu\n", l->rem_proc_recovered_ind.sl_timestamp);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			case SL_RTB_CLEARED_IND:
			{
				printf("SL_RTB_CLEARED_IND\n");
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			default:
			{
				printf("Unrecognized primitive %lu!\n", l->sl_primitive);
				FFLUSH(stdout);
			}
				return (l->sl_primitive);
			}
		}
	}
	return (0);
}

int
do_tests(void)
{
	int i, ret;
	int failed = 0, passed = 0, inconc = 0, errored = 0;

	printf("\n");
	FFLUSH(stdout);
	for (i = 0; i < sizeof(test_suite) / sizeof(test_case_t); i++) {
		printf("\n");
		FFLUSH(stdout);
		ret = run_test(&test_suite[i]);
		printf("\n********\n");
		switch (ret) {
		case FAILURE:
			failed++;
			printf("******** FAILURE(%d) - test case failed in state %d event %s.\n", state, failed_state, event_string(failed_event));
			break;
		case SUCCESS:
			passed++;
			printf("******** SUCCESS - test case successful.\n");
			break;
		case INCONCLUSIVE:
			inconc++;
			printf("******** INCONCLUSIVE(%d) - test case inconclusive in state %d event %s.\n", failed_state, failed_state, event_string(failed_event));
			break;
		case SCRIPTERROR:
		default:
			errored++;
			printf("******** ERROR(%d) - test case completed in error in state %d event %s.\n", failed_state, failed_state, event_string(failed_event));
			break;
		}
		printf("********\n\n");
		fflush(stdout);
	}
	printf("\n");
	printf("Test Suite Summary:\n");
	printf("    Failed       %d\n", failed);
	printf("    Passed       %d\n", passed);
	printf("    Inconclusive %d\n", inconc);
	printf("    Errors       %d\n", errored);
	printf("\n");
	FFLUSH(stdout);
	return (0);
}

void
copying(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
ITU-T Recommendation Q.781 - Conformance Test Suite\n\
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
General Public License (GPL) Version 3,  so long as the  software is distributed\n\
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
");
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
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.8 $ $Date: 2007/08/19 11:57:20 $");
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
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "qvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
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
	copying(argc, argv);
	do_tests();
	exit(0);
}
