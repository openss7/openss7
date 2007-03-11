/*****************************************************************************

 @(#) $RCSfile: test-tcpc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/07/18 11:56:35 $

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

 Last Modified $Date: 2005/07/18 11:56:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-tcpc.c,v $
 Revision 0.9.2.6  2005/07/18 11:56:35  brian
 - standard indentation

 Revision 0.9.2.5  2005/05/14 08:29:35  brian
 - copyright header correction

 Revision 1.5  2005/05/14 08:29:35  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-tcpc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/07/18 11:56:35 $"

static char const ident[] =
    "$RCSfile: test-tcpc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/07/18 11:56:35 $";

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
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LEN 64
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define HOST_BUF_LEN 256

int rep_time = 1;

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
	struct itimerval setting = { {0, 0}, {rep_time, 0} };

	if (timer_sethandler())
		return -1;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return -1;
	timer_timeout = 0;
	return 0;
}

static struct sockaddr_in loc_addr = { AF_INET, 0, {INADDR_ANY}, };
static struct sockaddr_in rem_addr = { AF_INET, 0, {INADDR_ANY}, };

int len = MSG_LEN;

int nodelay = 1;

int
test_tcpc(void)
{
	int fd;
	int out_offset = 0, inp_offset = 0;
	long rtt_delay = 0;
	long inp_count = 0, out_count = 0;
	long inp_bytes = 0, out_bytes = 0;
	struct pollfd pfd[1] = { {0, POLLIN | POLLOUT | POLLERR | POLLHUP, 0} };
	unsigned char my_msg[] =
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.\0"
	    "This is a good short test message that has some 64 bytes in it.";
	unsigned char ur_msg[2048];

	fprintf(stderr, "Opening socket\n");
	if ((fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		goto dead;
	}
	fprintf(stderr, "Binding socket to %s:%d\n", inet_ntoa(loc_addr.sin_addr),
		ntohs(loc_addr.sin_port));
	if (bind(fd, (struct sockaddr *) &loc_addr, sizeof(loc_addr)) < 0) {
		perror("bind");
		goto dead;
	}
	if (connect(fd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("connect");
		goto dead;
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
		pfd[0].events = POLLOUT | POLLIN | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		if (timer_timeout) {
			printf
			    ("Bytes sent: %7ld, recv: %7ld, tot: %7ld, dif: %8ld dly: %6ld\n",
			     out_bytes, inp_bytes, out_bytes + inp_bytes,
			     inp_bytes - out_bytes, inp_count ? rtt_delay / inp_count : 0);
			inp_count = 0;
			out_count = 0;
			inp_bytes = 0;
			out_bytes = 0;
			rtt_delay = 0;
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
		if (pfd[0].revents & POLLIN) {
			int rtn;

			if ((rtn = recv(fd, ur_msg + inp_offset, len - inp_offset,
					MSG_DONTWAIT)) < 0) {
				if (errno == EINTR || errno == EAGAIN)
					goto skip_pollin;
				perror("recv");
				goto dead;
			}
			if (rtn) {
				inp_bytes += rtn;
				inp_offset += rtn;
				if (inp_offset >= len) {
					struct timeval tnow;
					struct timeval *tv = (struct timeval *) ur_msg;

					if (gettimeofday(&tnow, NULL) < 0) {
						perror("gettimeofday");
						goto dead;
					}
					if (tnow.tv_sec < tv->tv_sec
					    || (tnow.tv_sec == tv->tv_sec
						&& tnow.tv_usec < tv->tv_usec))
						fprintf(stderr,
							"time: %ld.%06ld before %ld.%06ld\n",
							tnow.tv_sec, tnow.tv_usec,
							tv->tv_sec, tv->tv_usec);
					rtt_delay +=
					    (tnow.tv_sec - tv->tv_sec) * 1000000 +
					    tnow.tv_usec - tv->tv_usec;
					inp_count++;
					inp_offset = 0;
				}
			}
			continue;
		}
	      skip_pollin:
		if (pfd[0].revents & POLLOUT) {
			int rtn;

			if (!out_offset) {
				struct timeval *tv = (struct timeval *) my_msg;

				if (gettimeofday(tv, NULL) < 0) {
					perror("gettimeofday");
					goto dead;
				}
			}
			if ((rtn = send(fd, my_msg + out_offset, len - out_offset,
					MSG_DONTWAIT)) < 0) {
				if (errno == EINTR || errno == EAGAIN)
					goto skip_pollout;
				perror("send");
				goto dead;
			}
			if (rtn) {
				out_bytes += rtn;
				out_offset += rtn;
				if (out_offset >= len) {
					out_count++;
					out_offset = 0;
				}
			}
			continue;
		}
	      skip_pollout:
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
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    -p, --port PORT           (default: 10000)\n\
        port specifies both the local and remote port number\n\
    -l, --loc_host LOC_HOST   (default: 127.0.0.1)\n\
        specifies the LOC_HOST (bind) host for the TCP\n\
        socket with optional local port number\n\
    -r, --rem_host REM_HOST   (default: 127.0.0.2)\n\
        specifies the REM_HOST (sendto) address for the TCP\n\
        socket with optional remote port number\n\
    -t, --rep_time TIME       (default: 1 second)\n\
        specify the TIME in seconds between reports\n\
    -w, --length LENGTH       (default: 32 bytes)\n\
        specify the LENGTH of messages\n\
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
	char *hostl = "0.0.0.0";
	char *hostr = "127.0.0.2";
	char hostbufl[HOST_BUF_LEN];
	char hostbufr[HOST_BUF_LEN];
	char **hostlp = &hostl;
	char **hostrp = &hostr;
	short port = 10000;
	struct hostent *haddr;

	while (1) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"loc_host",	required_argument,	NULL, 'l'},
			{"rem_host",	required_argument,	NULL, 'r'},
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
		case 'r':
			strncpy(hostbufr, optarg, HOST_BUF_LEN);
			hostr = hostbufr;
			hostrp = &hostr;
			break;
		case 't':
			rep_time = atoi(optarg);
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
		case 'H':	/* -H */
		case 'h':	/* -h, --help */
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
	loc_addr.sin_port = 0;
	loc_addr.sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);

	haddr = gethostbyname(*hostrp);
	rem_addr.sin_family = AF_INET;
	rem_addr.sin_port = htons(port);
	rem_addr.sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);

	return test_tcpc();
}
