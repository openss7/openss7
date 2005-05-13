/*****************************************************************************

 @(#) $RCSfile: test-sctp_nc.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/13 11:15:50 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@openss7.org>

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
 any successor regulations) or, in the cases of NASA, in paragraph 18.52.227-86
 of the NASA Supplement to the FAR (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/13 11:15:50 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $rcsfile: test-sctp_nc.c,v $ $name:  $($revision: 0.9.2.1 $) $date: 2004/02/22 08:51:37 $"

static char const ident[] =
    "$rcsfile: test-sctp_nc.c,v $ $name:  $($revision: 0.9.2.1 $) $date: 2004/02/22 08:51:37 $";

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

#ifdef _gnu_source
#include <getopt.h>
#endif

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#define bufsize 4096

int verbose = 1;

union {
	np_ulong prim;
	union n_primitives npi;
	char cbuf[bufsize];
} cmd;

char dbuf[bufsize];
struct strbuf ctrl = { bufsize, 0, cmd.cbuf };
struct strbuf data = { bufsize, 0, dbuf };

typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[1] __attribute__ ((packed));
} addr_t;

n_qos_sel_data_sctp_t qos_data = {
	n_qos_sel_data_sctp,		/* n_qos_type */
	10,				/* ppi */
	0,				/* sid */
	0,				/* ssn */
	0,				/* tsn */
	0				/* more */
};

n_qos_sel_conn_sctp_t qos_conn = {
	n_qos_sel_conn_sctp,		/* n_qos_type */
	1,				/* i_streams */
	1				/* o_streams */
};

n_qos_sel_info_sctp_t qos_info = {
	n_qos_sel_info_sctp,		/* n_qos_type */
	1,				/* i_streams */
	1,				/* o_streams */
	10,				/* ppi */
	0,				/* sid */
	12,				/* max_inits */
	12,				/* max_retrans */
	-1ul,				/* ck_life */
	-1ul,				/* ck_inc */
	-1ul,				/* hmac */
	-1ul,				/* throttle */
	20,				/* max_sack */
	100,				/* rto_ini */
	20,				/* rto_min */
	200,				/* rto_max */
	5,				/* rtx_path */
	200,				/* hb_itvl */
	0x10				/* options */
};

#define host_buf_len 256

int rep_time = 1;

static volatile int timer_timeout = 0;

static void
timer_handler(int signum)
{
	if (signum == sigalrm)
		timer_timeout = 1;
	return;
}

static int
timer_sethandler(void)
{
	sigset_t mask;
	struct sigaction act;
	act.sa_handler = timer_handler;
	act.sa_flags = sa_restart | sa_oneshot;
	act.sa_restorer = null;
	sigemptyset(&act.sa_mask);
	if (sigaction(sigalrm, &act, null))
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, sigalrm);
	sigprocmask(sig_unblock, &mask, null);
	return 0;
}

