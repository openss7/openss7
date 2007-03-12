/*****************************************************************************

 @(#) $RCSfile: test-sctp_nc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/12 02:23:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2007/03/12 02:23:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp_nc.c,v $
 Revision 0.9.2.4  2007/03/12 02:23:15  brian
 - updating tests

 Revision 0.9.2.3  2005/05/14 08:31:27  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_nc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/12 02:23:15 $"

static char const ident[] = "$RCSfile: test-sctp_nc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/12 02:23:15 $";

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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <signal.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#define BUFSIZE 4096

int verbose = 1;

union {
	np_ulong prim;
	union N_primitives npi;
	char cbuf[BUFSIZE];
} cmd;

char dbuf[BUFSIZE];
struct strbuf ctrl = { BUFSIZE, 0, cmd.cbuf };
struct strbuf data = { BUFSIZE, 0, dbuf };

typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[1] __attribute__ ((packed));
} addr_t;

N_qos_sel_data_sctp_t qos_data = {
	N_QOS_SEL_DATA_SCTP,		/* n_qos_type */
	10,				/* ppi */
	0,				/* sid */
	0,				/* ssn */
	0,				/* tsn */
	0				/* more */
};

N_qos_sel_conn_sctp_t qos_conn = {
	N_QOS_SEL_CONN_SCTP,		/* n_qos_type */
	1,				/* i_streams */
	1				/* o_streams */
};

N_qos_sel_info_sctp_t qos_info = {
	N_QOS_SEL_INFO_SCTP,		/* n_qos_type */
	1,				/* i_streams */
	1,				/* o_streams */
	10,				/* ppi */
	0,				/* sid */
	12,				/* max_inits */
	12,				/* max_retrans */
	-1,				/* ck_life */
	-1,				/* ck_inc */
	-1,				/* hmac */
	-1,				/* throttle */
	20,				/* max_sack */
	100,				/* rto_ini */
	20,				/* rto_min */
	200,				/* rto_max */
	5,				/* rtx_path */
	200,				/* hb_itvl */
	0x10				/* options */
};

#define HOST_BUF_LEN 256

int rep_time = 1;

static volatile int timer_timeout = 0;

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

int
sctp_get(int fd, int wait)
{
	int ret;
	int flags = 0;

	while ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		switch (errno) {
		default:
		case EPROTO:
		case EINVAL:
			perror("sctp_get: getmsg");
			return -1;
		case EINTR:
		case ERESTART:
			continue;
		case EAGAIN:
			break;
		}
		break;
	}
	if (!ret)
		return cmd.prim;
	if (!wait) {
		/* 
		   fprintf(stderr,"sctp_get: no data and no wait specified\n"); */
		return -1;
	}
	do {
		struct pollfd pfd[] = { {fd, POLLIN | POLLPRI, 0} };

		if (!(ret = poll(pfd, 1, wait))) {
			perror("sctp_get: poll");
			return -1;
		}
		if ((ret == 1) | (ret == 2)) {
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				flags = 0;
				while ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
					switch (errno) {
					case EINTR:
					case ERESTART:
						continue;
					}
				}
				if (ret < 0) {
					perror("sctp_get: getmsg");
					return -1;
				}
				return cmd.prim;
			}
			continue;
		}
		if (ret == -1) {
			perror("sctp_get: poll");
			return -1;
		}
	}
	while (1);
}

int
sctp_options(int fd, ulong flags, N_qos_sel_info_sctp_t * qos)
{
	int ret;

	ctrl.len = sizeof(cmd.npi.optmgmt_req) + sizeof(*qos);
	cmd.prim = N_OPTMGMT_REQ;
	cmd.npi.optmgmt_req.OPTMGMT_flags = flags;
	cmd.npi.optmgmt_req.QOS_length = sizeof(*qos);
	cmd.npi.optmgmt_req.QOS_offset = sizeof(cmd.npi.optmgmt_req);
	bcopy(qos, (cmd.cbuf + sizeof(cmd.npi.optmgmt_req)), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, NULL, MSG_HIPRI)) < 0) {
		switch (errno) {
		case EINTR:
		case ERESTART:
			continue;
		}
		break;
	}
	if (ret < 0) {
		perror("sctp_options: putmsg");
		return -1;
	}
	if ((ret = sctp_get(fd, 0)) < 0) {
		fprintf(stderr, "sctp_options: couldn't get message\n");
		return -1;
	}
	if (ret != N_OK_ACK) {
		fprintf(stderr, "sctp_options: didn't get N_OK_ACK\n");
		return -1;
	}
	fprintf(stderr, "Options set\n");
	return 0;
}

