/*****************************************************************************

 @(#) $RCSfile: ss7capd.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/12 16:20:49 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/12 16:20:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ss7capd.c,v $
 Revision 0.9.2.3  2007/08/12 16:20:49  brian
 - new PPA handling

 Revision 0.9.2.2  2007/01/21 20:22:41  brian
 - working up drivers

 Revision 0.9.2.1  2007/01/15 11:33:57  brian
 - added new and old signalling link utilities

 *****************************************************************************/

#ident "@(#) $RCSfile: ss7capd.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/12 16:20:49 $"

static char const ident[] =
    "$RCSfile: ss7capd.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/12 16:20:49 $";

#include <stropts.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

int output = 1;
int debug = 0;
int nomead = 0;

int card = 0;
int span = 0;
int slot = 1;

int cap_fd = -1;

int link_state = 0;

char outfile[256] = "";
char errfile[256] = "";
char outpath[256] = "";
char errpath[256] = "";
char lnkname[256] = "";
char cfgfile[256] = "";
char outpdir[256] = "/var/log/ss7capd";
char devname[256] = "/dev/x400p-sl";

#define BUFSIZE 512

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
struct strbuf data = { sizeof(*dbuf), 0, dbuf };

struct strioctl ctl;

struct capconfig {
	lmi_option_t opt;
	sdl_config_t sdl;
	sdt_config_t sdt;
} capconf;

uint32_t iftype = SDL_TYPE_DS0A;
uint32_t ifrate = 56000;

void
version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

void
usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-f|--cfgfile} cfgfile\n\
    %1$s [options] {-e|--device} devname {-c|--card} card {-s|--span} span {-t|--slot} slot\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
", argv[0]);
}

void
help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-f|--cfgfile} cfgfile\n\
    %1$s [options] {-e|--device} devname {-c|--card} card {-s|--span} span {-t|--slot} slot\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
  Command Options:\n\
    -c, --card card                     (default: %2$d)\n\
        card number (starting at 0)\n\
    -s, --span span                     (default: %3$d)\n\
        span number (starting at 0)\n\
    -t, --slot slot                     (default: %4$d)\n\
        timeslot number (starting at 1)\n\
  Capture Options:\n\
    -d, --daemon                        (default: off)\n\
        run in the background as a daemon\n\
    -O, --outpdir outpdir               (default: %5$s)\n\
        output directory for capture and errors\n\
    -n, --lnkname lnkname               (default: ss7capd$$)\n\
        link name\n\
    -o, --outfile outfile               (default: ${lnkname}.out)\n\
        redirect output to outfile (always as daemon)\n\
    -l, --logfile errfile               (default: ${lnkname}.err)\n\
        redirect errors to errfile (always as daemon)\n\
    -f, --cfgfile cfgfile               (default: ss7capd.conf)\n\
        configuration file name\n\
    -e, --device devname                (default: %6$s)\n\
        device name to open\n\
    -a, --ds0a                          (default: ds0a)\n\
        channel is ds0a rather than ds0\n\
    -0, --ds0                           (default: ds0a)\n\
        channel is ds0 rather than ds0a\n\
  General Options:\n\
    -q, --quiet                         (default: off)\n\
        suppress output\n\
    -D, --debug [LEVEL]                 (default: %7$d)\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]               (default: %8$d)\n\
        increase or set verbosity of output\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
    -C, --copying\n\
        prints copying permissions and exits\n\
", argv[0], card, span, slot, outpdir, devname, debug, output);
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; version  2  of  the  License.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received a copy of the GNU  General  Public License  along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

/* events */
enum {
	CAP_NONE = 0,
	CAP_SUCCESS = 0,
	CAP_TIMEOUT,
	CAP_PCPROTO,
	CAP_PROTO,
	CAP_DATA,
	CAP_FAILURE = -1,
};

int alm_signal = 0;
int hup_signal = 0;
int trm_signal = 0;

void
sig_handler(int signum)
{
	switch (signum) {
	case SIGALRM:
		alm_signal = 1;
		break;
	case SIGHUP:
		hup_signal = 1;
		break;
	case SIGTERM:
		trm_signal = 1;
		break;
	}
	return;
}

int
sig_register(int signum, void (*handler) (int))
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = handler ? handler : SIG_DFL;
	act.sa_flags = handler ? SA_RESTART : 0;
	act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(signum, &act, NULL))
		return CAP_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, signum);
	sigprocmask(handler ? SIG_UNBLOCK : SIG_BLOCK, &mask, NULL);
	return CAP_SUCCESS;
}

int
hup_catch(void)
{
	return sig_register(SIGHUP, sig_handler);
}

