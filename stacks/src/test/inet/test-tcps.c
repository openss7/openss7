/*****************************************************************************

 @(#) $RCSfile: test-tcps.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/03/12 11:17:55 $

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

 Last Modified $Date: 2007/03/12 11:17:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-tcps.c,v $
 Revision 0.9.2.7  2007/03/12 11:17:55  brian
 - rationalize sctp test programs

 Revision 0.9.2.6  2007/03/12 09:33:49  brian
 - boosted default test port numbers from 10000 to 18000

 Revision 0.9.2.5  2007/03/12 02:23:17  brian
 - updating tests

 Revision 0.9.2.4  2005/05/14 08:31:28  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-tcps.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/03/12 11:17:55 $"

static char const ident[] = "$RCSfile: test-tcps.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/03/12 11:17:55 $";

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define MSG_LEN 64

#define HOST_BUF_LEN 256

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
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return 0;
}

static int
start_timer(void)
{
	struct itimerval setting = { {0, 0}, {1, 0} };

	if (timer_sethandler())
		return -1;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return -1;
	timer_timeout = 0;
	return 0;
}

static struct sockaddr_in loc_addr = { AF_INET, 0, {INADDR_ANY}, };

int len = MSG_LEN;

int nodelay = 1;

int
test_tcps(void)
{
	int lfd, fd;
	int offset = 0, mode = 0;
	long inp_count = 0, out_count = 0;
	long inp_bytes = 0, out_bytes = 0;
	struct pollfd pfd[1] = { {0, POLLIN | POLLOUT | POLLERR | POLLHUP, 0} };

//      unsigned char my_msg[] = "This is a good short test message that has some 64 bytes in it.";
	unsigned char ur_msg[len];
	unsigned char *my_msg = ur_msg;

	fprintf(stderr, "Opening socket\n");

	if ((lfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		goto dead1;
	}

	fprintf(stderr, "Binding socket to %s:%d\n", inet_ntoa(loc_addr.sin_addr),
		ntohs(loc_addr.sin_port));

	if (bind(lfd, (struct sockaddr *) &loc_addr, sizeof(loc_addr)) < 0) {
		perror("bind");
		goto dead1;
	}
	if (listen(lfd, 1) < 0) {
		perror("listen");
		goto dead1;
	}
	if ((fd = accept(lfd, NULL, NULL)) < 0) {
		perror("accept");
		goto dead1;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl");
		goto dead;
	}
	if (setsockopt(fd, SOL_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
		perror("setsockopt");
		goto dead;
	}
	if (start_timer()) {
		perror("timer");
		goto dead;
	}

	for (;;) {
		pfd[0].fd = fd;
		pfd[0].events = (mode ? POLLOUT : POLLIN) | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		if (timer_timeout) {
			printf
			    ("Bytes sent: %7ld, recv: %7ld, tot: %7ld, dif: %8ld\n",
			     out_bytes, inp_bytes, out_bytes + inp_bytes, inp_bytes - out_bytes);
			inp_count = 0;
			out_count = 0;
			inp_bytes = 0;
			out_bytes = 0;
			if (start_timer()) {
				perror("timer");
				goto dead;
			}
			continue;
		}
		if (poll(&pfd[0], 1, -1) < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			perror("poll");
			goto dead;
		}
		if (mode) {
			if (pfd[0].revents & POLLOUT) {
				int rtn;

				if ((rtn = send(fd, my_msg, offset, MSG_DONTWAIT)) < 0) {
					if (errno == EINTR || errno == EAGAIN)
						continue;
					perror("send");
					goto dead;
				}
				if (rtn) {
					out_bytes += rtn;
					offset = offset > rtn ? offset - rtn : 0;
					my_msg += rtn;
					if (!offset) {
						out_count++;
						mode = 0;
					}
				}
				continue;
			}
		} else {
			if (pfd[0].revents & POLLIN) {
				int rtn;

				if ((rtn =
				     recv(fd, ur_msg + offset,
					  sizeof(ur_msg) - offset, MSG_DONTWAIT)) < 0) {
					if (errno == EINTR || errno == EAGAIN)
						continue;
					perror("recv");
					goto dead;
				}
				if (rtn) {
					inp_bytes += rtn;
					offset += rtn;
					if (offset == len) {
						my_msg = ur_msg;
						inp_count++;
						mode = 1;
					}
				}
				continue;
			}
		}
		if (pfd[0].revents & POLLERR) {
			perror("POLLERR");
			goto dead;
		}
		if (pfd[0].revents & POLLHUP) {
			perror("POLLHUP");
			goto dead;
		}
	}
      dead:
	fprintf(stderr, "Error number: %d\n", errno);
	close(fd);
      dead1:
	close(lfd);
	return (0);
}

static int verbose = 1;

void
splash(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stdout, "\
%1$s: TCP Performance Test Program\n\
%2$s\n\
\n\
Copyright (c) 2001-2007 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
", argv[0], ident);
}

void
version(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (verbose <= 0)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

#define TEST_PORT_NUMBER 18000

void
help(int argc, char *argv[])
{
	if (verbose <= 0)
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
    -l, --loc_host\n\
        Local host (bind) address            [default: 0.0.0.0]\n\
    -p, --port PORTNUM\n\
        Remote port (connect) number         [default: %3$d]\n\
    -w, --length LENGTH\n\
        Length of message in bytes           [default: %2$d]\n\
    -n, --nagle\n\
        Suppress Nagle algorithm\n\
    -t, --rep_time INTERVAL\n\
        Sets the report time INTERVAL (secs) [default: 1]\n\
    -q, --quiet\n\
        Suppress normal output\n\
        (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL   [default 1]\n\
        This option may be repeated\n\
    -h, --help\n\
        Prints this usage message and exists\n\
    -V, --version\n\
        Prints the version and exits\n\
    -C, --copying\n\
	Prints copyright and copying information and exits\n\
", argv[0], MSG_LEN, TEST_PORT_NUMBER);
}

int
main(int argc, char **argv)
{
	char *hostl = "0.0.0.0";
	char hostbufl[HOST_BUF_LEN];
	char **hostlp = &hostl;
	short port = TEST_PORT_NUMBER;
	int time;
	struct hostent *haddr;

	while (1) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"loc_host",	required_argument,	NULL, 'l'},
			{"rep_time",	required_argument,	NULL, 't'},
			{"port",	required_argument,	NULL, 'p'},
			{"length",	required_argument,	NULL, 'w'},
			{"nagle",	no_argument,		NULL, 'n'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "l:r:p:w:nt:qvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "l:r:p:t:qvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'l':
			strncpy(hostbufl, optarg, HOST_BUF_LEN);
			hostl = hostbufl;
			hostlp = &hostl;
			break;
		case 't':
			time = atoi(optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'w':
			len = atoi(optarg);
			if (len > 2048)
				len = 2048;
			break;
		case 'n':
			nodelay = 0;
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
		case 'H':
		case 'h':
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case 'C':
			splash(argc, argv);
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
			usage(argc, argv);
			exit(2);
		}
	}
	if (optind < argc)
		goto bad_nonopt;

	splash(argc, argv);

	haddr = gethostbyname(*hostlp);
	loc_addr.sin_family = AF_INET;
	loc_addr.sin_port = htons(port);
	loc_addr.sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);

	return test_tcps();
}
