/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/************************************************************************
*                     Multi-Threaded Test                               *
*************************************************************************
*									*
* This program runs data through the STREAMS loopback driver using	*
* multiple threads.  The threads act in pairs with one thread of each	*
* pair generating data and the other thread reading data.  The "reading"*
* thread echos the data back so that the generating thread can read	*
* back what it originally wrote.					*
*									*
* The main program monitors the progress and maintains a summary screen.*
*									*
************************************************************************/

/* 
 * Copyright (C)  2001  David Grothe, Gcom, Inc <dave@gcom.com>
 */

#define _REENTRANT
#define _THREAD_SAFE
#define _XOPEN_SOURCE	500	/* single unix spec */

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif				/* _GNU_SOURCE */
#include <stropts.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif				/* _GNU_SOURCE */
#include <sys/LiS/loop.h>	/* an odd place for this file */

int verbose = 1;

/************************************************************************
*                          Constants                                    *
************************************************************************/
#define MAX_THR		16	/* less than # of loop-clones */
#define	MAX_MSG		1024

typedef struct thread_info {
	int thread_number;
	pthread_mutex_t startup;	/* so everyone can start at once */

	int xmcnt;			/* transmit msg count */
	int xbcnt;			/* transmit byte count */
	int rmcnt;			/* receive msg count */
	int rbcnt;			/* receive byte count */

	int fd;				/* file descr to loop driver */
	int minor;			/* minor device number of loop */
	int looped;			/* have been looped to partner */
	int burst;			/* initial burst size for writer */

	char *xdbuf;			/* xmit data buffer */
	char *xcbuf;			/* xmit ctl buffer */
	char *rdbuf;			/* receive data buffer */
	char *rcbuf;			/* receive ctl buffer */
	struct strbuf xd;
	struct strbuf xc;
	struct strbuf rd;
	struct strbuf rc;

	struct thread_info *other;	/* pointer to partner's struct */

} thread_info_t;

/************************************************************************
*                       Global Storage                                  *
************************************************************************/
pthread_t thread_ids[MAX_THR + 1];	/* numbered from 1 */
thread_info_t thread_info[MAX_THR + 1];	/* numbered from 1 */
time_t start_time;
int max_threads = MAX_THR;
int time_factor = 1;
int initial_burst = 0;

/************************************************************************
*                           setup_thread_info				*
*************************************************************************
*									*
* Initialize a thread info structure.					*
*									*
************************************************************************/
void setup_thread_info(int thread_nr)
{
	thread_info_t *tp = &thread_info[thread_nr];
	int i;
	char *p;
	int arg;
	int rslt;
	int other_inx;			/* thr nr of my partner */
	struct strioctl ioc;
	pthread_mutexattr_t attr;

	memset(tp, 0, sizeof(*tp));
	tp->thread_number = thread_nr;
	tp->burst = initial_burst;

	tp->fd = open("/dev/loop_clone", O_RDWR, 0);
	if (tp->fd < 0) {
		if (verbose)
			perror("/dev/loop_clone");
		exit(1);
	}

	ioc.ic_timout = 10;
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &arg;

	ioc.ic_cmd = LOOP_GET_DEV;
	arg = -1;
	rslt = ioctl(tp->fd, I_STR, &ioc);
	if (rslt < 0) {
		if (verbose)
			perror("loop_clone: ioctl LOOP_GET_DEV");
		exit(1);
	}

	if (arg < 0) {
		if (verbose)
			fprintf(stderr, "loop_clone: ioctl LOOP_GET_DEV returned %d\n", arg);
		exit(1);
	}

	tp->minor = arg;	/* minor number of this loop clone */
	other_inx = ((tp->thread_number - 1) ^ 1) + 1;
	tp->other = &thread_info[other_inx];

	tp->xdbuf = malloc(MAX_MSG);
	tp->xcbuf = malloc(MAX_MSG);
	tp->rdbuf = malloc(MAX_MSG);
	tp->rcbuf = malloc(MAX_MSG);
	if (tp->xdbuf == NULL || tp->xcbuf == NULL || tp->rdbuf == NULL || tp->rcbuf == NULL) {
		if (verbose)
			fprintf(stderr, "setup_thread_info: cannot allocate memory\n");
		exit(1);
	}

	tp->xd.buf = tp->xdbuf;
	tp->xc.buf = tp->xcbuf;
	tp->rd.buf = tp->rdbuf;
	tp->rc.buf = tp->rcbuf;
	tp->rd.maxlen = MAX_MSG;
	tp->rc.maxlen = MAX_MSG;

	/* 
	 * Place a counting pattern in the xmit buffers
	 */
	for (p = tp->xdbuf, i = 0; i < MAX_MSG; i++, p++)
		*p = (char) i;
	for (p = tp->xcbuf, i = 0; i < MAX_MSG; i++, p++)
		*p = (char) i;

	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&tp->startup, &attr);
	pthread_mutex_lock(&tp->startup);

}