int
alm_catch(void)
{
	return sig_register(SIGALRM, sig_handler);
}

int
trm_catch(void)
{
	return sig_register(SIGTERM, sig_handler);
}

void
sig_catch(void)
{
	trm_catch();
	hup_catch();
	alm_catch();
}

int
hup_block(void)
{
	return sig_register(SIGHUP, NULL);
}

int
alm_block(void)
{
	return sig_register(SIGALRM, NULL);
}

int
trm_block(void)
{
	return sig_register(SIGTERM, NULL);
}

void
sig_block(void)
{
	alm_block();
	hup_block();
	trm_block();
}

int cap_stop(void);

void
cap_exit(int retval)
{
	syslog(LOG_NOTICE, "Exiting %d", retval);
	fflush(stdout);
	fflush(stderr);
	cap_stop();
	sig_block();
	closelog();
	exit(retval);
}

static struct timeval tv;

void
fprint_time(FILE * stream)
{
	if (0 > tv.tv_usec || tv.tv_usec > 1000000) {
		tv.tv_sec += (tv.tv_usec / 1000000);
		tv.tv_usec %= 1000000;
	}
	fprintf(stream, "%012ld.%06ld", tv.tv_sec, tv.tv_usec);
};
int
ftimestamp(void)
{
	while (gettimeofday(&tv, NULL) < 0)
		if (errno == EAGAIN || errno == EINTR || errno == ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not read timestamp: winging it%s", outpath);
			break;
		}
	return CAP_SUCCESS;
}

/* generate output header */
void
output_header(void)
{
	char buf[128] = "";
	struct utsname uts;

	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout,
		" # SS7CAPD $Id: ss7capd.c,v 0.9.2.3 2007/08/12 16:20:49 brian Exp $ Output File Header\n");
	uname(&uts);
	fprint_time(stdout);
	fprintf(stdout, " # machine: %s %s %s %s %s\n", uts.sysname, uts.nodename, uts.release,
		uts.version, uts.machine);
	fprint_time(stdout);
	fprintf(stdout, " # device: %s (%d:%d:%d) %s\n", devname, card, span, slot, lnkname);
	fprint_time(stdout);
	if (outpath[0] != '\0')
		snprintf(buf, sizeof(buf), outpath);
	else
		snprintf(buf, sizeof(buf), "(stdout)");
	fprintf(stdout, " # original file name: %s\n", buf);
	fprint_time(stdout);
	gethostname(buf, sizeof(buf));
	fprintf(stdout, " # host: %s\n", buf);
	fprint_time(stdout);
	fprintf(stdout, " # date: %s", ctime(&tv.tv_sec));
}

void
trm_action(void)
{
	syslog(LOG_WARNING, "Caught SIGTERM, shutting down");
	cap_exit(0);
}

int
hup_action(void)
{
	syslog(LOG_WARNING, "Caught SIGHUP, reopening files.");
	if (output > 1)
		syslog(LOG_NOTICE, "Reopening output file %s", outpath);
	if (outpath[0] != '\0') {
		fflush(stdout);
		fclose(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not reopen stdout file %s", outpath);
		}
		output_header();
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reopening error file %s", errpath);
	if (errpath[0] != '\0') {
		fflush(stderr);
		fclose(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not reopen stderr file %s", errpath);
		}
	}
	return (0);
}

int
alm_action(void)
{
	return (0);
}

int
start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (alm_catch())
		return CAP_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return CAP_FAILURE;
	alm_signal = 0;
	return CAP_SUCCESS;
}

int
stop_timer(void)
{
	return alm_block();
}

