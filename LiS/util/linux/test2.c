/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* 
 *  Test program for clarification of bug with inode locking.
 *
 *  This forks two processes:
 *  1) writer1. This does blocking putmsg() on /dev/loop.1.
 *  2) writer2. This opens another file on /dev/loop.1 and does
 *     non-blocking putmsg(). Whenever the putmsg() call takes
 *     too long, a warning is printed.
 *  The main process does getmsg() from /dev/loop.2 (connected
 *  to /dev/loop.1), but slowly.
 *
 */
#include <sys/stropts.h>
#include <sys/LiS/loop.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif				/* _GNU_SOURCE */

#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int verbose = 1;

int fd1, fd2;

void writer1(void)
{
	struct strbuf datbuf;
	char buf[1024];

	buf[0] = 'A';
	while (1) {
		usleep(1000);

		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *) &buf;

		printf("a");
		fflush(stdout);
		if (putmsg(fd1, NULL, &datbuf, 0) < 0)
			perror("writer1: putmsg()");
	}
}

void writer2(void)
{
	int fd;
	struct strbuf datbuf;
	char buf[1024];
	int ret;
	struct timeval tv1, tv2;
	int t;

	if ((fd = open("/dev/loop.1", O_RDWR | O_NONBLOCK)) < 0) {
		perror("writer2: open(fd)");
		exit(1);
	}

	buf[0] = 'B';
	while (1) {
		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *) &buf;

		gettimeofday(&tv1, NULL);
		ret = putmsg(fd, NULL, &datbuf, 0);
		gettimeofday(&tv2, NULL);

		t = tv2.tv_usec - tv1.tv_usec + 1000000 * (tv2.tv_sec - tv1.tv_sec);
		if (t > 500000) {
			printf("\n*** Nonblocking putmsg() duration: %d microsecs.\n", t);
			fflush(stdout);
		}

		if (ret < 0) {
			if (errno != EWOULDBLOCK)
				perror("writer2: putmsg()");
			else {
				/* 
				   printf("\n*** Nonblocking putmsg(): Would block.\n");
				   fflush(stdout); */
				usleep(1);
			}
		} else {
			printf("b");
			fflush(stdout);
		}
	}
}

void reader(void)
{
	struct strbuf datbuf;
	char buf[1024];
	int flags;

	while (1) {
		usleep(200000);	/* Slow reader.. */

		datbuf.maxlen = sizeof(buf);
		datbuf.len = 0;
		datbuf.buf = (char *) &buf;
		flags = 0;

		if (getmsg(fd2, NULL, &datbuf, &flags) < 0)
			perror("reader: getmsg()");
		else
			printf("%c", buf[0]);
		fflush(stdout);
	}
}

void copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
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
Regulations  (\"FAR\") (or any success  regulations) or, in the  cases of NASA, in\n\
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
    Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
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
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL [default: 1]\n\
        This option may be repeated.\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -h, --help, -?, --?\n\
        Print this usage message and exits\n\
    -V, --version\n\
        Print the version and exits\n\
\n\
", argv[0]);
}

int main(int argc, char *argv[])
{
	struct strioctl ioc;
	int arg;

	for (;;) {
		int c;
#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{ "quiet",	no_argument,		NULL, 'q'},
			{ "verbose",	optional_argument,	NULL, 'v'},
			{ "help",	no_argument,		NULL, 'h'},
			{ "version",	no_argument,		NULL, 'V'},
			{ "copying",	no_argument,		NULL, 'C'},
			{ "?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long_only(argc, argv, "qvhVC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "qvhVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
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

	/* 
	 *  Make this a process group leader
	 *  so that signals hit all children.
	 */
	if (setpgrp() < 0) {
		perror("setpgrp()");
		exit(1);
	}

	/* 
	 *  Open two loop endpoints and connect them.
	 */
	if ((fd1 = open("/dev/loop.1", O_RDWR)) < 0) {
		perror("open(fd1)");
		exit(1);
	}

	if ((fd2 = open("/dev/loop.2", O_RDWR)) < 0) {
		perror("open(fd2)");
		exit(1);
	}

	arg = 2;
	ioc.ic_cmd = LOOP_SET;
	ioc.ic_timout = 10;
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &arg;

	if (ioctl(fd1, I_STR, &ioc) < 0) {
		perror("ioctl()");
		exit(1);
	}

	/* 
	 *  Start the writer processes
	 */
	if (!fork())
		writer1();

	if (!fork())
		writer2();

	/* 
	 *  Now run reader
	 */
	reader();

	exit(0);
}
