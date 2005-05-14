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


/* 
 *  Test program for clarification of Berges bug.
 *
 *  This forks three processes:
 *  1) The two sleepers. They ends up sleeping in getpmsg().
 *  2) The hanger. This tries to do a putpmsg() on the
 *     same file where the sleepers are sleeping. It will
 *     hang because the sleeper has downed the inode
 *     semaphore.
 *
 */
#include <sys/stropts.h>
#include <sys/LiS/loop.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif				/* _GNU_SOURCE */

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int verbose = 1;

int fd1, fd2;

void sleeper1(void)
{
	struct strbuf datbuf;
	char buf[256];
	int flags;

	datbuf.maxlen = sizeof(buf);
	datbuf.len = 0;
	datbuf.buf = (char *) &buf;
	flags = 0;

	if (verbose)
		printf("sleeper1: calling getmsg()\n");
	if (getmsg(fd1, NULL, &datbuf, &flags) < 0) {
		if (verbose)
			perror("sleeper1: getmsg()");
	} else {
		if (verbose)
			printf("sleeper1: returned from getmsg()\n");
	}
}

void sleeper2(void)
{
	struct strbuf datbuf;
	char buf[256];
	int flags;

	datbuf.maxlen = sizeof(buf);
	datbuf.len = 0;
	datbuf.buf = (char *) &buf;
	flags = 0;

	if (verbose)
		printf("sleeper2: calling getmsg()\n");
	if (getmsg(fd1, NULL, &datbuf, &flags) < 0) {
		if (verbose)
			perror("sleeper2: getmsg()");
	} else {
		if (verbose)
			printf("sleeper2: returned from getmsg()\n");
	}
}

void hanger(void)
{
	struct strbuf datbuf;
	char buf[256];

	sleep(10);		/* Sleeper goes to sleep first */

	datbuf.maxlen = sizeof(buf);
	datbuf.len = sizeof(buf);
	datbuf.buf = (char *) &buf;

	if (verbose)
		printf("hanger: calling putmsg()\n");
	if (putmsg(fd1, NULL, &datbuf, 0) < 0) {
		if (verbose)
			perror("hanger: putmsg()");
	} else {
		if (verbose)
			printf("hanger: returned from putmsg()\n");
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
        Print this usage message and exit\n\
    -V, --version\n\
        Print the version and exit\n\
    -C, --copying\n\
        Print copying permission and exit\n\
\n\
", argv[0]);
}

int main(int argc, char *argv[])
{
	struct strioctl ioc;
	int arg, i;

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
		c = getopt_long_only(argc, argv, "qhv::VC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "qhv::VC?");
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
	 *  Start the slepper and hanger processes.
	 */

	if (!fork()) {
		sleeper1();
		exit(0);
	}

	if (!fork()) {
		sleeper2();
		exit(0);
	}

	if (!fork()) {
		hanger();
		exit(0);
	}

	/* 
	 *  Wait until things get hung up.
	 */

	sleep(30);

	/* 
	 *  Now wakeup sleepers. That will make hanger continue.
	 */

	for (i = 1; i < 3; ++i) {
		struct strbuf datbuf;
		char buf[256];

		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *) &buf;

		if (verbose)
			printf("main: calling putmsg() to wakeup sleeper%d\n", i);
		if (putmsg(fd2, NULL, &datbuf, 0) < 0) {
			if (verbose) {
				perror("main: putmsg()");
			} else {
				if (verbose)
					printf("main: returned from putmsg()\n");
			}
		}
	}

	exit(0);
}