void
print_data(void)
{
	static unsigned char hexchar[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	int i;

	for (i = 0; i < data.len; i++) {
		fputc(hexchar[(dbuf[i] >> 4) & 0x0f], stdout);
		fputc(hexchar[(dbuf[i] >> 0) & 0x0f], stdout);
	}
}
int
decode_data(void)
{
	ftimestamp();
	fprint_time(stdout);
	fputc(' ', stdout);
	print_data();
	fputc('\n', stdout);
	return CAP_DATA;
}

int
decode_ctrl(int flag)
{
	if (output > 2)
		fprintf(stderr, "Got control meesage! %d\n", *(uint32_t *) cbuf);
	switch (*(uint32_t *) cbuf) {
	case SDT_RC_SIGNAL_UNIT_IND:
		decode_data();
		return CAP_NONE;	/* go around again */
	case LMI_INFO_ACK:
	case LMI_OK_ACK:
	case LMI_ERROR_ACK:
	case LMI_ENABLE_CON:
	case LMI_DISABLE_CON:
	case LMI_OPTMGMT_ACK:
		break;
	case LMI_ERROR_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # LMI_ERROR_IND\n");
		return CAP_NONE;
	case LMI_STATS_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # LMI_STATS_IND\n");
		return CAP_NONE;
	case LMI_EVENT_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # LMI_EVENT_IND\n");
		return CAP_NONE;
	case SDT_RC_CONGESTION_ACCEPT_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_RC_CONGESTION_ACCEPT_IND\n");
		return CAP_NONE;
	case SDT_RC_CONGESTION_DISCARD_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_RC_CONGESTION_DISCARD_IND\n");
		return CAP_NONE;
	case SDT_RC_NO_CONGESTION_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_RC_NO_CONGESTION_IND\n");
		return CAP_NONE;
	case SDT_IAC_CORRECT_SU_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_IAC_CORRECT_SU_IND\n");
		return CAP_NONE;
	case SDT_IAC_ABORT_PROVING_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_IAC_ABORT_PROVING_IND\n");
		return CAP_NONE;
	case SDT_LSC_LINK_FAILURE_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_LSC_LINK_FAILURE_IND\n");
		return CAP_NONE;
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		ftimestamp();
		fprint_time(stdout);
		fprintf(stdout, " # SDT_TXC_TRANSMISSION_REQUEST_IND\n");
		return CAP_NONE;
	default:
		break;
	}
	if (flag)
		return CAP_PCPROTO;
	return CAP_PROTO;
}

int
wait_event(int wait)
{
	for (;;) {
		struct pollfd pfd[] = {
			{cap_fd, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
		};

		if (trm_signal) {
			trm_signal = 0;
			trm_action();
		}
		if (hup_signal) {
			hup_signal = 0;
			hup_action();
		}
		if (alm_signal) {
			alm_signal = 0;
			alm_action();
			return CAP_TIMEOUT;
		}
		if (output > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, 1, wait)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			syslog(LOG_ERR, "%s: %m", __FUNCTION__);
			cap_exit(1);
			return CAP_FAILURE;
		case 0:
			return CAP_NONE;
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int ret, flags = 0;

				ctrl.maxlen = BUFSIZE;
				ctrl.len = 0;
				ctrl.buf = cbuf;
				data.maxlen = BUFSIZE;
				data.len = 0;
				data.buf = dbuf;
				if (getmsg(cap_fd, &ctrl, &data, &flags) < 0) {
					syslog(LOG_ERR, "%s: getmsg error", __FUNCTION__);
					syslog(LOG_ERR, "%s: %m", __FUNCTION__);
					cap_exit(1);
					break;
				}
				if (ctrl.len > 0) {
					if ((ret = decode_ctrl(flags)) == CAP_NONE)
						continue;
					return ret;
				}
				if (data.len > 0) {
					if ((ret = decode_data()) == CAP_NONE)
						continue;
					return ret;
				}
				break;
			}
			if (pfd[0].revents & POLLNVAL) {
				syslog(LOG_ERR, "%s: device invalid", __FUNCTION__);
				cap_exit(1);
				return CAP_FAILURE;
			}
			if (pfd[0].revents & POLLHUP) {
				syslog(LOG_ERR, "%s: device hangup", __FUNCTION__);
				cap_exit(1);
				return CAP_FAILURE;
			}
			if (pfd[0].revents & POLLERR) {
				syslog(LOG_ERR, "%s: device error", __FUNCTION__);
				cap_exit(1);
				return CAP_FAILURE;
			}
			break;
		default:
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			cap_exit(1);
			return CAP_FAILURE;
		}
	}
}