static int
start_timer(void)
{
	struct itimerval setting = { {0, 0}, {rep_time, 0} };
	if (timer_sethandler())
		return -1;
	if (setitimer(itimer_real, &setting, null))
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
		case eproto:
		case einval:
			perror("sctp_get: getmsg");
			return -1;
		case eintr:
		case erestart:
			continue;
		case eagain:
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
		struct pollfd pfd[] = { {fd, pollin | pollpri, 0} };
		if (!(ret = poll(pfd, 1, wait))) {
			perror("sctp_get: poll");
			return -1;
		}
		if ((ret == 1) | (ret == 2)) {
			if (pfd[0].revents & (pollin | pollpri)) {
				flags = 0;
				while ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
					switch (errno) {
					case eintr:
					case erestart:
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
sctp_options(int fd, ulong flags, n_qos_sel_info_sctp_t * qos)
{
	int ret;
	ctrl.len = sizeof(cmd.npi.optmgmt_req) + sizeof(*qos);
	cmd.prim = n_optmgmt_req;
	cmd.npi.optmgmt_req.optmgmt_flags = flags;
	cmd.npi.optmgmt_req.qos_length = sizeof(*qos);
	cmd.npi.optmgmt_req.qos_offset = sizeof(cmd.npi.optmgmt_req);
	bcopy(qos, (cmd.cbuf + sizeof(cmd.npi.optmgmt_req)), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, null, msg_hipri)) < 0) {
		switch (errno) {
		case eintr:
		case erestart:
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
	if (ret != n_ok_ack) {
		fprintf(stderr, "sctp_options: didn't get n_ok_ack\n");
		return -1;
	}
	fprintf(stderr, "options set\n");
	return 0;
}

int
sctp_bind(int fd, addr_t * addr, int coninds)
{
	int ret;
	ctrl.len = sizeof(cmd.npi.bind_req) + sizeof(*addr);
	cmd.prim = n_bind_req;
	cmd.npi.bind_req.addr_length = sizeof(*addr);
	cmd.npi.bind_req.addr_offset = sizeof(cmd.npi.bind_req);
	cmd.npi.bind_req.conind_number = coninds;
	cmd.npi.bind_req.bind_flags = token_request;
	cmd.npi.bind_req.protoid_length = 0;
	cmd.npi.bind_req.protoid_offset = 0;
	bcopy(addr, (&cmd.npi.bind_req) + 1, sizeof(*addr));
	while ((ret = putmsg(fd, &ctrl, null, msg_hipri)) < 0) {
		switch (errno) {
		case eintr:
		case erestart:
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
	if (ret != n_bind_ack) {
		fprintf(stderr, "sctp_bind: didn't get n_bind_ack\n");
		return -1;
	}
	fprintf(stderr, "stream bound\n");
	return 0;
}

int
sctp_connect(int fd, addr_t * addr, n_qos_sel_conn_sctp_t * qos)
{
	int ret;
	ctrl.len = sizeof(cmd.npi.conn_req) + sizeof(*addr) + sizeof(*qos);
	cmd.prim = n_conn_req;
	cmd.npi.conn_req.dest_length = sizeof(*addr);
	cmd.npi.conn_req.dest_offset = sizeof(cmd.npi.conn_req);
	cmd.npi.conn_req.conn_flags = rec_conf_opt | ex_data_opt;
	cmd.npi.conn_req.qos_length = sizeof(*qos);
	cmd.npi.conn_req.qos_offset = sizeof(cmd.npi.conn_req) + sizeof(*addr);
	bcopy(addr, (cmd.cbuf + sizeof(cmd.npi.conn_req)), sizeof(*addr));
	bcopy(qos, (cmd.cbuf + sizeof(cmd.npi.conn_req) + sizeof(*addr)), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, null, 0)) < 0) {
		switch (errno) {
		case eintr:
		case erestart:
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
	if (ret != n_conn_con) {
		fprintf(stderr, "sctp_connect: didn't get n_conn_con\n");
		return -1;
	}
	fprintf(stderr, "stream connected\n");
	return 0;
}

int
sctp_accept(int fd, int fd2, int tok, int seq)
{
	int ret;
	ctrl.len = sizeof(cmd.npi.conn_res);
	cmd.prim = n_conn_res;
	cmd.npi.conn_res.token_value = tok;
	cmd.npi.conn_res.res_offset = 0;
	cmd.npi.conn_res.res_length = 0;
	cmd.npi.conn_res.seq_number = seq;
	cmd.npi.conn_res.conn_flags = rec_conf_opt | ex_data_opt;
	cmd.npi.conn_res.qos_offset = 0;
	cmd.npi.conn_res.qos_length = 0;
	while ((ret = putmsg(fd, &ctrl, null, 0)) < 0) {
		switch (errno) {
		case eintr:
		case erestart:
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
	if (ret != n_ok_ack) {
		fprintf(stderr, "sctp_accept: didn't get n_ok_ack\n");
		return -1;
	}
	fprintf(stderr, "connection indication accepted\n");
	return 0;
}

int
sctp_write(int fd, void *msg, size_t len, int flags, n_qos_sel_data_sctp_t * qos)
{
	int ret = 0;
	data.buf = msg;
	data.len = len;
	data.maxlen = len;
	ctrl.len = sizeof(cmd.npi.data_req) + sizeof(*qos);
	cmd.prim = n_data_req;
	cmd.npi.data_req.data_xfer_flags = flags;
	bcopy(qos, cmd.cbuf + sizeof(cmd.npi.data_req), sizeof(*qos));
	while ((ret = putmsg(fd, &ctrl, &data, 0)) < 0) {
		switch (errno) {
		case eintr:
		case erestart:
			continue;
		}
		break;
	}
	if (ret < 0) {
		if (errno != eagain)
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
	if (ret != n_data_ind) {
		fprintf(stderr, "sctp_read: didn't get n_data_ind\n");
		return -1;
	}
	return 0;
}

static addr_t loc_addr = { 0, {{inaddr_any}} };
static addr_t rem_addr = { 0, {{inaddr_any}} };

int len = 32;

int
test_sctpc(void)
{
	int ret;
	int fd;
	int flags = 0;
	long inp_count = 0, out_count = 0;
	struct pollfd pfd[1] = { {0, pollin | pollout | pollerr | pollhup, 0} };
	unsigned char my_msg[] = "this is a good short test message that has some 64 bytes in it.";
	// unsigned char ur_msg[100];

	fprintf(stderr, "opening stream\n");

	if ((fd = open("/dev/sctp_n", o_nonblock | o_rdwr)) < 0) {
		perror("open");
		goto dead;
	}
	if (ioctl(fd, i_srdopt, rmsgd) < 0) {
		perror("ioctl");
		goto dead;
	}
	fprintf(stderr, "--> stream opened\n");
	if (sctp_options(fd, flags, &qos_info) < 0)
		goto dead;
	fprintf(stderr, "--> options set\n");
	if (sctp_bind(fd, &loc_addr, 0) < 0)
		goto dead;
	fprintf(stderr, "--> stream bound\n");
	if (sctp_connect(fd, &rem_addr, &qos_conn) < 0)
		goto dead;
	fprintf(stderr, "--> stream connected\n");

	if (start_timer()) {
		perror("timer");
		goto dead;
	}
	fprintf(stderr, "--> timer started\n");

	for (;;) {
		pfd[0].fd = fd;
		pfd[0].events = pollout;
		pfd[0].revents = 0;
		if (timer_timeout) {
			printf("msgs sent: %5ld, recv: %5ld, tot: %5ld, dif: %5ld, tput: %10ld\n",
			       inp_count, out_count, inp_count + out_count, out_count - inp_count,
			       8 * (42 + len) * (inp_count + out_count));
			inp_count = 0;
			out_count = 0;
			if (start_timer()) {
				perror("timer");
				goto dead;
			}
		}
		if (poll(&pfd[0], 1, -1) < 0) {
			switch (errno) {
			case eintr:
			case erestart:
				continue;
			}
			perror("poll");
			goto dead;
		}
		if (pfd[0].revents & pollout) {
			while ((ret = sctp_write(fd, my_msg, len, 0, &qos_data)) >= 0)
				++out_count;
			if (ret < 0) {
				if (errno != eagain) {
					fprintf(stderr, "sctp_write: couldn't write message\n");
					goto dead;
				}
				pfd[0].revents &= ~pollout;
			}
		}
	}
      dead:
	close(fd);
	return (0);
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
rfc 2960 sctp - openss7 streams sctp - conformance test suite\n\
\n\
copyright (c) 2001-2005 openss7 corporation <http://www.openss7.com/>\n\
copyright (c) 1997-2001 brian f. g. bidulock <bidulock@openss7.org>\n\
\n\
all rights reserved.\n\
\n\
unauthorized distribution or duplication is prohibited.\n\
\n\
this software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
no part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
the recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
the author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   the author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
as an exception to the above,  this software may be  distributed  under the  gnu\n\
general public license  (gpl)  version 2  or later,  so long as  the software is\n\
distributed with,  and only used for the testing of,  openss7 modules,  drivers,\n\
and libraries.\n\
\n\
u.s. government restricted rights.  if you are licensing this software on behalf\n\
of the  u.s. government  (\"government\"),  the following provisions apply to you.\n\
if the software is  supplied by the department of defense (\"dod\"), it is classi-\n\
fied as  \"commercial computer software\"  under paragraph 252.227-7014 of the dod\n\
supplement  to the  federal acquisition regulations  (\"dfars\") (or any successor\n\
regulations) and the  government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-government  users).  if\n\
the software is supplied to any unit or agency of the government other than dod,\n\
it is classified as  \"restricted computer software\" and the  government's rights\n\
in the  software are defined in  paragraph 52.227-19 of the federal  acquisition\n\
regulations  (\"far\") (or any success  regulations) or, in the  cases of nasa, in\n\
paragraph  18.52.227-86 of the  nasa supplement  to the  far (or  any  successor\n\
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
    copyright (c) 2001-2005  openss7 corporation.  all rights reserved.\n\
\n\
    distributed by openss7 corporation under gpl version 2,\n\
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
        specifies both the local and remote PORT number\n\
    -l, --loc_host=LOCAL-HOST\n\
        specifies the  LOCAL-HOST (bind) for the SCTP socket with optional\n\
        local port number\n\
    -r, --rem_host=REMOTE-HOST\n\
        specifies the REMOTE-HOST (connect) address for the SCTP socket\n\
        with optional remote port number\n\
    -t, --rep_time=REPORT-TIME\n\
        specifies the REPORT-TIME in seconds between reports\n\
    -w, --length=LENGTH\n\
        specifies the message LENGTH\n\
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
			{ 0, }
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