/************************************************************************
*                         set_loop                                      *
*************************************************************************
*									*
* Set the loopback file to point to its partner.			*
*									*
************************************************************************/
int set_loop(int fd, int other_minor)
{
	int rslt;
	struct strioctl ioc;

	if (verbose > 1)
		printf("set_loop(%d, %d)\n", fd, other_minor);
	ioc.ic_timout = 10;
	ioc.ic_cmd = LOOP_SET;
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &other_minor;
	rslt = ioctl(fd, I_STR, &ioc);
	if (rslt < 0)
		if (verbose)
			perror("set_loop: ioctl LOOP_SET");

	return (rslt);
}

/************************************************************************
*                           set_timer                                   *
*************************************************************************
*									*
* Set the timer for delay of messages in loopback driver.  Once set	*
* this value is used until the file is closed.				*
*									*
************************************************************************/
int set_timer(int fd, int timeout)
{
	int rslt;
	struct strioctl ioc;

	timeout = time_factor * timeout;
	ioc.ic_cmd = LOOP_TIMR;	/* set timer for queue */
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &timeout;
	rslt = ioctl(fd, I_STR, &ioc);
	if (rslt < 0)
		if (verbose)
			perror("set_timer: ioctl LOOP_TIMR");

	return (rslt);
}

/************************************************************************
*                           reader                                      *
*************************************************************************
*									*
* The reader process.  It reads messages and writes them back.		*
*									*
************************************************************************/
void *reader(void *arg)
{
	thread_info_t *tp = (thread_info_t *) arg;
	int nbytes;

	if (verbose > 1)
		printf("Reader[%d]: Waiting for startup\n", tp->thread_number);
	pthread_mutex_lock(&tp->startup);
	if (verbose)
		printf("Reader[%d]: Startup\n", tp->thread_number);

	for (;;) {
		if ((nbytes = read(tp->fd, tp->rdbuf, MAX_MSG)) < 0)
			break;

		tp->rmcnt++;
		tp->rbcnt += nbytes;

		if (write(tp->fd, tp->xdbuf, nbytes) < 0)
			break;

		tp->xmcnt++;
		tp->xbcnt += nbytes;
	}

	if (verbose)
		printf("Reader[%d]: Exiting\n", tp->thread_number);
	return (NULL);
}

/************************************************************************
*                           writer                                      *
*************************************************************************
*									*
* The writer process.  It writes messages and then reads them back.	*
*									*
************************************************************************/
void *writer(void *arg)
{
	thread_info_t *tp = (thread_info_t *) arg;
	int msg_size = MAX_MSG;
	int i;
	int nbytes;

	if (verbose > 1)
		printf("Writer[%d]: Waiting for startup\n", tp->thread_number);
	pthread_mutex_lock(&tp->startup);
	if (verbose)
		printf("Writer[%d]: Startup\n", tp->thread_number);

	for (i = 0; i < tp->burst; i++) {
		if (write(tp->fd, tp->xdbuf, msg_size) < 0)
			break;

		tp->xmcnt++;
		tp->xbcnt += msg_size;
	}

	for (;;) {
		if (write(tp->fd, tp->xdbuf, msg_size) < 0)
			break;

		tp->xmcnt++;
		tp->xbcnt += msg_size;

		if ((nbytes = read(tp->fd, tp->rdbuf, MAX_MSG)) < 0)
			break;

		tp->rmcnt++;
		tp->rbcnt += nbytes;
	}

	if (verbose)
		printf("Writer[%d]: Exiting\n", tp->thread_number);
	return (NULL);
}