int
cap_open(void)
{
	if (output > 1)
		syslog(LOG_NOTICE, "opening %s", devname);
	if ((cap_fd = open(devname, O_NONBLOCK | O_RDWR)) < 0) {
		syslog(LOG_ERR, "%s: couldn't open devname: %s", __FUNCTION__, devname);
		syslog(LOG_ERR, "%s: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	if (output > 1)
		syslog(LOG_NOTICE, "configuring %s", devname);
	if (ioctl(cap_fd, I_SRDOPT, RMSGD) < 0) {
		syslog(LOG_ERR, "%s: couldn't configure devname: %s", __FUNCTION__, devname);
		syslog(LOG_ERR, "%s: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	return CAP_SUCCESS;
}

int
cap_attach(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	unsigned short ppa = (card << 12) | (span << 8) | (slot << 0);
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	p->attach_req.lmi_ppa_length = sizeof(ppa);
	p->attach_req.lmi_ppa_offset = sizeof(p->attach_req);
	bcopy(&ppa, cbuf + sizeof(p->attach_req), sizeof(ppa));
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	for (;;) {
		switch ((ret = wait_event(100))) {
		case CAP_NONE:
			syslog(LOG_ERR, "%s: error, no response to LMI_ATTACH_REQ", __FUNCTION__);
			cap_exit(1);
			return CAP_FAILURE;
		case CAP_PCPROTO:
			switch (p->lmi_primitive) {
			case LMI_OK_ACK:
				return CAP_SUCCESS;
			case LMI_ERROR_ACK:
				syslog(LOG_ERR, "%s: lmi_errno: %s", __FUNCTION__,
				       strerror(p->error_ack.lmi_errno));
				syslog(LOG_ERR, "%s: lmi_reason: 0x%08x", __FUNCTION__,
				       p->error_ack.lmi_reason);
				syslog(LOG_ERR, "%s: lmi_error_primitive: %d", __FUNCTION__,
				       p->error_ack.lmi_error_primitive);
				cap_exit(1);
				return CAP_FAILURE;
			default:
				syslog(LOG_ERR,
				       "%s: error, unexpected response to LMI_ATTACH_REQ %d",
				       __FUNCTION__, p->lmi_primitive);
				cap_exit(1);
				return CAP_FAILURE;
			}
		default:
			syslog(LOG_ERR, "%s: error, unexpected response to LMI_ATTACH_REQ %d",
			       __FUNCTION__, ret);
			cap_exit(1);
		case CAP_FAILURE:
			return CAP_FAILURE;
		}
	}
}

int
cap_enable(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->enable_req);
	ctrl.buf = cbuf;
	p->enable_req.lmi_primitive = LMI_ENABLE_REQ;
	p->enable_req.lmi_rem_length = 0;
	p->enable_req.lmi_rem_offset = sizeof(p->enable_req);
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	for (;;) {
		switch ((ret = wait_event(100))) {
		case CAP_NONE:
			syslog(LOG_ERR, "%s: error, no response to LMI_ENABLE_REQ", __FUNCTION__);
			cap_exit(1);
			return CAP_FAILURE;
		case CAP_PCPROTO:
			if (p->lmi_primitive == LMI_ENABLE_CON || p->lmi_primitive == LMI_OK_ACK)
				return CAP_SUCCESS;
			syslog(LOG_ERR, "%s: error, unexpected response to LMI_ENABLE_REQ %u",
			       __FUNCTION__, p->lmi_primitive);
			cap_exit(1);
			return CAP_FAILURE;
		default:
			syslog(LOG_ERR, "%s: error, unexpected response to LMI_ENABLE_REQ %d",
			       __FUNCTION__, ret);
			cap_exit(1);
		case CAP_FAILURE:
			return CAP_FAILURE;
		}
	}
}

void
show_sdl_config(void)
{
	fprintf(stderr, "\tifname: %s", capconf.sdl.ifname);
	fprintf(stderr, "\n\tifflags:");
	if (capconf.sdl.ifflags & SDL_IF_UP)
		fprintf(stderr, " up");
	if (capconf.sdl.ifflags & SDL_IF_RX_RUNNING)
		fprintf(stderr, " rx");
	if (capconf.sdl.ifflags & SDL_IF_TX_RUNNING)
		fprintf(stderr, " tx");
	fprintf(stderr, "\n\tiftype:");
	switch (capconf.sdl.iftype) {
	case SDL_TYPE_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_TYPE_V35:
		fprintf(stderr, " v35");
		break;
	case SDL_TYPE_DS0:
		fprintf(stderr, " ds0");
		break;
	case SDL_TYPE_DS0A:
		fprintf(stderr, " ds0a");
		break;
	case SDL_TYPE_E1:
		fprintf(stderr, " e1");
		break;
	case SDL_TYPE_T1:
		fprintf(stderr, " t1");
		break;
	case SDL_TYPE_ATM:
		fprintf(stderr, " atm");
		break;
	case SDL_TYPE_PACKET:
		fprintf(stderr, " packet");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.iftype);
		break;
	}
	fprintf(stderr, "\n\tifrate: %u", capconf.sdl.ifrate);
	fprintf(stderr, "\n\tifgtype:");
	switch (capconf.sdl.ifgtype) {
	case SDL_GTYPE_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_GTYPE_T1:
		fprintf(stderr, " t1");
		break;
	case SDL_GTYPE_E1:
		fprintf(stderr, " e1");
		break;
	case SDL_GTYPE_J1:
		fprintf(stderr, " j1");
		break;
	case SDL_GTYPE_ATM:
		fprintf(stderr, " atm");
		break;
	case SDL_GTYPE_ETH:
		fprintf(stderr, " eth");
		break;
	case SDL_GTYPE_IP:
		fprintf(stderr, " ip");
		break;
	case SDL_GTYPE_UDP:
		fprintf(stderr, " udp");
		break;
	case SDL_GTYPE_TCP:
		fprintf(stderr, " tcp");
		break;
	case SDL_GTYPE_RTP:
		fprintf(stderr, " rtp");
		break;
	case SDL_GTYPE_SCTP:
		fprintf(stderr, " sctp");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifgtype);
		break;
	}
	fprintf(stderr, "\n\tifgrate: %u", capconf.sdl.ifgrate);
	fprintf(stderr, "\n\tifmode:");
	switch (capconf.sdl.ifmode) {
	case SDL_MODE_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_MODE_DSU:
		fprintf(stderr, " dsu");
		break;
	case SDL_MODE_CSU:
		fprintf(stderr, " csu");
		break;
	case SDL_MODE_DTE:
		fprintf(stderr, " dte");
		break;
	case SDL_MODE_DCE:
		fprintf(stderr, " dce");
		break;
	case SDL_MODE_CLIENT:
		fprintf(stderr, " client");
		break;
	case SDL_MODE_SERVER:
		fprintf(stderr, " server");
		break;
	case SDL_MODE_PEER:
		fprintf(stderr, " peer");
		break;
	case SDL_MODE_ECHO:
		fprintf(stderr, " echo");
		break;
	case SDL_MODE_REM_LB:
		fprintf(stderr, " rem_lb");
		break;
	case SDL_MODE_LOC_LB:
		fprintf(stderr, " loc_lb");
		break;
	case SDL_MODE_LB_ECHO:
		fprintf(stderr, " lb_echo");
		break;
	case SDL_MODE_TEST:
		fprintf(stderr, " test");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifmode);
		break;
	}
	fprintf(stderr, "\n\tifgmode:");
	switch (capconf.sdl.ifgmode) {
	case SDL_GMODE_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_GMODE_LOC_LB:
		fprintf(stderr, " loc_lb");
		break;
	case SDL_GMODE_REM_LB:
		fprintf(stderr, " rem_lb");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifgmode);
		break;
	}
	fprintf(stderr, "\n\tifgcrc:");
	switch (capconf.sdl.ifgcrc) {
	case SDL_GCRC_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_GCRC_CRC4:
		fprintf(stderr, " crc4");
		break;
	case SDL_GCRC_CRC5:
		fprintf(stderr, " crc5");
		break;
	case SDL_GCRC_CRC6:
		fprintf(stderr, " crc6");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifgcrc);
		break;
	}
	fprintf(stderr, "\n\tifclock:");
	switch (capconf.sdl.ifclock) {
	case SDL_CLOCK_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_CLOCK_INT:
		fprintf(stderr, " int");
		break;
	case SDL_CLOCK_EXT:
		fprintf(stderr, " ext");
		break;
	case SDL_CLOCK_LOOP:
		fprintf(stderr, " loop");
		break;
	case SDL_CLOCK_MASTER:
		fprintf(stderr, " master");
		break;
	case SDL_CLOCK_SLAVE:
		fprintf(stderr, " slave");
		break;
	case SDL_CLOCK_DPLL:
		fprintf(stderr, " dpll");
		break;
	case SDL_CLOCK_ABR:
		fprintf(stderr, " abr");
		break;
	case SDL_CLOCK_SHAPER:
		fprintf(stderr, " shaper");
		break;
	case SDL_CLOCK_TICK:
		fprintf(stderr, " tick");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifclock);
		break;
	}
	fprintf(stderr, "\n\tifcoding:");
	switch (capconf.sdl.ifcoding) {
	case SDL_CODING_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_CODING_NRZ:
		fprintf(stderr, " nrz");
		break;
	case SDL_CODING_NRZI:
		fprintf(stderr, " nrzi");
		break;
	case SDL_CODING_AMI:
		fprintf(stderr, " ami");
		break;
	case SDL_CODING_B6ZS:
		fprintf(stderr, " b6zs");
		break;
	case SDL_CODING_B8ZS:
		fprintf(stderr, " b8zs");
		break;
	case SDL_CODING_HDB3:
		fprintf(stderr, " hdb3");
		break;
	case SDL_CODING_AAL1:
		fprintf(stderr, " aal1");
		break;
	case SDL_CODING_AAL2:
		fprintf(stderr, " aal2");
		break;
	case SDL_CODING_AAL5:
		fprintf(stderr, " aal5");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifcoding);
		break;
	}
	fprintf(stderr, "\n\tifframing:");
	switch (capconf.sdl.ifframing) {
	case SDL_FRAMING_NONE:
		fprintf(stderr, " none");
		break;
	case SDL_FRAMING_CCS:
		fprintf(stderr, " ccs");
		break;
	case SDL_FRAMING_CAS:
		fprintf(stderr, " cas");
		break;
	case SDL_FRAMING_SF:
		fprintf(stderr, " sf");
		break;
	case SDL_FRAMING_ESF:
		fprintf(stderr, " esf");
		break;
	default:
		fprintf(stderr, " [%u]", capconf.sdl.ifframing);
		break;
	}
	fprintf(stderr, "\n\tifblksize: %u", capconf.sdl.ifblksize);
	fprintf(stderr, "\n\tifleads:");
	if (capconf.sdl.ifleads & SDL_LEAD_DTR)
		fprintf(stderr, " dtr");
	if (capconf.sdl.ifleads & SDL_LEAD_RTS)
		fprintf(stderr, " rts");
	if (capconf.sdl.ifleads & SDL_LEAD_DCD)
		fprintf(stderr, " dcd");
	if (capconf.sdl.ifleads & SDL_LEAD_CTS)
		fprintf(stderr, " cts");
	if (capconf.sdl.ifleads & SDL_LEAD_DSR)
		fprintf(stderr, " dsr");
	fprintf(stderr, "\n\tifbpv: %u", capconf.sdl.ifbpv);
	fprintf(stderr, "\n\tifalarms:");
	if (capconf.sdl.ifalarms & SDL_ALARM_RED)
		fprintf(stderr, " red");
	if (capconf.sdl.ifalarms & SDL_ALARM_BLU)
		fprintf(stderr, " blue");
	if (capconf.sdl.ifalarms & SDL_ALARM_YEL)
		fprintf(stderr, " yellow");
	if (capconf.sdl.ifalarms & SDL_ALARM_REC)
		fprintf(stderr, " recovery");
	fprintf(stderr, "\n\tifrxlevel: %u", capconf.sdl.ifrxlevel);
	fprintf(stderr, "\n\tiftxlevel: %u", capconf.sdl.iftxlevel);
	fprintf(stderr, "\n\tifsync: %u", capconf.sdl.ifsync);
	fprintf(stderr, "\n\tifsyncsrc:");
	fprintf(stderr, " %u", capconf.sdl.ifsyncsrc[0]);
	fprintf(stderr, ",%u", capconf.sdl.ifsyncsrc[1]);
	fprintf(stderr, ",%u", capconf.sdl.ifsyncsrc[2]);
	fprintf(stderr, ",%u", capconf.sdl.ifsyncsrc[3]);
	fprintf(stderr, "\n");
}

