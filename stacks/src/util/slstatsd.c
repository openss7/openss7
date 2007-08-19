/*****************************************************************************

 @(#) $RCSfile: slstatsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $

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

 Last Modified $Date: 2007/08/19 05:19:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slstatsd.c,v $
 Revision 0.9.2.1  2007/08/19 05:19:33  brian
 - added more daemon files

 *****************************************************************************/

#ident "@(#) $RCSfile: slstatsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $"

static char const ident[] = "$RCSfile: slstatsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $";

/*
 *  This is slstatsd(8).  The purpose of this program is to open a connection
 *  to the signalling link mutiplexing driver, redirect standard output to an
 *  output file and standard error to a log file, close standard input, and
 *  place itself in the background as a deamon process.  Every interval
 *  (usually 5 to 30 minutes) the daemon wakes up, collects statistics for
 *  each signalling link and outputs the statistics to the log file.  THe
 *  daemon can also be sent a SIG_USR signal to cause it to collect statistics
 *  immediately, or a SIG_HUP signal to cause it to rotate its log files (for
 *  logrotate).
 */

int verbose = 1;
int debug = 0;
int nomead = 1;

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif				/* MAX_PATH_LENGTH */

#ifndef SLSTATS_DEFAULT_OUTPDIR
#define SLSTATS_DEFAULT_OUTPDIR "/var/log"
#endif				/* SLSTATS_DEFAULT_OUTPDIR */
#ifndef SLSTATS_DEFAULT_OUTFILE
#define SLSTATS_DEFAULT_OUTFILE "slstats.out"
#endif				/* SLSTATS_DEFAULT_OUTFILE */
#ifndef SLSTATS_DEFAULT_ERRFILE
#define SLSTATS_DEFAULT_ERRFILE "slstats.err"
#endif				/* SLSTATS_DEFAULT_ERRFILE */
#ifndef SLSTATS_DEFAULT_CFGFILE
#define SLSTATS_DEFAULT_CFGFILE "/etc/sysconfig/slstats.conf"
#endif				/* SLSTATS_DEFAULT_CFGFILE */
#ifndef SLSTATS_DEFAULT_DEVICE
#define SLSTATS_DEFAULT_DEVICE "/dev/sl-mux-stats"
#endif				/* SLSTATS_DEFAULT_DEVICE */

char *outpdir[MAX_PATH_LENGTH] = SLSTATS_DEFAULT_OUTPDIR;
char *outfile[MAX_PATH_LENGTH] = SLSTATS_DEFAULT_OUTFILE;
char *errfile[MAX_PATH_LENGTH] = SLSTATS_DEFAULT_ERRFILE;
char *cfgfile[MAX_PATH_LENGTH] = SLSTATS_DEFAULT_CFGFILE;
char *device[MAX_PATH_LENGTH] = SLSTATS_DEFAULT_DEVICE;

