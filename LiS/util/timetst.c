/*****************************************************************************

 @(#) $RCSfile: timetst.c,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/05/14 08:35:14 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/05/14 08:35:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: timetst.c,v $
 Revision 1.1.1.2.4.3  2005/05/14 08:35:14  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: timetst.c,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/05/14 08:35:14 $"

static char const ident[] =
    "$RCSfile: timetst.c,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/05/14 08:35:14 $";

/*
 * Copyright 1997 David Grothe, Gcom, Inc <dave@gcom.com>
 */

#define	inline			/* make disappear */

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>		/* for strerror */
#ifdef LINUX
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#endif
#ifdef SYS_QNX
#include <ioctl.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/stropts.h>

#if	defined(LINUX)
#include <sys/LiS/linuxio.h>
#elif	defined(SYS_QNX)
#include <sys/LiS/qnxio.h>
#else
#include <sys/LiS/usrio.h>
#endif

#include <sys/LiS/loop.h>	/* an odd place for this file */

/************************************************************************
*                      File Names                                       *
************************************************************************/
#if	defined(LINUX)
#define	LOOP_1		"/dev/loop.1"
#define	LOOP_2		"/dev/loop.2"
#define LOOP_CLONE	"/dev/loop_clone"
#elif	defined(QNX)
#define	LOOP_1		"/dev/gcom/loop.1"
#define	LOOP_2		"/dev/gcom/loop.2"
#define LOOP_CLONE	"/dev/gcom/loop_clone"
#else
#define	LOOP_1		"loop.1"
#define	LOOP_2		"loop.2"
#define LOOP_CLONE	"loop_clone"
#endif

#define	ALL_DEBUG_BITS	( ~ ((unsigned long long) 0) )

#ifdef DIRECT_USER		/* tie-in to cmn_err */
#define	ENO(neg_e)	(-(neg_e))
#else
#define	ENO(dmy)	(errno)
#endif

/************************************************************************
*                           Storage                                     *
************************************************************************/

unsigned long long debug_mask;
char buf[1000];				/* general purpose */
char rdbuf[1000];			/* for reading */
long iter_cnt = 100000;			/* default iteration count */
char ctlbuf[1000];			/* control messages */
char rdctlbuf[1000];			/* control messages */

struct strbuf wr_ctl = { 0, 0, ctlbuf };
struct strbuf wr_dta = { 0, 0, buf };
struct strbuf rd_ctl = { 0, 0, rdctlbuf };
struct strbuf rd_dta = { 0, 0, rdbuf };

int output = 1;

extern void make_nodes(void);

/************************************************************************
*                        Histogram Maintenance                          *
************************************************************************/

int latency_opt = 0;			/* option to measure latency */

typedef struct {
	int micro_secs;
	unsigned counter;

} histogram_bucket_t;

#define	HISTOGRAM_INIT	{ \
			    {1, 0}, \
			    {2, 0}, \
			    {3, 0}, \
			    {4, 0}, \
			    {5, 0}, \
			    {6, 0}, \
			    {7, 0}, \
			    {8, 0}, \
			    {9, 0}, \
			    {10, 0}, \
			    {20, 0}, \
			    {30, 0}, \
			    {40, 0}, \
			    {50, 0}, \
			    {60, 0}, \
			    {70, 0}, \
			    {80, 0}, \
			    {90, 0}, \
			    {100, 0}, \
			    {200, 0}, \
			    {300, 0}, \
			    {400, 0}, \
			    {500, 0}, \
			    {600, 0}, \
			    {700, 0}, \
			    {800, 0}, \
			    {900, 0}, \
			    {1000, 0}, \
			    {2000, 0}, \
			    {3000, 0}, \
			    {4000, 0}, \
			    {5000, 0}, \
			    {6000, 0}, \
			    {7000, 0}, \
			    {8000, 0}, \
			    {9000, 0}, \
			    {10000, 0}, \
			    {20000, 0}, \
			    {30000, 0}, \
			    {40000, 0}, \
			    {50000, 0}, \
			    {60000, 0}, \
			    {70000, 0}, \
			    {80000, 0}, \
			    {90000, 0}, \
			    {100000, 0}, \
			    {200000, 0}, \
			    {300000, 0}, \
			    {400000, 0}, \
			    {500000, 0}, \
			    {600000, 0}, \
			    {700000, 0}, \
			    {800000, 0}, \
			    {900000, 0}, \
			    {1000000, 0}, \
			    {2000000, 0}, \
			    {3000000, 0}, \
			    {4000000, 0}, \
			    {5000000, 0}, \
			    {6000000, 0}, \
			    {7000000, 0}, \
			    {8000000, 0}, \
			    {9000000, 0}, \
			    {0, 0} \
			}