void
show_sdt_config(void)
{
	fprintf(stderr, "\tt8: %u\n", capconf.sdt.t8);
	fprintf(stderr, "\tTin: %u\n", capconf.sdt.Tin);
	fprintf(stderr, "\tTie: %u\n", capconf.sdt.Tie);
	fprintf(stderr, "\tT: %u\n", capconf.sdt.T);
	fprintf(stderr, "\tD: %u\n", capconf.sdt.D);
	fprintf(stderr, "\tTe: %u\n", capconf.sdt.Te);
	fprintf(stderr, "\tDe: %u\n", capconf.sdt.De);
	fprintf(stderr, "\tUe: %u\n", capconf.sdt.Ue);
	fprintf(stderr, "\tN: %u\n", capconf.sdt.N);
	fprintf(stderr, "\tm: %u\n", capconf.sdt.m);
	fprintf(stderr, "\tb: %u\n", capconf.sdt.b);
	fprintf(stderr, "\tf:");
	switch (capconf.sdt.f) {
	case SDT_FLAGS_ONE:
		fprintf(stderr, " one\n");
		break;
	case SDT_FLAGS_SHARED:
		fprintf(stderr, " shared\n");
		break;
	default:
		fprintf(stderr, " %u\n", capconf.sdt.f);
		break;
	}
}

