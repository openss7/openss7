/*****************************************************************************

 @(#) $RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 01:30:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/28 01:30:35 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 01:30:35 $"

static char const ident[] =
    "$RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 01:30:35 $";

/* 
 *  AIX Daemon: strerr - (Daemon) Receives error log messages from the STREAMS
 *  log driver.
 */

#define _XOPEN_SOURCE 600

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>
#include <strlog.h>

static int nomead = 1;			/* default daemon mode */
static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

int strlog_fd = -1;

char outfile[256] = "";
char errfile[256] = "";
char outpath[256] = "";
char errpath[256] = "";
char basname[256] = "";
char cfgfile[256] = "";
char outpdir[256] = "/var/log/streams";
char devname[256] = "";
char mailuid[256] = "";

static void version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Distributed under GPL Version 2, included here by reference.\n\
", argv[0], ident);
}

static void usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s { -h |--help }\n\
    %1$s { -V |--version }\n\
    %1$s { -C |--copying }\n\
", argv[0]);
}

static void help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s { -h |--help }\n\
    %1$s { -V |--version }\n\
    %1$s { -C |--copying }\n\
Options:\n\
    -a, --admin MAILID\n\
        specify a mail address for notifications, default: 'root'\n\
    -d, --directory DIRECTORY\n\
        specify a directory for log files, default: '/var/log/streams'\n\
    -n, --nodaemon\n\
        do not daemonize, run in the foreground, default: off\n\
    -b, --basename BASENAME\n\
        file basename, default: 'error'\n\
    -o, --outfile OUTFILE\n\
        redirect output to OUTFILE, default: ${BASENAME}.mm-dd\n\
    -e, --errfile ERRFILE\n\
        redirect errors to ERRFILE, default: ${BASENAME}.errors\n\
    -l, --logdev LOGDEVICE\n\
        log device to open, default: '/dev/strlog'\n\
    -q, --quiet\n\
        suppress output\n\
    -D, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
    -C, --copying\n\
        prints copying permissions and exits\n\
", argv[0]);
}

static void copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; either  version  2  of  the  License, or (at  your option) any later\n\
version.\n\
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
	STRERR_NONE = 0,
	STRERR_SUCCESS = 0,
	STRERR_TIMEOUT,
	STRERR_PCPROTO,
	STRERR_PROTO,
	STRERR_DATA,
	STRERR_FAILURE = -1,
};

int sig_register(int signum, RETSIGTYPE(*handler) (int))
{
	sigset_t mask;
	struct sigaction act;
	act.sa_handler = handler ? handler : SIG_DFL;
	act.sa_flags = handler ? SA_RESTART : 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(signum, &act, NULL))
		return STRERR_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, signum);
	sigprocmask(handler ? SIG_UNBLOCK : SIG_BLOCK, &mask, NULL);
	return STRERR_SUCCESS;
}

int alm_signal = 0;
int hup_signal = 0;
int trm_signal = 0;

RETSIGTYPE alm_handler(int signum)
{
	alm_signal = 1;
	return (RETSIGTYPE) (0);
}

int alm_catch(void)
{
	sig_register(SIGALRM, &alm_handler);
}

int alm_block(void)
{
	sig_register(SIGALRM, NULL);
}

int alm_action(void)
{
	alm_signal = 0;
	return (0);
}

RETSIGTYPE hup_handler(int signum)
{
	hup_signal = 1;
	return (RETSIGTYPE) (0);
}

int hup_catch(void)
{
	sig_register(SIGALRM, &hup_handler);
}

int hup_block(void)
{
	sig_register(SIGALRM, NULL);
}