histogram_bucket_t hist_init[] = HISTOGRAM_INIT;
histogram_bucket_t hist[] = HISTOGRAM_INIT;

/*
 * Initialize the 'hist' buffer.
 */
void
init_hist(histogram_bucket_t * h)
{
	memcpy(h, hist_init, sizeof(hist_init));
}

/*
 * Enter a time interval into a histogram bucket
 */
void
enter_hist(histogram_bucket_t * h, int interval)
{
	for (; h->micro_secs != 0; h++) {
		if (interval <= h->micro_secs)
			break;
	}

	h->counter++;
}

/*
 * Print out the histogram
 */
void
print_hist(histogram_bucket_t * h, int minimum)
{
	histogram_bucket_t *p = h;
	int max_counter = 0;

	for (; p->micro_secs != 0; p++) {
		if (p->counter > max_counter)
			max_counter = p->counter;
	}

	for (p = h; p->micro_secs != 0; p++) {
		if (p->counter >= minimum)
			if (output)
				printf("%8d %12u\n", p->micro_secs, p->counter);
	}

	if (p->counter != 0)
		if (output)
			printf("%8s %12u\n", "Larger", p->counter);

}

/************************************************************************
*                           register_drivers                            *
*************************************************************************
*									*
* Register the drivers that we are going to use in the test with	*
* streams.								*
*									*
************************************************************************/
void
register_drivers(void)
{
#if	!defined(LINUX) && !defined(QNX)
	port_init();		/* stream head init routine */
#endif

}				/* register_drivers */