/* 
 *  The capture device is enabled in SDT mode.  We do no want to see so many
 *  FISUs and LSSUs, we are only really interested in MSUs.
 */
int
cap_config(void)
{
	capconf.opt.pvar = SS7_PVAR_ITUT_96;
	capconf.opt.popt = 0;
	if (output > 1)
		syslog(LOG_NOTICE, "Setting link configuration");
	ctl.ic_cmd = SDL_IOCSOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.opt);
	ctl.ic_dp = (char *) &capconf.opt;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCSOPTIONS: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reading link configuration");
	ctl.ic_cmd = SDL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.opt);
	ctl.ic_dp = (char *) &capconf.opt;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCSOPTIONS: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	/* defaults will do */
	if (output > 1)
		syslog(LOG_NOTICE, "Reading link configuration");
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.sdl);
	ctl.ic_dp = (char *) &capconf.sdl;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCGCONFIG: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	if (output > 2)
		show_sdl_config();
	/* these are the only two things we affect at this level */
	capconf.sdl.iftype = iftype;
	capconf.sdl.ifrate = ifrate;
	if (output > 1)
		syslog(LOG_NOTICE, "Setting link configuration");
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.sdl);
	ctl.ic_dp = (char *) &capconf.sdl;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCSCONFIG: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	capconf.sdt.N = 16;
	capconf.sdt.m = 272;
	capconf.sdt.t8 = 10;
	capconf.sdt.Tin = 4;
	capconf.sdt.Tie = 1;
	capconf.sdt.T = 64;
	capconf.sdt.D = 256;
	capconf.sdt.Te = 577169;
	capconf.sdt.De = 9308000;
	capconf.sdt.Ue = 144292000;
	capconf.sdt.b = 8;
	capconf.sdt.f = 0;
	if (output > 1)
		syslog(LOG_NOTICE, "Setting link configuration");
	ctl.ic_cmd = SDT_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.sdt);
	ctl.ic_dp = (char *) &capconf.sdt;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDT_IOCSCONFIG: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reading link configuration");
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(capconf.sdl);
	ctl.ic_dp = (char *) &capconf.sdl;
	if (ioctl(cap_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCGCONFIG: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	if (output > 2)
		show_sdt_config();
	/* defaults will do */
	return CAP_SUCCESS;
}

/* 
 *  We only start the DAEDR (receive) for monitoring.  We leave the
 *  transmitters disabled.
 */
int
cap_monitor(void)
{
	union SDT_primitives *d = (union SDT_primitives *) cbuf;
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(d->daedr_start_req);
	ctrl.buf = cbuf;
	d->daedr_start_req.sdt_primitive = SDT_DAEDR_START_REQ;
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
#if 0
	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(d->daedr_start_req);
	ctrl.buf = cbuf;
	d->daedr_start_req.sdt_primitive = SDT_DAEDT_START_REQ;
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
#endif
	/* start requests are not acknowledged */
	return CAP_SUCCESS;
}

int
cap_disable(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->disable_req);
	ctrl.buf = cbuf;
	p->disable_req.lmi_primitive = LMI_DISABLE_REQ;
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	/* start requests are not acknowledged */
	return CAP_SUCCESS;
}