/************************************************************************
*                            print_status                               *
*************************************************************************
*									*
* Print a status report on the progress of the threads.			*
*									*
************************************************************************/
void print_progress(void)
{
	int i;
	time_t now;
	long delta;
	thread_info_t *tp;

	if (verbose)
		printf("Thr   Xmit-Cnt Xmit/Sec    Rcv-Cnt  Rcv/Sec\n");
	now = time(NULL);
	delta = now - start_time;
	if (delta == 0)
		delta = 1;
	for (i = 1; i <= max_threads; i++) {
		tp = &thread_info[i];
		if (verbose)
			printf("%3u %10u %8u %10u %8u\n",
			       i,
			       tp->xmcnt, (int) (tp->xmcnt / delta), tp->rmcnt,
			       (int) (tp->rmcnt / delta));
	}
}

/************************************************************************
*                           get_options                                 *
************************************************************************/
void copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2003-2004  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 2001       David Grothe, Gcom, Inc <dave@gcom.com>\n\
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

void version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 2001       David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    included here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Options:\n\
    -b, --burst BURST\n\
        set initial burst for writer thread\n\
    -f, --factor FACTOR\n\
        set time delay factor to FACTOR\n\
    -t, --threads THREADS\n\
        set number of threads to THREADS\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0]);
}

void get_options(int argc, char **argv)
{
	for (;;) {
		int c;
#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{ "burst",   required_argument,	NULL, 'b'},
			{ "factor",  required_argument,	NULL, 'f'},
			{ "threads", required_argument,	NULL, 't'},
			{ "quiet",   no_argument,	NULL, 'q'},
			{ "verbose", optional_argument,	NULL, 'v'},
			{ "help",    no_argument,	NULL, 'h'},
			{ "version", no_argument,	NULL, 'V'},
			{ "copying", no_argument,	NULL, 'C'},
			{ "?",	     no_argument,	NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long_only(argc, argv, "b:f:t:qv::hVC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "b:f:t:qv::hVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'b':	/* -b, --burst BURST */
			initial_burst = strtol(optarg, NULL, 0);
			break;
		case 'f':	/* -f, --factor FACTOR */
			time_factor = strtol(optarg, NULL, 0);
			break;
		case 't':	/* -f, --threads THREADS */
			max_threads = strtol(optarg, NULL, 0);
			if (max_threads < 0)
				max_threads = 2;
			else if (max_threads & 0x01)
				max_threads++;
			if (max_threads > MAX_THR)
				max_threads = MAX_THR;
			break;
		case 'q':	/* -q, --quiet */
			verbose = 0;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (!optarg)
				verbose++;
			else
				verbose = strtol(optarg, NULL, 0);
			break;
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
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	if (optind < argc)
		goto bad_nonopt;
}

/************************************************************************
*                            main                                       *
*************************************************************************
*									*
* The main program sets up all the structurs and file descriptors and	*
* then lets the threads just move the data.				*
*									*
************************************************************************/
int main(int argc, char **argv)
{
	int rslt;
	int i;
	void *(*funcp) (void *);
	thread_info_t *tp;

	get_options(argc, argv);

	for (i = 1; i <= max_threads; i++) {
		if (i & 0x01)
			funcp = reader;
		else
			funcp = writer;

		setup_thread_info(i);
		tp = &thread_info[i];
		rslt = pthread_create(&thread_ids[i], NULL, funcp, tp);
		if (rslt < 0) {
			if (verbose)
				fprintf(stderr, "Thread #%d: ", i);
			if (verbose)
				perror("pthread_create");
		} else if (verbose > 1)
			printf("main: Thread #%d id=%ld\n", i, thread_ids[i]);
	}

	for (i = 1; i <= max_threads; i++) {
		tp = &thread_info[i];
		if (!tp->looped) {
			if (set_loop(tp->fd, tp->other->minor) < 0)
				exit(1);
			tp->looped = 1;
			tp->other->looped = 1;
		}
		if (set_timer(tp->fd, tp->thread_number) < 0)
			exit(1);
	}

	start_time = time(NULL);
	for (i = 1; i <= max_threads; i++) {
		tp = &thread_info[i];
		pthread_mutex_unlock(&tp->startup);
	}

	for (;;) {
		if (verbose > 1)
			printf("main: sleeping for 5 secs\n");
		sleep(5);
		print_progress();
	}

	if (verbose)
		printf("main: exiting\n");

	return (0);
}