/************************************************************************
*                           timing_test                                 *
*************************************************************************
*									*
* Time reading and writing messages through streams.			*
*									*
************************************************************************/
void
timing_test(void)
{
	time_t time_on;
	time_t et;
	long i;
	int fd1;
	int fd2;
	int rslt;
	int lgth;
	int arg;
	int flags = MSG_ANY;
	int rband = 0;
	struct strioctl ioc;
	struct timeval *tp = NULL;
	struct timeval *rtp = NULL;
	int xmit_time;
	int rcv_time;
	int interval;
	struct timeval x;

	if (output)
		printf("\nBegin timing test\n");

	fd1 = user_open(LOOP_1, O_RDWR, 0);
	if (fd1 < 0) {
		if (output)
			printf("loop.1: %s\n", strerror(-fd1));
		return;
	}

	fd2 = user_open(LOOP_2, O_RDWR, 0);
	if (fd2 < 0) {
		if (output)
			printf("loop.2: %s\n", strerror(-fd2));
		return;
	}

	ioc.ic_cmd = LOOP_SET;
	ioc.ic_timout = 10;
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &arg;

	arg = 2;
	rslt = user_ioctl(fd1, I_STR, &ioc);
	if (rslt < 0) {
		if (output)
			printf("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt));
	}

	if (latency_opt) {
		tp = (struct timeval *) buf;
		strcpy(&buf[sizeof(*tp)], "Data to send down the file");
		lgth = sizeof(*tp) + strlen(&buf[sizeof(*tp)]);
		rtp = (struct timeval *) rdbuf;
	} else {
		strcpy(buf, "Data to send down the file");
		lgth = strlen(buf);
	}

	if (output) {
		printf("Time test:  write %d bytes, read/write and service queue: ", lgth);
		fflush(stdout);
	}

	sync();			/* do file sync now rather than in the middle of the test. */
	init_hist(hist);
	time_on = time(NULL);
#if 1
	for (i = 0; i < iter_cnt; i++) {
		if (latency_opt)
			gettimeofday(tp, NULL);

		rslt = user_write(fd1, buf, lgth);

		if (rslt != lgth) {
			if (output) {
				if (rslt < 0)
					printf("loop.1: write: %s\n", strerror(-rslt));
				else
					printf("loop.1:  write returned %d, expected %d\n", rslt,
					       lgth);
			}
			break;
		}

		rslt = user_read(fd2, rdbuf, lgth);
		if (rslt != lgth) {
			if (output) {
				if (rslt < 0)
					printf("loop.2: read: %s\n", strerror(-rslt));
				else
					printf("loop.2:  read returned %d, expected %d\n", rslt,
					       lgth);
			}
			break;
		}

		if (latency_opt) {
			gettimeofday(&x, NULL);	/* current time */
			xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec;
			rcv_time = x.tv_sec * 1000000 + x.tv_usec;
			interval = rcv_time - xmit_time;
			enter_hist(hist, interval);
		}
	}
#endif
	et = (time(NULL) - time_on) * 1000000;	/* time in usecs */

	if (output)
		printf("%ld micro-secs\n", et / iter_cnt);
	if (latency_opt)
		print_hist(hist, 1000);
	if (output)
		printf("\n");

	if (output)
		printf("%ld micro-secs\n", et / iter_cnt);

	ioc.ic_cmd = LOOP_PUTNXT;	/* use putnxt rather then svcq */
	ioc.ic_len = 0;
	rslt = user_ioctl(fd1, I_STR, &ioc);
	if (rslt < 0) {
		if (output)
			printf("loop.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt));
	}

	if (output)
		printf("Time test:  write %d bytes, read/write w/o service queue: ", lgth);
	if (output)
		fflush(stdout);
	init_hist(hist);
	sync();
	time_on = time(NULL);

	for (i = 0; i < iter_cnt; i++) {
		if (latency_opt)
			gettimeofday(tp, NULL);

		rslt = user_write(fd1, buf, lgth);

		if (rslt != lgth) {
			if (output) {
				if (rslt < 0)
					printf("loop.1: write: %s\n", strerror(-rslt));
				else
					printf("loop.1:  write returned %d, expected %d\n", rslt,
					       lgth);
			}
			break;
		}

		rslt = user_read(fd2, rdbuf, lgth);
		if (rslt != lgth) {
			if (output) {
				if (rslt < 0)
					printf("loop.2: read: %s\n", strerror(-rslt));
				else
					printf("loop.2:  read returned %d, expected %d\n", rslt,
					       lgth);
			}
			break;
		}

		if (latency_opt) {
			gettimeofday(&x, NULL);	/* current time */
			xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec;
			rcv_time = x.tv_sec * 1000000 + x.tv_usec;
			interval = rcv_time - xmit_time;
			enter_hist(hist, interval);
		}
	}

	et = (time(NULL) - time_on) * 1000000;	/* time in usecs */

	if (output)
		printf("%ld micro-secs\n", et / iter_cnt);
	if (latency_opt)
		print_hist(hist, 1000);
	if (output)
		printf("\n");

	ioc.ic_cmd = LOOP_PUTNXT;	/* use putnxt rather then svcq */
	ioc.ic_len = 0;
	rslt = user_ioctl(fd1, I_STR, &ioc);
	if (rslt < 0) {
		if (output)
			printf("loop.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt));
	}

	if (output)
		printf("Time test:  write %d bytes, getmsg/putmsg w/o service queue: ", lgth);
	fflush(stdout);
	init_hist(hist);
	sync();
	time_on = time(NULL);

	rd_ctl.maxlen = sizeof(rdctlbuf);
	rd_dta.maxlen = sizeof(rdbuf);
	wr_ctl.len = -1;
	wr_dta.len = lgth;
	for (i = 0; i < iter_cnt; i++) {
		if (latency_opt)
			gettimeofday(tp, NULL);

		rslt = user_putpmsg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND);

		if (rslt < 0) {
			if (output)
				printf("loop.1: putmsg: %s\n", strerror(-rslt));
			break;
		}

		rd_ctl.len = -1;
		rd_dta.len = -1;
		flags = MSG_ANY;

		rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags);

		if (rslt < 0) {
			if (output)
				printf("loop.2: getmsg: %s\n", strerror(-rslt));
			break;
		} else if (rd_dta.len != lgth || rd_ctl.len > 0) {
			if (output) {
				printf("expected rd_ctl.len = %d, got %d\n", -1, rd_ctl.len);
				printf("expected rd_dta.len = %d, got %d\n", lgth, rd_dta.len);
			}
		}

		if (latency_opt) {
			gettimeofday(&x, NULL);	/* current time */
			xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec;
			rcv_time = x.tv_sec * 1000000 + x.tv_usec;
			interval = rcv_time - xmit_time;
			enter_hist(hist, interval);
		}
	}

	et = (time(NULL) - time_on) * 1000000;	/* time in usecs */

	if (output)
		printf("%ld micro-secs\n", et / iter_cnt);
	if (latency_opt)
		print_hist(hist, 1000);
	if (output)
		printf("\n");

	if (output)
		printf("%ld micro-secs\n", et / iter_cnt);

	user_close(fd1);
	user_close(fd2);

}				/* timing_test */

/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void
set_debug_mask(unsigned long long msk)
{
	int fd;
	int rslt;
	unsigned long mask1 = (unsigned long) (msk & 0xFFFFFFFF);
	unsigned long mask2 = (msk >> 32);

	fd = user_open(LOOP_1, O_RDWR, 0);
	if (fd < 0) {
		if (output)
			printf("loop.1: %s\n", strerror(ENO(fd)));
		exit(1);
	}

	rslt = user_ioctl(fd, I_LIS_SDBGMSK, mask1);
	if (rslt < 0) {
		if (output)
			printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(ENO(rslt)));
		exit(1);
	}

	rslt = user_ioctl(fd, I_LIS_SDBGMSK2, mask2);
	if (output)
		printf("\nSTREAMS debug mask set to 0x%08lx%08lx\n", mask2, mask1);

	user_close(fd);

}				/* set_debug_mask */