int
cap_detach(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(cap_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		cap_exit(1);
		return CAP_FAILURE;
	}
	/* start requests are not acknowledged */
	return CAP_SUCCESS;
}

int
cap_close(void)
{
	close(cap_fd);
	return CAP_SUCCESS;
}

int
cap_start(void)
{
	switch (link_state) {
	case 0:
		if (output > 2)
			syslog(LOG_NOTICE, "opening link");
		if (cap_open() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 1;
	case 1:
		if (output > 2)
			syslog(LOG_NOTICE, "attaching link");
		if (cap_attach() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 2;
	case 2:
		if (output > 2)
			syslog(LOG_NOTICE, "configuring link");
		if (cap_config() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 3;
	case 3:
		if (output > 2)
			syslog(LOG_NOTICE, "enabling link");
		if (cap_enable() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 4;
	case 4:
		if (output > 2)
			syslog(LOG_NOTICE, "monitoring on link");
		if (cap_monitor() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 5;
	case 5:
		break;
	}
	return CAP_SUCCESS;
}

int
cap_stop(void)
{
	switch (link_state) {
	case 5:
	case 4:
		if (output > 2)
			syslog(LOG_NOTICE, "disabling link");
		if (cap_disable() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 3;
	case 3:
		link_state = 2;
	case 2:
		if (output > 2)
			syslog(LOG_NOTICE, "detaching link");
		if (cap_detach() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 1;
	case 1:
		if (output > 2)
			syslog(LOG_NOTICE, "closing link");
		if (cap_close() != CAP_SUCCESS)
			return CAP_FAILURE;
		link_state = 0;
	}
	return CAP_SUCCESS;
}

void
cap_enter(void)
{
	if (nomead) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			perror("ss7cap");
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		setsid();	/* become a session leader */
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror("ss7cap");
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		/* release current directory */
		if (chdir("/") == -1) {
			perror("ss7cap");
			exit(2);
		}
		umask(0);	/* clear file creation mask */
		/* rearrange file streams */
		fclose(stdin);
	}
	/* continue as foreground or background */
	openlog("ss7capd", LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON);
	if (lnkname[0] == '\0')
		snprintf(lnkname, sizeof(lnkname), "ss7cap%d", getpid());
	if (nomead || outfile[0] != '\0') {
		/* initialize default filename */
		if (outfile[0] == '\0')
			snprintf(outfile, sizeof(outfile), "%s.out", lnkname);
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stdout to file %s", outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stdout to %s", outpath);
			cap_exit(2);
		}
	}
	if (nomead || errfile[0] != '\0') {
		/* initialize default filename */
		if (errfile[0] == '\0')
			snprintf(errfile, sizeof(errfile), "%s.err", lnkname);
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stderr to file %s", errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stderr to %s", errpath);
			cap_exit(2);
		}
	}
	sig_catch();
	output_header();
	syslog(LOG_NOTICE, "Startup complete.");
}

void
ss7cap(void)
{
	cap_enter();
	if (cap_start() == CAP_SUCCESS)
		while (wait_event(10000) != CAP_FAILURE) ;
	cap_stop();
	cap_exit(1);
}

int
main(int argc, char **argv)
{
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"daemon",	no_argument,		NULL, 'd'},
			{"outpdir",	required_argument,	NULL, 'O'},
			{"lnkname",	required_argument,	NULL, 'n'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"logfile",	required_argument,	NULL, 'l'},
			{"cfgfile",	required_argument,	NULL, 'f'},
			{"card",	required_argument,	NULL, 'c'},
			{"span",	required_argument,	NULL, 's'},
			{"slot",	required_argument,	NULL, 't'},
			{"ds0a",	no_argument,		NULL, 'a'},
			{"ds0",		no_argument,		NULL, '0'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "dO:n:o:l:f:c:s:t:qD::v::hVC?W:", long_options,
				&option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "dO:n:o:l:f:c:s:t:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			break;
		}
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'd':	/* -d, --daemon */
			if (debug)
				fprintf(stderr, "%s: selecting daemon mode\n", argv[0]);
			nomead = 1;
			break;
		case 'O':	/* -O, --outpdir outpdir */
			if (debug)
				fprintf(stderr, "%s: setting outpdir to %s\n", argv[0], optarg);
			strncpy(outpdir, optarg, sizeof(outpdir) - 1);
			break;
		case 'n':	/* -n, --lnkname lnkname */
			if (debug)
				fprintf(stderr, "%s: setting lnkname to %s\n", argv[0], optarg);
			strncpy(lnkname, optarg, sizeof(lnkname) - 1);
			break;
		case 'o':	/* -o, --outfile outfile */
			if (debug)
				fprintf(stderr, "%s: setting outfile to %s\n", argv[0], optarg);
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'l':	/* -l, --logfile errfile */
			if (debug)
				fprintf(stderr, "%s: setting errfile to %s\n", argv[0], optarg);
			strncpy(errfile, optarg, sizeof(errfile) - 1);
			break;
		case 'f':	/* -f, --cfgfile cfgfile */
			if (debug)
				fprintf(stderr, "%s: setting cfgfile to %s\n", argv[0], optarg);
			strncpy(cfgfile, optarg, sizeof(cfgfile) - 1);
			break;
		case 'e':	/* -e, --device */
			if (debug)
				fprintf(stderr, "%s: setting devname to %s\n", argv[0], optarg);
			strncpy(devname, optarg, sizeof(devname) - 1);
			break;
		case 'c':	/* -c, --card */
			if (debug)
				fprintf(stderr, "%s: setting card to %d\n", argv[0], atoi(optarg));
			card = atoi(optarg);
			break;
		case 's':	/* -s, --span */
			if (debug)
				fprintf(stderr, "%s: setting span to %d\n", argv[0], atoi(optarg));
			span = atoi(optarg);
			break;
		case 't':	/* -t, --slot */
			if (debug)
				fprintf(stderr, "%s: setting slot to %d\n", argv[0], atoi(optarg));
			slot = atoi(optarg);
			break;
		case 'a':	/* -a, --ds0a */
			if (debug)
				fprintf(stderr, "%s: setting interface type to ds0a\n", argv[0]);
			iftype = SDL_TYPE_DS0A;
			ifrate = 56000;
			break;
		case '0':	/* -0, --ds0 */
			if (debug)
				fprintf(stderr, "%s: setting interface type to ds0\n", argv[0]);
			iftype = SDL_TYPE_DS0;
			ifrate = 64000;
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'D':	/* -D, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
				goto bad_usage;
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	ss7cap();
	exit(4);
}