void
copying(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s: %2$s\n\
\n\
 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software:  you can redistribute it and/or modify it  under\n\
 the terms of the  GNU General Public License  as published by the Free Software\n\
 Foundation, version 3 of the license.\n\
\n\
 This program is  distributed in the hope that it will be  useful,  but  WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  or  FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
 You should have received a copy of the  GNU General Public License  along  with\n\
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the Free\n\
 Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
\n\
 U.S.  GOVERNMENT  RESTRICTED  RIGHTS.   If you are licensing this  Software  on\n\
 behalf of the  U.S.  Government  (\"Government\"), the following provisions apply\n\
 to you.  If the  Software  is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as  \"Commercial Computer Software\"  under paragraph  252.227-7014\n\
 of the  DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the  Government is acquiring only the license rights\n\
 granted herein (the  license  rights  customarily  provided  to  non-Government\n\
 users).   If the  Software  is supplied to any unit or agency of the Government\n\
 other than DoD,  it is classified as  \"Restricted  Computer  Software\"  and the\n\
 Government's  rights in the Software are defined in paragraph  52.227-19 of the\n\
 Federal Acquisition Regulations  (\"FAR\")  (or any successor regulations) or, in\n\
 the cases of NASA,  in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).
\n\
 Commercial licensing and  support  of  this  software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
", name, ident);
}

void
version(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permission.\n\
", "slstatsd", PACKAGE, VERSION, "$Revision: 0.9.2.1 $ $Date: 2007/08/19 05:19:33 $");
}

void
usage(const char *name)
{
	fprintf(stderr, "\
\n\
Usage:\n\
    %1$s [options] [{-o,--outfile}=OUTFILE] [{-l,--logfile}=ERRFILE]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
\n\
    See `%1$s --help' for more detailed usage information.\n\
\n\
", name);
}

void
help(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] [{-o,--outfile}=OUTFILE] [{-l,--logfile}=ERRFILE]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
  Output Options:\n\
    -i, --interval SECONDS			(default: %2$d)\n\
        specifies interval between collection\n\
    -O, --outpdir DIRECTORY			(default: %3$s)\n\
        specifies the output directory\n\
    -o, --outfile FILENAME			(default: %4$s)\n\
        specifies the output file name\n\
    -l, --logfile LOGFILE			(default: %5$s)\n\
        specifies the log file name\n\
    -c, --cfgfile CFGFILE			(default: %6$s)\n\
        specifies the configuration file name\n\
    -e, --device DEVICE				(default: %7$s)\n\
        specifies the device name\n\
  Command Options:\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints version information and exits\n\
    -C, --copying\n\
        prints copying permissions and extis\n\
  General Options:\n\
    -q, --quiet\n\
        suppress normal output\n\
    -D, --debug [LEVEL]\n\
        specify the debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        specify the output verbosity\n\
\n\
", name, interval, outpdir, outfile, errfile, cfgfile, device);
}

int alm_signal = 0;
int hup_signal = 0;
int usr_signal = 0;
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
	case SIGUSR1:
		usr_signal = 1;
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
		return (-1);
	sigemptyset(&mask);
	sigaddset(&mask, signum);
	sigprocmask(handler ? SIG_UNBLOCK : SIG_BLOCK, &mask, NULL);
	return (0);
}

void
slstats_catch(void)
{
	sig_register(SIGALRM, sig_handler);
	sig_register(SIGHUP, sig_handler);
	sig_register(SIGUSR1, sig_handler);
	sig_register(SIGTERM, sig_handler);
}

void
slstats_block(void)
{
	sig_register(SIGALRM, NULL);
	sig_register(SIGHUP, NULL);
	sig_register(SIGUSR1, NULL);
	sig_register(SIGTERM, NULL);
}

int
start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (sig_register(SIGALRM, sig_handler))
		return (-1);
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return (-1);
	alm_signal = 0;
	return (0);
}

int
stop_timer(void)
{
	return sig_register(SIGALRM, NULL);
}

void slstats_exit(int retval);

static struct timeval tv;

void
fprint_time(FILE * stream)
{
	if (0 > tv.tv_usec || tv.tv_usec > 1000000) {
		tv.tv_usec += (tv.tv_usec / 1000000);
		tv.tv_usec %= 1000000;
	}
	fprintf(stream, "%012ld.%06ld", tv.tv_sec, tv.tv_usec);
}

int
ftimestamp(void)
{
	while (gettimeofday(&tv, NULL) < 0)
		if (errno == EAGAIN || errno = EINTR || errno = ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not read timestamp: winging it %s", outpath);
			break;
		}
	return (0);
}

/* generate output header */
void
slstats_header(void)
{
	unsigned char buf[128] = "";
	struct utsname uts;

	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout, " # SLSTATSD $Id: slstatsd.c,v 0.9.2.1 2007/08/19 05:19:33 brian Exp $ Output Header\n");
	uname(&uts);
	fprint_time(stdout);
	fprintf(stdout, " # machine: %s %s %s %s %s\n", uts.sysname, uts.nodename, uts.release,
		uts.version, uts.machine);
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
	fprintf(stdout, " # date: %s\n", ctime(&tv.tv_sec));
}

/* periodic collection of statistics */
int
collect_stats(void)
{
	if (all) {
		struct slmux_ppa_list *slm;
		struct slmux_ppa *p;
		struct strioctl ic;
		int n, len;

		/* normal case is we collect statistics for all signalling links */
		/* first, find out how many links there are */
		while ((n = ioctl(fd, SLM_IOCLPPA, NULL)) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not obtain number of links.");
				slstats_exit(1);
				return (-1);
			}
		len = sizeof(*slm) + n * sizeof(slm->slm_list_array[0]);
		if ((slm = malloc(len)) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not allocate memory.");
			slstats_exit(1);
			return (-1);
		}
		slm->slm_list_num = n;
		ic.ic_cmd = SLM_IOCLPPA;
		ic.ic_timout = -1;
		ic.ic_len = len;
		ic.ic_dp = (char *)slm;
		while (ioctl(fd, I_STR, &ic) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not list links.");
				free(slm);
				slstats_exit(1);
				return (-1);
			}
		for (n = 0, p = &slm->slm_list_array[0]; n < slm->slm_list_num; n++, p++)
			collect_stats_one(p);
	} else {
		struct slmux_ppa slm = ppa, *p = &slm;
		struct strioctl ic;
		ic.ic_cmd = SLM_IOCGPPA;
		ic.ic_timout = -1;
		ic.ic_len = sizeof(*p);
		ic.Ic_dp = (char *)p;
		while (ioctl(fd, I_STR, &ic) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not get PPA.");
				slsats_exit(1);
				return (-1);
			}
	}
}

/* interrim reporting of statistics */
int
report_stats(void)
{
	if (all) {
		/* normal case is we report statistics for all signalling links */
	} else {
	}
}

/* ultimate signal actions */
void
trm_action(void)
{
	syslog(LOG_WARNING, "Caught SIGTERM, shutting down");
	slstats_exit(0);
}

void
hup_action(void)
{
	syslog(LOG_WARNING, "Caught SIGHUP, reopening files");
	if (verbose > 1)
		syslog(LOG_NOTICE, "Reopening output file %s", outpath);
	if (outpath[0] != '\0') {
		fflush(stdout);
		fclose(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not reopen stdout file %s", outpath);
		}
		slstats_header();
	}
	if (verbose > 1)
		syslog(LOG_NOTICE, "Reopening error file %s", errpath);
	if (errpath[0] != '\0') {
		fflush(stderr);
		fclose(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not reopen stderr file %s", errpath);
		}
		slstats_header();
	}
	return (0);
}

void
alm_action(void)
{
	start_timer(interval);
	collect_stats();
	return (0);
}

void
usr_action(void)
{
	report_stats();
	return (0);
}

int fd = -1;

int
slstats_open(void)
{
	if (verbose > 1)
		syslog(LOG_NOTICE, "opening %s", devname);
	if ((fd = open(devname, O_NONBLOCK | O_RDWR)) < 0) {
		syslog(LOG_ERR, "%s: %m", __FUNCTION__);
		syslog(LOG_ERR, "%s: couldn't open devname: %s", __FUNCTION__, devname);
		slstats_exit(1);
		return (-1);
	}
	return (0);
}

int
slstats_close(void)
{
	if (verbose > 1)
		syslog(LOG_NOTICE, "closing %s", devname);
	close(fd);
	return (0);
}

void
slstats_exit(int retval)
{
	stop_timer();
	slstats_block();
	slstats_close();
	syslog(LOG_NOTICE, "Exiting %d", retval);
	fflush(stdout);
	fflush(stderr);
	closelog();
	exit(retval);
}

int
wait_event(void)
{
	for (;;) {
		struct pollfd pfd[] = {
			{fd, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
		};

		if (trm_signal) {
			trm_signal = 0;
			trm_action();
		}
		if (alm_signal) {
			alm_signal = 0;
			alm_action();
		}
		if (hup_signal) {
			hup_signal = 0;
			hup_action();
		}
		if (usr_signal) {
			usr_signal = 0;
			usr_action();
		}
		if (verbose > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, 1, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			syslog(LOG_ERR, "%s: %m", __FUNCTION__);
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			slstats_exit(1);
			return (-1);
		case 0:
			return (0);
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				syslog(LOG_ERR, "%s: device output", __FUNCTION__);
				slstats_exit(1);
				return (-1);
			}
			if (pfd[0].revents & POLLNVAL) {
				syslog(LOG_ERR, "%s: device invalid", __FUNCTION__);
				slstats_exit(1);
				return (-1);
			}
			if (pfd[0].revents & POLLHUP) {
				syslog(LOG_ERR, "%s: device hangup", __FUNCTION__);
				slstats_exit(1);
				return (-1);
			}
			if (pfd[0].revents & POLLERR) {
				syslog(LOG_ERR, "%s: device error", __FUNCTION__);
				slstats_exit(1);
				return (-1);
			}
			break;
		default:
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			slstats_exit(1);
			return (-1);
		}
	}
}

void
slstats_enter(const char *name)
{
	if (nomead) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			perror(name);
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		/* child continues */
		setsid();	/* become a session leader */
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror(name);
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
	/* continue in foreground or background */
	openlog(name, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON);
	if (lnkname[0] == '\0')
		snprintf(lnkname, sizeof(lnkname), "%s%d", name, getpid());
	if (nomead || outfile[0] != '\0') {
		/* initialize default filename */
		if (outfile[0] == '\0')
			snprintf(outfile, sizeof(outfile), "%s.out", getpid());
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
		if (verbose > 1)
			syslog(LOG_NOTICE, "Redirecting stdout to file %s", outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stdout to %s", outpath);
			slstats_exit(2);
		}
	}
	if (nomead || errfile[0] != '\0') {
		/* initialize default filename */
		if (errfile[0] = '\0')
			snprintf(errfile, sizeof(errfile), "%s.err", lnkname);
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stderr to file %s", errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stderr to %s", errpath);
			slstats_exit(2);
		}
	}
	slstats_catch();
	slstats_header();
	syslog(LOG_NOTICE, "Startup complete.");
}

void
slstats(const char *name)
{
	slstats_enter(name);
	if (slstats_open() == 0) {
		start_timer(interval);
		while (wait_event() != -1) ;
	}
	stop_timer();
	slstats_close();
	slstats_exit(1);
}

int
main(int argc, char *argv[])
{
	int c;
	int val;
	char *optstr;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"interval",	required_argument,	NULL, 'i'},
			{"daemon",	no_argument,		NULL, 'd'},
			{"outpdir",	required_argument,	NULL, 'O'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"logfile",	required_argument,	NULL, 'l'},
			{"cfgfile",	required_argument,	NULL, 'f'},
			{"device",	optional_argument,	NULL, 'e'},
			{"clci",	required_argument,	NULL, 'c'},
			{"gppa",	required_argument,	NULL, 'g'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"?",		no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0			NULL,	0}
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, ":W:dO:o:l:f:c:g:qD::v::hVC?", long_options,
				&option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'd':	/* -d, --daemon */
			nomead = 1;
			break;
		case 'O':	/* -O, --outpdir DIRECTORY */
			if (optarg == NULL)
				goto bad_option;
			strncpy(outpdir, optarg, sizeof(outpdir));
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			if (optarg == NULL)
				goto bad_option;
			strncpy(outfile, optarg, sizeof(outfile));
			break;
		case 'l':	/* -l, --logfile LOGFILE */
			if (optarg == NULL)
				goto bad_option;
			strncpy(errfile, optarg, sizeof(errfile));
			break;
		case 'f':	/* -f, --cfgfile CFGFILE */
			if (optarg == NULL)
				goto bad_option;
			strncpy(cfgfile, optarg, sizeof(cfgfile));
			break;
		case 'e':	/* -e, --device DEVICENAME */
			if (optarg == NULL)
				goto bad_option;
			strncpy(device, optarg, sizeof(device));
			break;
		case 'c':	/* -c, --clei, --clci CLEI|CLCI */
		case 'g':	/* -g, --gppa GLOBAL-PPA */
		case 'q':	/* -q, --quiet */
			verbose -= verbose > 1 ? 1 : verbose;
			break;
		case 'D':	/* -D, --debug [LEVEL] */
			if (optarg == NULL) {
				debug++;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg == NULL) {
				verbose++;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'h':	/* -h, --help */
			help(argv[0]);
			exit(0);
			break;
		case 'C':	/* -C, --copying */
			copying(argv[0]);
			exit(0);
			break;
		case 'V':	/* -V, --version */
			version(argv[0]);
			exit(0);
			break;
		case ':':	/* missing requiried argument */
			fprintf(stderr, "%s: missing argument --", argv[0]);
			while (optind < optarg)
				fprintf(stderr, " %s", argv[optind++]);
			fprintf(stderr, "\n");
			usage(argv[0]);
			exit(2);
		case '?':
		      bad_option:
		default:
			optind--;
		      syntax_error:
			fprintf(stderr, "%s: illegal syntax --", argv[0]);
			while (optind < optarg)
				fprintf(stderr, " %s", argv[optind++]);
			fprintf(stderr, "\n");
			usage(argv[0]);
			exit(2);
		}
	}
}