int hup_action(void)
{
	hup_signal = 0;
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
		// output_header(void);
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

RETSIGTYPE trm_handler(int signum)
{
	trm_signal = 1;
	return (RETSIGTYPE) (0);
}

int trm_catch(void)
{
	sig_register(SIGALRM, &trm_handler);
}

int trm_block(void)
{
	sig_register(SIGALRM, NULL);
}

void strerr_exit(int retval);

int trm_action(void)
{
	trm_signal = 0;
	syslog(LOG_WARNING, "Caught SIGTERM, shutting down");
	strerr_exit(0);
}

void sig_catch(void)
{
	alm_catch();
	hup_catch();
	trm_catch();
}

void sig_block(void)
{
	alm_block();
	hup_block();
	trm_block();
}

int start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};
	if (alm_catch())
		return STRERR_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return STRERR_FAILURE;
	alm_signal = 0;
	return STRERR_SUCCESS;
}

int sterr_stop(void);

void strerr_exit(int retval)
{
	syslog(LOG_NOTICE, "Exiting %d", retval);
	fflush(stdout);
	fflush(stderr);
	// strerr_stop();
	sig_block();
	closelog();
	exit(retval);
}

void strerr_enter(int argc, char *argv[])
{
	if (nomead) {
		pid_t pid;
		if ((pid = fork()) < 0) {
			perror(__FUNCTION__);
			exit(2);
		} else if (pid != 0) {
			/* parent exists */
			exit(0);
		}
		setsid();	/* become a session leader */
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror(__FUNCTION__);
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		chdir("/");	/* release current directory */
		umask(0);	/* clear file creation mask */
		/* rearrange file streams */
		fclose(stdin);
	}
	/* continue as foreground or background */
	openlog("strerr", LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON);
	if (basname[0] == '\0')
		snprintf(basname, sizeof(basname), "error");
	if (nomead || outfile[0] != '\0') {
		struct tm tm;
		time_t curtime;
		time(&curtime);
		localtime_r(&curtime, &tm);
		/* initialize default filename */
		if (outfile[0] == '\0')
			snprintf(outfile, sizeof(outfile), "%s.%02d-%02d", basname, tm.tm_mon,
				 tm.tm_mday);
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stdout to file %s", outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stdout to %s", outpath);
			strerr_exit(2);
		}
	}
	if (nomead || errfile[0] != '\0') {
		/* initialize default filename */
		if (errfile[0] == '\0')
			snprintf(errfile, sizeof(errfile), "error");
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stderr to file %s", errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stderr to %s", errpath);
			strerr_exit(2);
		}
	}
	sig_catch();
	// output_header();
	syslog(LOG_NOTICE, "Startup complete.");
}

void strerr_open(void)
{
	struct strioctl ioc;
	/* open log device */
	if (devname[0] == '\0') {
		/* search if not specified */
		if ((strlog_fd = open("/dev/strlog", O_RDWR)) < 0) {
			if ((strlog_fd = open("/dev/streams/log", O_RDWR)) < 0) {
				if ((strlog_fd = open("/dev/log", O_RDWR)) < 0) {
					perror(__FUNCTION__);
					strerr_exit(1);
				}
			}
		}
	} else {
		if ((strlog_fd = open(devname, O_RDWR)) < 0) {
			perror(__FUNCTION__);
			strerr_exit(1);
		}
	}
	/* setup log device for logging */
	ioc.ic_cmd = I_ERRLOG;
	ioc.ic_timout = 0;
	ioc.ic_len = 0;
	ioc.ic_dp = NULL;
	if (ioctl(strlog_fd, I_STR, &ioc) < 0) {
		perror(__FUNCTION__);
		strerr_exit(1);
	}
}
void strerr_close(void)
{
	if (close(strlog_fd) < 0)
		perror(__FUNCTION__);
}

int main(int argc, char *argv[])
{
	while (1) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"admin",	required_argument,	NULL, 'a'},
			{"directory",	required_argument,	NULL, 'd'},
			{"nodaemon",	no_argument,		NULL, 'n'},
			{"basename",	required_argument,	NULL, 'b'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"errfile",	required_argument,	NULL, 'e'},
			{"logdev",	required_argument,	NULL, 'l'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
		};
		/* *INDENT-ON* */
		c = getopt_long_only(argc, argv, "a:d:nb:o:e:l:qDvhVC?", long_options,
				     &option_index);
#else
		c = getopt(argc, argv, "a:d:nb:o:e:l:qDvhVC?");
