/*****************************************************************************

 @(#) $RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/10/19 01:53:28 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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
 General Public License (GPL) Version 2 or later, so long as the software is
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

 Last Modified $Date: 2005/10/19 01:53:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: perftest.c,v $
 Revision 0.9.2.3  2005/10/19 01:53:28  brian
 - some alterations to perf tests

 Revision 0.9.2.2  2005/10/18 08:55:27  brian
 - added Linux native pipe test

 Revision 0.9.2.1  2005/10/18 03:10:12  brian
 - pipe performance tests

 Revision 0.9.2.3  2005/05/14 08:31:24  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/10/19 01:53:28 $"

static char const ident[] = "$RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/10/19 01:53:28 $";

/*
 *  These are benchmark performance tests on a pipe for testing LiS
 *  performance patches.
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
#include <sys/uio.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define MAXMSGSIZE 32768
int verbose = 1;
int msgsize = 64;
int report = 1;
#ifdef NATIVE_PIPES
int native = 1;
int readwrite = 1;
#else
int native = 0;
int readwrite = 0;
#endif
int push = 0;
int blocking = 0;
char my_msg[MAXMSGSIZE] = { 0, };

volatile int timer_timeout = 0;

void
timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

int
timer_sethandler(void)
{
	sigset_t mask;
	struct sigaction act;
	act.sa_handler = timer_handler;
	act.sa_flags = SA_RESTART | SA_ONESHOT;
	act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return (0);
}

int
start_timer(void)
{
	struct itimerval setting = { {0, 0}, {report, 0} };
	if (timer_sethandler())
		return (-1);
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return (-1);
	timer_timeout = 0;
	return 0;
}

/*
   first child process sits and reads 
 */