int
sctp_bind(int fd, addr_t * addr, int coninds)
{
	int ret;

	ctrl.len = sizeof(cmd.npi.bind_req) + sizeof(*addr);
	cmd.prim = N_BIND_REQ;
	cmd.npi.bind_req.ADDR_length = sizeof(*addr);
	cmd.npi.bind_req.ADDR_offset = sizeof(cmd.npi.bind_req);
	cmd.npi.bind_req.CONIND_number = coninds;
	cmd.npi.bind_req.BIND_flags = TOKEN_REQUEST;
	cmd.npi.bind_req.PROTOID_length = 0;
	cmd.npi.bind_req.PROTOID_offset = 0;
	bcopy(addr, (&cmd.npi.bind_req) + 1, sizeof(*addr));
	while ((ret = putmsg(fd, &ctrl, NULL, MSG_HIPRI)) < 0) {
		switch (errno) {
		case EINTR:
		case ERESTART:
			continue;
		}
		break;
	}
	if (ret < 0) {
		perror("sctp_bind: putmsg");
		return -1;
	}
	if ((ret = sctp_get(fd, 0)) < 0) {
		fprintf(stderr, "sctp_bind: couldn't get message\n");
		return -1;
	}
	if (ret != N_BIND_ACK) {
		fprintf(stderr, "sctp_bind: didn't get N_BIND_ACK\n");
		return -1;
	}
	fprintf(stderr, "STREAM bound\n");
	return 0;
}

int
sctp_connect(int fd, addr_t * addr, N_qos_sel_conn_sctp_t * qos)
{
	int ret;

	ctrl.len = sizeof(cmd.npi.conn_req) + sizeof(*addr) + sizeof(*qos);
	cmd.prim = N_CONN_REQ;
	cmd.npi.conn_req.DEST_length = sizeof(*addr);
	cmd.npi.conn_req.DEST_offset = sizeof(cmd.npi.conn_req);
	cmd.npi.conn_req.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_req.QOS_length = sizeof(*qos);
	cmd.npi.conn_req.QOS_offset = sizeof(cmd.npi.conn_req) + sizeof(*addr);
	bcopy(addr, (cmd.cbuf + sizeof(cmd.npi.conn_req)), sizeof(*addr));
	bcopy(qos, (cmd.cbuf + sizeof(cmd.npi.conn_req) + sizeof(*addr)), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, NULL, 0)) < 0) {
		switch (errno) {
		case EINTR:
		case ERESTART:
			continue;
		}
		break;
	}
	if (ret < 0) {
		perror("sctp_connect: putmsg");
		return -1;
	}
	if ((ret = sctp_get(fd, 10000)) < 0) {
		fprintf(stderr, "sctp_connect: couldn't get message\n");
		return -1;
	}
	if (ret != N_CONN_CON) {
		fprintf(stderr, "sctp_connect: didn't get N_CONN_CON\n");
		return -1;
	}
	fprintf(stderr, "STREAM connected\n");
	return 0;
}

int
sctp_accept(int fd, int fd2, int tok, int seq)
{
	int ret;

	ctrl.len = sizeof(cmd.npi.conn_res);
	cmd.prim = N_CONN_RES;
	cmd.npi.conn_res.TOKEN_value = tok;
	cmd.npi.conn_res.RES_offset = 0;
	cmd.npi.conn_res.RES_length = 0;
	cmd.npi.conn_res.SEQ_number = seq;
	cmd.npi.conn_res.CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	cmd.npi.conn_res.QOS_offset = 0;
	cmd.npi.conn_res.QOS_length = 0;
	while ((ret = putmsg(fd, &ctrl, NULL, 0)) < 0) {
		switch (errno) {
		case EINTR:
		case ERESTART:
			continue;
		}
		break;
	}
	if (ret < 0) {
		perror("sctp_accept: putmsg");
		return -1;
	}
	if ((ret = sctp_get(fd, 0)) < 0) {
		fprintf(stderr, "sctp_accept: couldn't get message\n");
		return -1;
	}
	if (ret != N_OK_ACK) {
		fprintf(stderr, "sctp_accept: didn't get N_OK_ACK\n");
		return -1;
	}
	fprintf(stderr, "Connection Indication accepted\n");
	return 0;
}

int
sctp_write(int fd, void *msg, size_t len, int flags, N_qos_sel_data_sctp_t * qos)
{
	int ret = 0;

	data.buf = msg;
	data.len = len;
	data.maxlen = len;
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(*qos);
	cmd.prim = N_DATA_REQ;
	cmd.npi.data_req.DATA_xfer_flags = flags;
	bcopy(qos, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, &data, 0)) < 0) {
		switch (errno) {
		case EINTR:
		case ERESTART:
			continue;
		}
		break;
	}
	if (ret < 0) {
		if (errno != EAGAIN)
			perror("sctp_write: putmsg");
		return -1;
	}
	return (0);
}

int
sctp_read(int fd, void *msg, size_t len)
{
	int ret;

	data.buf = msg;
	data.len = 0;
	data.maxlen = len;
	if ((ret = sctp_get(fd, 0)) < 0) {
		/* 
		   fprintf(stderr,"sctp_read: couldn't get message\n"); */
		return -1;
	}
	if (ret != N_DATA_IND) {
		fprintf(stderr, "sctp_read: didn't get N_DATA_IND\n");
		return -1;
	}
	return 0;
}

