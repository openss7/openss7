/*****************************************************************************

 @(#) $RCSfile: test-sctps.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2002/05/22 14:34:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@dallas.net>

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

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2002/05/22 14:34:53 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctps.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2002/05/22 14:34:53 $"

static char const ident[] =
    "$RCSfile: test-sctps.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2002/05/22 14:34:53 $";

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
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

void usage(void)
{
	fprintf(stderr, "Usage:  test-sctps [options]\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -p, --port port           (default: 10000)\n");
	fprintf(stderr,
		"      port specifies both the local and remote port number\n");
	fprintf(stderr, "  -l, --loc_host loc_host   (default: 0.0.0.0)\n");
	fprintf(stderr,
		"      loc_host specifies the local (bind) host for the SCTP\n");
	fprintf(stderr, "      socket with optional local port number\n");
	fprintf(stderr, "  -t, --rep_time time       (default: 1 second)\n");
	fprintf(stderr, "      time give the time in seconds between reports\n");
	fprintf(stderr, "  -w, --length              (default: %d)\n", MSG_LEN);
	fprintf(stderr, "      packet length\n");
	fprintf(stderr, "  -n, --nagle               (default: nonagle)\n");
	fprintf(stderr, "      enable nagle algorithm\n");
}

#define HOST_BUF_LEN 256

static int timer_timeout = 0;

static void timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int timer_sethandler(void)
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

static int start_timer(void)
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
static struct sockaddr_in rem_addr = { AF_INET, 0, {INADDR_ANY}, };

int len = MSG_LEN;

int nodelay = 1;

int test_sctps(void)
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

	if ((lfd = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0) {
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
	if (setsockopt(fd, SOL_SCTP, SCTP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
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
			     out_bytes, inp_bytes, out_bytes + inp_bytes,
			     inp_bytes - out_bytes);
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
				if ((rtn =
				     send(fd, my_msg, offset, MSG_DONTWAIT)) < 0) {
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
					  sizeof(ur_msg) - offset,
					  MSG_DONTWAIT)) < 0) {
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

int main(int argc, char **argv)
{
	int c;
	char *hostl = "0.0.0.0";
	char hostbufl[HOST_BUF_LEN];
	char hostbufr[HOST_BUF_LEN];
	char **hostlp = &hostl;
	short port = 10000;
	int time;
	struct hostent *haddr;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"loc_host", 1, 0, 'l'},
			{"rep_time", 1, 0, 't'},
			{"help", 0, 0, 'h'},
			{"port", 1, 0, 'p'},
			{"length", 1, 0, 'w'},
			{"nagle", 0, 0, 'n'}
		};
		c = getopt_long(argc, argv, "l:r:t:hp:w:n", long_options,
				&option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:	/* loc_host */
				strncpy(hostbufl, optarg, HOST_BUF_LEN);
				hostl = hostbufl;
				hostlp = &hostl;
				break;
			case 2:	/* rep_time */
				time = atoi(optarg);
				break;
			case 3:	/* help */
				usage();
				exit(0);
			case 4:	/* port */
				port = atoi(optarg);
				break;
			case 5:	/* length */
				len = atoi(optarg);
				if (len > 2048) {
					len = 2048;
				}
				break;
			case 6:	/* nagle */
				nodelay = 0;
				break;
			default:
				usage();
				exit(1);
			}
			break;
		case 'l':
			strncpy(hostbufl, optarg, HOST_BUF_LEN);
			hostl = hostbufl;
			hostlp = &hostl;
			break;
		case 't':
			time = atoi(optarg);
			break;
		case 'h':
			usage();
			exit(0);
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
		default:
			fprintf(stderr, "ERROR: Unrecognized option `%c'.\n", c);
			usage();
			exit(1);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "ERROR: Option syntax: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
		usage();
		exit(1);
	}

	haddr = gethostbyname(*hostlp);
	loc_addr.sin_family = AF_INET;
	loc_addr.sin_port = htons(port);
	loc_addr.sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);

	return test_sctps();
}