/************************************************************************
*                              main                                     *
************************************************************************/

void
copying(int argc, char *argv[])
{
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2003-2005  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997       David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software;  you can  redistribute  it and/or modify it under\n\
the terms of the GNU General  Public License as  published by the  Free Software\n\
Foundation; either  version 2 of  the  License, or  (at  your option) any  later\n\
version.\n\
\n\
This program is distributed in the hope that it will be  useful, but WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received  a copy of the GNU  General  Public License along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
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
", argv[0], ident);
}

void
version(int argc, char *argv[])
{
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 1997       David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2, included\n\
    here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-c,--count}] count\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] [{-c,--count}] count\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Options:\n\
    [-c, --count] COUNT\n\
        sets the iteration count to COUNT [default: %2$ld]\n\
    -q, --quiet\n\
        suppresses normal output\n\
    -h, --help\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0], iter_cnt);
}

int
main(int argc, char *argv[])
{
	/* 
	 *  Automake distcheck (reasonably) does not permit installed binaries
	 *  that do not accept --help and --version options.
	 */
	while (1) {
		int c;

#ifdef _GNU_SOURCE
		int option_index = 0;
	/* *INDENT-OFF* */
	static struct option long_options[] = {
	    { "count",	 required_argument, NULL, 'c' },
	    { "quiet",	 no_argument,	    NULL, 'q' },
	    { "help",	 no_argument,	    NULL, 'h' },
	    { "version", no_argument,	    NULL, 'V' },
	    { "copying", no_argument,	    NULL, 'C' },
	    { 0, }
	};
	/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "c:qhVC", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "c:qhVC");
#endif				/* _GNU_SOURCE */

		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'q':
			output = 0;
			break;
		case 'h':
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case 'C':
			copying(argc, argv);
			exit(0);
		case 'c':
			if (sscanf(optarg, "%li", &iter_cnt) != 1)
				goto bad_option;
			break;
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonoption:
			if (optind < argc && output) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * mimic old behavior of taking iteration count as non-option argument 
	 */
	if (optind < argc) {
		if (sscanf(optarg, "%li", &iter_cnt) != 1)
			goto bad_nonoption;
		optind++;
	}
	/* 
	 * don't ignore additional (permuted) non-option arguments 
	 */
	if (optind < argc)
		goto bad_nonoption;

#if	!defined(LINUX) && !defined(QNX)
	register_drivers();
	make_nodes();
#endif

	version(argc, argv);
	copying(argc, argv);

	if (output) {
		printf("Timing test version %s\n\n", "2.5 09/02/04");
		printf("Using safe constructs and message tracing\n");
	}
	set_debug_mask(debug_mask | 0x30000L);
	timing_test();

	if (output)
		printf("\n\nNot using safe constructs or message tracing\n");
	set_debug_mask(debug_mask & ~0x30000L);
	timing_test();

	return 0;
}