static addr_t loc_addr = { 0, {{INADDR_ANY}} };
static addr_t rem_addr = { 0, {{INADDR_ANY}} };

int len = 32;

#ifdef LFS
static const char sctpname[] = "/dev/streams/clone/sctp_n";
#else
static const char sctpname[] = "/dev/sctp_n";
#endif

int
test_sctpc(void)
{
	int ret;
	int fd;
	int flags = 0;
	long inp_count = 0, out_count = 0;
	struct pollfd pfd[1] = { {0, POLLIN | POLLOUT | POLLERR | POLLHUP, 0} };
	unsigned char my_msg[] = "This is a good short test message that has some 64 bytes in it.";

	// unsigned char ur_msg[100];

	fprintf(stderr, "Opening stream\n");

	if ((fd = open(sctpname, O_NONBLOCK | O_RDWR)) < 0) {
		perror("open");
		goto dead;
	}
	if (ioctl(fd, I_SRDOPT, RMSGD) < 0) {
		perror("ioctl");
		goto dead;
	}
	fprintf(stderr, "--> STREAM opened\n");
	if (sctp_options(fd, flags, &qos_info) < 0)
		goto dead;
	fprintf(stderr, "--> Options set\n");
	if (sctp_bind(fd, &loc_addr, 0) < 0)
		goto dead;
	fprintf(stderr, "--> STREAM bound\n");
	if (sctp_connect(fd, &rem_addr, &qos_conn) < 0)
		goto dead;
	fprintf(stderr, "--> STREAM connected\n");

	if (start_timer()) {
		perror("timer");
		goto dead;
	}
	fprintf(stderr, "--> Timer started\n");

	for (;;) {
		pfd[0].fd = fd;
		pfd[0].events = POLLOUT;
		pfd[0].revents = 0;
		if (timer_timeout) {
			printf("Msgs sent: %5ld, recv: %5ld, tot: %5ld, dif: %5ld, tput: %10ld\n", inp_count, out_count, inp_count + out_count, out_count - inp_count, 8 * (42 + len) * (inp_count + out_count));
			inp_count = 0;
			out_count = 0;
			if (start_timer()) {
				perror("timer");
				goto dead;
			}
		}
		if (poll(&pfd[0], 1, -1) < 0) {
			switch (errno) {
			case EINTR:
			case ERESTART:
				continue;
			}
			perror("poll");
			goto dead;
		}
		if (pfd[0].revents & POLLOUT) {
			while ((ret = sctp_write(fd, my_msg, len, 0, &qos_data)) >= 0)
				++out_count;
			if (ret < 0) {
				if (errno != EAGAIN) {
					fprintf(stderr, "sctp_write: couldn't write message\n");
					goto dead;
				}
				pfd[0].revents &= ~POLLOUT;
			}
		}
	}
      dead:
	close(fd);
	return (0);
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
RFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Suite\n\
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
Regulations (\"FAR\") (or any successor regulations) or, in the  cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
");
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    %1$s {-C, --copying}\n\
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
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -p, --port=PORT\n\
        Specifies both the local and remote PORT number\n\
    -l, --loc_host=LOCAL-HOST\n\
        Specifies the  LOCAL-HOST (bind) for the SCTP socket with optional\n\
        local port number\n\
    -r, --rem_host=REMOTE-HOST\n\
        Specifies the REMOTE-HOST (connect) address for the SCTP socket\n\
        with optional remote port number\n\
    -t, --rep_time=REPORT-TIME\n\
        Specifies the REPORT-TIME in seconds between reports\n\
    -w, --length=LENGTH\n\
        Specifies the message LENGTH\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL [default: 1]\n\
        This option may be repeated.\n\
    -h, --help, -?, --?\n\
        Prints this usage message and exits\n\
    -V, --version\n\
        Prints the version and exits\n\
    -C, --copying\n\
        Prints copyright and permissions and exits\n\
", argv[0]);
}

int
main(int argc, char **argv)
{
	char *hostl = "127.0.0.1";
	char *hostr = "127.0.0.1";
	char hostbufl[HOST_BUF_LEN];
	char hostbufr[HOST_BUF_LEN];
	char **hostlp = &hostl;
	char **hostrp = &hostr;
	short portl = 10000;
	short portr = 10001;
	struct hostent *haddr;

	for (;;) {
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
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL, }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "l:r:t:p:w:qvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "l:r:t:p:w:qvhVC?");
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
			portl = atoi(optarg);
			portr = portl;
			break;
		case 'w':
			len = atoi(optarg);
			if (len > 1024)
				len = 1024;
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

	haddr = gethostbyname(*hostlp);
	loc_addr.port = htons(portl);
	loc_addr.addr[0].s_addr = *(uint32_t *) (haddr->h_addr);

	haddr = gethostbyname(*hostrp);
	rem_addr.port = htons(portr);
	rem_addr.addr[0].s_addr = *(uint32_t *) (haddr->h_addr);

	return test_sctpc();
}