int
read_child(int fd)
{
	int bytcnt = 0, avg_msgs = 0, avg_tput = 0;
	struct pollfd pfd = { fd, POLLIN | POLLERR | POLLHUP, 0 };
	if (verbose > 1)
		fprintf(stderr, "Starting timer\n");
	if (start_timer()) {
		if (verbose)
			perror("start_timer()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Timer started\n");
	for (;;) {
		if (timer_timeout) {
			int thrput = bytcnt / report;
			int msgcnt = thrput / msgsize;
			avg_msgs = (2 * avg_msgs + msgcnt) / 3;
			avg_tput = (2 * avg_tput + thrput) / 3;
			fprintf(stdout, "%d Msgs read: %ld (%ld), throughput: %ld (%ld)\n", fd,
					(long)msgcnt,
					(long)avg_msgs,
					(long)thrput,
					(long)avg_tput);
			fflush(stdout);
			bytcnt = 0;
			if (start_timer()) {
				if (verbose)
					perror("start_timer()");
				goto dead;
			}
		}
		if (poll(&pfd, 1, -1) < 0) {
			switch (errno) {
			case EINTR:
			case ERESTART:
				continue;
			}
			if (verbose)
				perror("poll()");
			goto dead;
		}
		if (pfd.revents & POLLIN) {
			int ret;
			if (readwrite) {
				while (!timer_timeout && (ret = read(fd, my_msg, msgsize)) > 0) {
					bytcnt += ret;
					if (bytcnt < 0)
						goto dead;
				}
			} else {
				int flags;
				struct strbuf cbuf = { -1, 0, my_msg };
				struct strbuf dbuf = { msgsize, 0, my_msg };
				while (!timer_timeout && (ret = getmsg(fd, &cbuf, &dbuf, &flags)) != -1) {
					bytcnt += dbuf.len;
					if (bytcnt < 0)
						goto dead;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
				case EINTR:
					break;
				default:
					if (verbose)
						perror("read()");
					goto dead;
				}
			}
			pfd.revents &= ~POLLIN;
		}
		if (pfd.revents & POLLHUP)
			goto done;
		if (pfd.revents & POLLERR)
			goto dead;
	}
      dead:
	close(fd);
	return (1);
      done:
	close(fd);
	return (0);
}

/*
   second child process sits and writes 
 */
int
write_child(int fd)
{
	int bytcnt = 0, avg_msgs = 0, avg_tput = 0;
	struct pollfd pfd = { fd, POLLOUT | POLLERR | POLLHUP, 0 };
	if (verbose > 1)
		fprintf(stderr, "Starting timer\n");
	if (start_timer()) {
		if (verbose)
			perror("start_timer()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Timer started\n");
	for (;;) {
		if (timer_timeout) {
			int thrput = bytcnt / report;
			int msgcnt = thrput / msgsize;
			avg_msgs = (2 * avg_msgs + msgcnt) / 3;
			avg_tput = (2 * avg_tput + thrput) / 3;
			fprintf(stdout, "%d Msgs sent: %ld (%ld), throughput: %ld (%ld)\n", fd,
					(long)msgcnt,
					(long)avg_msgs,
					(long)thrput,
					(long)avg_tput);
			fflush(stdout);
			bytcnt = 0;
			if (start_timer()) {
				if (verbose)
					perror("start_timer()");
				goto dead;
			}
		}
		if (poll(&pfd, 1, -1) < 0) {
			switch (errno) {
			case EINTR:
			case ERESTART:
				continue;
			}
			if (verbose)
				perror("poll()");
			goto dead;
		}
		if (pfd.revents & POLLOUT) {
			int ret;
			if (readwrite) {
				while (!timer_timeout && (ret = write(fd, my_msg, msgsize)) > 0) {
					bytcnt += ret;
					if (bytcnt < 0)
						goto dead;
				}
			} else {
				struct strbuf dbuf = { 0, msgsize, my_msg };
				while (!timer_timeout && (ret = putmsg(fd, NULL, &dbuf, 0)) != -1) {
					bytcnt += msgsize;
					if (bytcnt < 0)
						goto dead;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
				case EINTR:
					break;
				default:
					if (verbose)
						perror("write()");
					goto dead;
				}
			}
			pfd.revents &= ~POLLOUT;
		}
		if (pfd.revents & POLLHUP)
			goto done;
		if (pfd.revents & POLLERR)
			goto dead;
	}
      dead:
	close(fd);
	return (1);
      done:
	close(fd);
	return (0);
}

int
do_tests(void)
{
	int fds[2] = { 0, 0 };
	int children[2] = { 0, 0 };

	if (verbose > 1) {
		fprintf(stderr, "Opening pipe\n");
		fflush(stderr);
	}
	if (pipe(fds) != 0) {
		if (verbose)
			perror("pipe()");
		goto dead;
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Pipe opened.\n");
	}
#if 0
	if (!native) {
		if (verbose > 1) {
			fprintf(stderr, "Setting options on fd %d\n", fds[0]);
		}
		if (ioctl(fds[0], I_SRDOPT, RMSGD) < 0) {
			if (verbose)
				perror("ioctl(I_SRDOPT)");
			goto dead;
		}
	}
#endif
	if (!blocking) {
		if (fcntl(fds[0], F_SETFL, O_NONBLOCK) < 0) {
			if (verbose)
				perror("fcntl(O_NONBLOCK)");
			goto dead;
		}
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Options set.\n");
	}
#if 0
	if (!native) {
		if (verbose > 1) {
			fprintf(stderr, "Setting options on fd %d\n", fds[1]);
		}
		if (ioctl(fds[1], I_SRDOPT, RMSGD) < 0) {
			if (verbose)
				perror("ioctl(I_SRDOPT)");
			goto dead;
		}
	}
#endif
	if (!blocking) {
		if (fcntl(fds[1], F_SETFL, O_NONBLOCK) < 0) {
			if (verbose)
				perror("fcntl(O_NONBLOCK)");
			goto dead;
		}
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Options set.\n");
	}
	if (!native) {
		int i;
		if (verbose > 1) {
			fprintf(stderr, "Pushing %d instances of pipemod on %d\n", push, fds[0]);
		}
		for (i = 0; i < push; i++) {
			if (ioctl(fds[0], I_PUSH, "pipemod") < 0) {
				if (verbose)
					perror("ioctl(I_PUSH)");
				goto dead;
			}
		}
		if (verbose > 1) {
			fprintf(stderr, "--> Pipemod pushed.\n");
		}
	}
	if (verbose > 1) {
		fprintf(stderr, "Starting read child on fd %d\n", fds[0]);
	}
	switch ((children[0] = fork())) {
	case 0:		/* child */
		exit(read_child(fds[0]));
	case -1:
		if (verbose)
			perror("fork()");
		goto dead;
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Read child started.\n");
		fprintf(stderr, "Starting write child on fd %d\n", fds[1]);
	}
	switch ((children[1] = fork())) {
	case 0:		/* child */
		exit(write_child(fds[1]));
	case -1:
		if (verbose)
			perror("fork()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Write child started.\n");
	close(fds[0]);
	close(fds[1]);
	for (;;) {
		int child;
		int status;

		if ((child = wait(&status)) > 0) {
			if (WIFEXITED(status)) {
				if (children[0] == child)
					children[0] = 0;
				if (children[1] == child)
					children[1] = 0;
				if (WEXITSTATUS(status) != 0)
					goto dead;
			} else if (WIFSIGNALED(status)) {
				if (children[0] == child)
					children[0] = 0;
				if (children[1] == child)
					children[1] = 0;
				switch (WTERMSIG(status)) {
				default:
					goto dead;
				}
			} else if (WIFSTOPPED(status)) {
				if (children[0] == child)
					children[0] = 0;
				if (children[1] == child)
					children[1] = 0;
				switch (WSTOPSIG(status)) {
				default:
					goto dead;
				}
			}
		}
	}
      dead:
	if (children[0] > 0)
		kill(children[0], SIGTERM);
	if (children[1] > 0)
		kill(children[1], SIGTERM);
	close(fds[0]);
	close(fds[1]);
	return (0);
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
LiS Benchmark Performance Tests on a Pipe\n\
\n\
Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
");
	fflush(stdout);
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2003-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    -p, --push=[COUNT]\n\
        push COUNT instances of pipemod [default: %5$d]\n\
    -b, --blocking\n\
        Use blocking operation on read and write\n\
    -s, --size=[MSGSIZE]\n\
        Packet size to be used for testing [default: %2$d]\n\
    -r, --readwrite\n\
        Use read/write instead of putmsg/getmsg\n\
    -t, --time=[REPORT]\n\
        Report time in seconds [default: %3$d]\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose=[LEVEL]\n\
        Increase verbosity or set to LEVEL [default: %4$d]\n\
        This option may be repeated.\n\
    -h, --help, -?, --?\n\
        Prints this usage message and exists\n\
    -V, --version\n\
        Prints the version and exists\n\
", argv[0], msgsize, report, verbose, push);
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
			{"push",	required_argument,	NULL, 'p'},
			{"blocking",	no_argument,		NULL, 'b'},
			{"size",	required_argument,	NULL, 's'},
			{"readwrite",	no_argument,		NULL, 'r'},
			{"time",	required_argument,	NULL, 't'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"?",		no_argument,		NULL, 'h'},
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "p:bs:rt:qvhV?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "p:bs:rt:qvhV?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'p':
			if (optarg == NULL) {
				push++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			push = val;
			break;
		case 'b':
			blocking = 1;
			break;
		case 's':
			msgsize = strtol(optarg, NULL, 0);
			if (1 > msgsize || msgsize > MAXMSGSIZE)
				goto bad_option;
			break;
		case 'r':
			readwrite = 1;
			break;
		case 't':
			report = strtol(optarg, NULL, 0);
			if (1 > report)
				goto bad_option;
			break;
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
	splash(argc, argv);
	do_tests();
	exit(0);
}