#endif
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 'n':	/* -n, --nodaemon */
			nomead = 0;
			break;
		case 'a':	/* -a, --admin MAILID */
			strncpy(mailuid, optarg, 256);
			break;
		case 'd':	/* -d, --directory DIRECTORY */
			strncpy(outpdir, optarg, 256);
			break;
		case 'b':	/* -b, --basename BASNAME */
			strncpy(basname, optarg, 256);
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			strncpy(outfile, optarg, 256);
			break;
		case 'e':	/* -e, --errfile ERRFILE */
			strncpy(errfile, optarg, 256);
			break;
		case 'l':	/* -l, --logdev DEVNAME */
			strncpy(devname, optarg, 256);
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
				debug++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output++;
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
		      bad_nonopt:
			if (output > 0 || debug > 0) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (optind < argc)
		goto bad_nonopt;
	strerr_open();
	for (;;) {
		struct pollfd pfd[] = {
			{strlog_fd, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
		};
		if (trm_signal)
			trm_action();
		if (hup_signal)
			hup_action();
		if (alm_signal)
			alm_action();
		if (output > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, 1, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			syslog(LOG_ERR, "%s: %m", __FUNCTION__);
			strerr_exit(1);
			return STRERR_FAILURE;
		case 0:
			return STRERR_NONE;
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int ret, flags;
				char cbuf[1024];
				char dbuf[1024];
				struct strbuf ctl = { 1024, 1024, cbuf };
				struct strbuf dat = { 1024, 1024, dbuf };
				struct log_ctl *lc;
				if ((ret = getmsg(strlog_fd, &ctl, &dat, &flags)) < 0) {
					perror(__FUNCTION__);
					exit(1);
				}
				if (ret != 0) {
					errno = ERANGE;
					perror(__FUNCTION__);
					exit(1);
				}
				lc = (struct log_ctl *) cbuf;
				if (ctl.len < sizeof(*lc))
					continue;
				if (dat.len <= 0)
					continue;
				fprintf(stdout, "%lu", lc->seq_no);
				fprintf(stdout, " %s", ctime(&lc->ltime));
				fprintf(stdout, "%lu", (unsigned long) lc->ttime);
				fprintf(stdout, "%3d", lc->level);
				switch (lc->flags & (SL_TRACE | SL_FATAL | SL_NOTIFY)) {
				case 0:
					fprintf(stdout, "   ");
					break;
				case SL_TRACE:
					fprintf(stdout, "T  ");
					break;
				case SL_FATAL:
					fprintf(stdout, "F  ");
					break;
				case SL_NOTIFY:
					fprintf(stdout, "N  ");
					break;
				case SL_TRACE | SL_FATAL:
					fprintf(stdout, "TF ");
					break;
				case SL_TRACE | SL_NOTIFY:
					fprintf(stdout, "TN ");
					break;
				case SL_FATAL | SL_NOTIFY:
					fprintf(stdout, "FN ");
					break;
				case SL_TRACE | SL_FATAL | SL_NOTIFY:
					fprintf(stdout, "TFN");
					break;
				}
				fprintf(stdout, "%d", lc->mid);
				fprintf(stdout, "%d", lc->sid);
				fprintf(stdout, "\n");
				fflush(stdout);
			}
			if (pfd[0].revents & POLLNVAL) {
				syslog(LOG_ERR, "%s: device invalid", __FUNCTION__);
				strerr_exit(1);
				return STRERR_FAILURE;
			}
			if (pfd[0].revents & POLLHUP) {
				syslog(LOG_ERR, "%s: device hangup", __FUNCTION__);
				strerr_exit(1);
				return STRERR_FAILURE;
			}
			if (pfd[0].revents & POLLERR) {
				syslog(LOG_ERR, "%s: device error", __FUNCTION__);
				strerr_exit(1);
				return STRERR_FAILURE;
			}
			break;
		default:
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			strerr_exit(1);
			return STRERR_FAILURE;
		}
	}
	strerr_close();
	exit(0);
}
