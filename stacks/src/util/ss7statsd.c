/*****************************************************************************

 @(#) $RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:11:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ss7statsd.c,v $
 Revision 0.9.2.4  2008-04-29 07:11:33  brian
 - updating headers for release

 Revision 0.9.2.3  2007/09/06 11:16:27  brian
 - testing updates

 Revision 0.9.2.2  2007/08/19 11:57:41  brian
 - move stdbool.h, obviate need for YFLAGS, general workup

 Revision 0.9.2.1  2007/08/19 05:19:33  brian
 - added more daemon files

 *****************************************************************************/

#ident "@(#) $RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:33 $"

static char const ident[] =
    "$RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:33 $";

/* This file can be processed with doxygen(1). */

/** @addtogroup ss7statsd SS7 Statistics Daemon
  * @{ */

/*
 * This is ss7statsd(8).  The purpose of the daemon is to spawn a daemon process to collect
 * statistics and log them to a statistics collection log.
 */

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
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
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include <ss7/sl_mux.h>

#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif				/* MAX_PATH_LENGTH */

int verbose = 1;
int debug = 0;
int nomead = 1;
int useconfig = 0;

long interval = 300000;			/* 5 minutes in milliseconds. */

#ifndef SS7STATSD_DEFAULT_OUTPDIR
#define SS7STATSD_DEFAULT_OUTPDIR "/var/log" "/" PACKAGE
#endif				/* SS7STATSD_DEFAULT_OUTPDIR */
#ifndef SS7STATSD_DEFAULT_OUTFILE
#define SS7STATSD_DEFAULT_OUTFILE NAME ".out"
#endif				/* SS7STATSD_DEFAULT_OUTFILE */
#ifndef SS7STATSD_DEFAULT_ERRFILE
#define SS7STATSD_DEFAULT_ERRFILE NAME ".err"
#endif				/* SS7STATSD_DEFAULT_ERRFILE */
#ifndef SS7STATSD_DEFAULT_CFGFILE
#define SS7STATSD_DEFAULT_CFGFILE PACKAGE_CONFIGDIR "/" NAME ".conf"
#endif				/* SS7STATSD_DEFAULT_CFGFILE */

#ifndef SS7STATSD_DEFAULT_DEVSLMX
#define SS7STATSD_DEFAULT_DEVSLMX "/dev/sl-stats"
#endif				/* SS7STATSD_DEFAULT_DEVSLMX */
#ifndef SS7STATSD_DEFAULT_EVTSLMX
#define SS7STATSD_DEFAULT_EVTSLMX "/dev/sl-events"
#endif				/* SS7STATSD_DEFAULT_EVTSLMX */
#ifndef SS7STATSD_DEFAULT_DEVMTPX
#define SS7STATSD_DEFAULT_DEVMTPX "/dev/mtp-stats"
#endif				/* SS7STATSD_DEFAULT_DEVMTPX */
#ifndef SS7STATSD_DEFAULT_EVTMTPX
#define SS7STATSD_DEFAULT_EVTMTPX "/dev/mtp-events"
#endif				/* SS7STATSD_DEFAULT_EVTMTPX */
#ifndef SS7STATSD_DEFAULT_DEVSCCP
#define SS7STATSD_DEFAULT_DEVSCCP "/dev/sccp-stats"
#endif				/* SS7STATSD_DEFAULT_DEVSCCP */
#ifndef SS7STATSD_DEFAULT_EVTSCCP
#define SS7STATSD_DEFAULT_EVTSCCP "/dev/sccp-events"
#endif				/* SS7STATSD_DEFAULT_EVTSCCP */
#ifndef SS7STATSD_DEFAULT_DEVTCAP
#define SS7STATSD_DEFAULT_DEVTCAP "/dev/tcap-stats"
#endif				/* SS7STATSD_DEFAULT_DEVTCAP */
#ifndef SS7STATSD_DEFAULT_EVTTCAP
#define SS7STATSD_DEFAULT_EVTTCAP "/dev/tcap-events"
#endif				/* SS7STATSD_DEFAULT_EVTTCAP */
#ifndef SS7STATSD_DEFAULT_DEVISUP
#define SS7STATSD_DEFAULT_DEVISUP "/dev/isup-stats"
#endif				/* SS7STATSD_DEFAULT_DEVISUP */
#ifndef SS7STATSD_DEFAULT_EVTISUP
#define SS7STATSD_DEFAULT_EVTISUP "/dev/isup-events"
#endif				/* SS7STATSD_DEFAULT_EVTISUP */

char outpdir[MAX_PATH_LENGTH] = "";
char outfile[MAX_PATH_LENGTH] = "";
char errfile[MAX_PATH_LENGTH] = "";
char cfgfile[MAX_PATH_LENGTH] = "";
char outpath[MAX_PATH_LENGTH] = "";
char errpath[MAX_PATH_LENGTH] = "";

char devslmx[MAX_PATH_LENGTH] = "";
char evtslmx[MAX_PATH_LENGTH] = "";
char devmtpx[MAX_PATH_LENGTH] = "";
char evtmtpx[MAX_PATH_LENGTH] = "";
char devsccp[MAX_PATH_LENGTH] = "";
char evtsccp[MAX_PATH_LENGTH] = "";
char devtcap[MAX_PATH_LENGTH] = "";
char evttcap[MAX_PATH_LENGTH] = "";
char devisup[MAX_PATH_LENGTH] = "";
char evtisup[MAX_PATH_LENGTH] = "";

int slmx_fd = -1;			/* signalling link multiplexer */
int mtpx_fd = -1;			/* message transfer part */
int sccp_fd = -1;			/* signalling connection control part */
int tcap_fd = -1;			/* trasnaction capabilities application part */
int isup_fd = -1;			/* integrated services digital network user part */

struct pollfd pfd[] = {
	{-1, POLLIN | POLLPRI | POLLERR | POLLHUP, 0},
	{-1, POLLIN | POLLPRI | POLLERR | POLLHUP, 0},
	{-1, POLLIN | POLLPRI | POLLERR | POLLHUP, 0},
	{-1, POLLIN | POLLPRI | POLLERR | POLLHUP, 0},
	{-1, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
};

int pfd_num = 0;

/** @brief Print copying permissions.
  * @param name program name.
  * @internal
  *
  * This function prints the copying information as a result of the -C,
  * --copying options.  This is a standard OpenSS7 coding practice.
  */
void
copying(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
This program is  free  software:  you can redistribute it and/or modify it under\n\
the terms of the GNU Affero General Public License Version 3 as published by the\n\
Free Software Foundation,  found in the distributed information file  \"COPYING\",\n\
with the Section 7 conditions found in the file \"CONDITIONS\".\n\
\n\
This program is distributed in the hope that it will be useful, but  WITHOUT ANY\n\
WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.  If not, see <http://www.gnu.org/licenses/>,  or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
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
Commercial licensing and  support  of  this  software is available from  OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.4 $ $Date: 2008-04-29 07:11:33 $");
}

/** @brief Print version.
  * @param name program name.
  * @internal
  *
  * This function prints the version information as a result of the -V,
  * --version options.  This is stanard OpenSS7 coding practice.  This function
  * also prints to the second blank line (first to paragraphs) in the same
  * format as GNU projects.  The last two paragraphs are OpenSS7 practice.
  */
void
version(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permission.\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.4 $ $Date: 2008-04-29 07:11:33 $");
}

void
usage(const char *name)
{
	fprintf(stderr, "\
\n\
Usage:\n\
    %1$s [{-i,--interval=}INTERVAL] [{-n,--nodaemon}] [{-O,--outpdir=}DIRECTORY]\n\
	 [{-o,--outfile=}OUTFILE] [{-l,--logfile=}LOGFILE]\n\
	 [{-f,--cfgfile[=]}[CFGFILE]] [{-L,--slmx[=]}[DEVNAME[,DEVNAME]]]\n\
	 [{-M,--mtpx[=]}[DEVNAME[,DEVNAME]]] [{-S,--sccp[=]}[DEVNAME[,DEVNAME]]]\n\
	 [{-T,--tcap[=]}[DEVNAME[,DEVNAME]]] [{-I,--isup[=]}[DEVNAME[,DEVNAME]]]\n\
	 [{-q,--quiet}] [{-d,--debug[=]}[LEVEL]] [{-v,--verbose[=]}[LEVEL]]\n\
    %1$s {-h, -?, --?, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
\n\
    See `%1$s --help' for detailed information.\n\
\n\
", name);
}

/** @brief Print help text.
  * @param name program name.
  * @internal
  *
  * This function prints help information as a results of the -h, --help
  * options.  This is standard OpenSS7 coding practice.  The help text is
  * formatted according to OpenSS7 practices.
  */
void
help(const char *name)
{
	if (!verbose)
		return;
/* *INDENT-OFF* */
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s: [options]\n\
    %1$s: {-h, -?, --?, --help}\n\
    %1$s: {-V, --version}\n\
    %1$s: {-C, --copying}\n\
\n\
Arguments (non-option):\n\
    (none)\n\
\n\
Options:\n\
\n\
  Command Options:\n\
    {-n, --nodaemon}\n\
	run the stats daemon in the foreground\n\
    {-h, -?, --?, --help}\n\
	print this usage information and exit\n\
    {-V, --version}\n\
	print version information and exit\n\
    {-C, --copying}\n\
	print copying permissions and exit\n\
\n\
  Collection Options:\n\
    {-L, --slmx[=]}[DEVNAME[,DEVNAME]]		(default: %9$s,%10$s)\n\
	collect statistics and events for SL-MUX\n\
    {-M, --mtpx[=]}[DEVNAME[,DEVNAME]]		(default: %11$s,%12$s)\n\
	collect statistics and events for MTP\n\
    {-S, --sccp[=]}[DEVNAME[,DEVNAME]]		(default: %13$s,%14$s)\n\
	collect statistics and events for SCCP\n\
    {-T, --tcap[=]}[DEVNAME[,DEVNAME]]		(default: %15$s,%16$s)\n\
	collect statistics and events for TCAP\n\
    {-I, --isup[=]}[DEVNAME[,DEVNAME]]		(default: %17$s,%18$s)\n\
	collect statistics and events for ISUP\n\
\n\
  Daemon Options:\n\
    {-i, --interval=}INTERVAL			(default: %4$ld)\n\
	statistics collection interval milliseconds\n\
    {-O, --outpdir=}DIRECTORY			(default: %5$s)\n\
	specify directory for output files\n\
    {-o, --outfile=}FILENAME			(default: %6$s)\n\
	specify output filename\n\
    {-l, --logfile=}FILENAME			(default: %7$s)\n\
	specify error log filename\n\
    {-f, --cfgfile[=]}[FILENAME]		(default: %8$s)\n\
	specify configuration file and name\n\
\n\
  General Options:\n\
    {-q, --quiet}\n\
	suppress normal output\n\
    {-d, --debug[=]}[LEVEL]			(default: %2$d)\n\
	increase or set debugging output level\n\
    {-v, --verbose[=]}[LEVEL]			(default: %3$d)\n\
	increase or set output verbosity level\n\
\n\
",
	NAME,
	debug,
	verbose,
	interval,
	SS7STATSD_DEFAULT_OUTPDIR,
	SS7STATSD_DEFAULT_OUTFILE,
	SS7STATSD_DEFAULT_ERRFILE,
	SS7STATSD_DEFAULT_CFGFILE,
	SS7STATSD_DEFAULT_DEVSLMX,
	SS7STATSD_DEFAULT_EVTSLMX,
	SS7STATSD_DEFAULT_DEVMTPX,
	SS7STATSD_DEFAULT_EVTMTPX,
	SS7STATSD_DEFAULT_DEVSCCP,
	SS7STATSD_DEFAULT_EVTSCCP,
	SS7STATSD_DEFAULT_DEVTCAP,
	SS7STATSD_DEFAULT_EVTTCAP,
	SS7STATSD_DEFAULT_DEVISUP,
	SS7STATSD_DEFAULT_EVTISUP
	);
/* *INDENT-ON* */
}

int stats_alm_signal = 0;
int stats_hup_signal = 0;
int stats_us1_signal = 0;
int stats_us2_signal = 0;
int stats_trm_signal = 0;

/** @brief Signal handler.
  * @param signum signal number.
  * @internal
  *
  * This is the signal handler.  It simply marks the signal as received and
  * returns.  The main poll loop in stats_wait_event() will wake as a result
  * of the signal and examines these flags when it awakes.
  */
void
stats_sig_handler(int signum)
{
	switch (signum) {
	case SIGALRM:
		stats_alm_signal = 1;
		break;
	case SIGHUP:
		stats_hup_signal = 1;
		break;
	case SIGUSR1:
		stats_us1_signal = 1;
		break;
	case SIGUSR2:
		stats_us2_signal = 1;
		break;
	case SIGTERM:
		stats_trm_signal = 1;
		break;
	}
	return;
}

/** @brief Register or unregister a signal.
  * @param signum signal number.
  * @param handler signal handler or NULL.
  * @internal
  *
  * Registers (handler != NULL) or unregisters (handler == NULL) a signal
  * handler against a signal number.
  */
int
stats_sig_register(int signum, void (*handler) (int))
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

/** @brief Catch signals.
  * @internal
  *
  * Register for all signals of interest.
  */
void
stats_catch(void)
{
	stats_sig_register(SIGALRM, stats_sig_handler);
	stats_sig_register(SIGHUP, stats_sig_handler);
	stats_sig_register(SIGUSR1, stats_sig_handler);
	stats_sig_register(SIGUSR2, stats_sig_handler);
	stats_sig_register(SIGTERM, stats_sig_handler);
}

/** @brief Block signals.
  * @internal
  *
  * Unregister from all signals of interest.
  */
void
stats_block(void)
{
	stats_sig_register(SIGALRM, NULL);
	stats_sig_register(SIGHUP, NULL);
	stats_sig_register(SIGUSR1, NULL);
	stats_sig_register(SIGUSR2, NULL);
	stats_sig_register(SIGTERM, NULL);
}

/** @brief Start interval timer.
  * @param duration duration of timer.
  * @internal
  *
  * Start the interval timer.
  */
int
stats_timer_start(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (stats_sig_register(SIGALRM, stats_sig_handler))
		return (-1);
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return (-1);
	stats_alm_signal = 0;
	return (0);
}

/** @brief Stop interval timer.
  * @internal
  *
  * Stop the interval timer.
  */
int
stats_timer_stop(void)
{
	return stats_sig_register(SIGALRM, NULL);
}

void stats_exit(int retval);

static struct timeval tv;

/** @brief Output a timestamp.
  * @param stream output stream to write to.
  * @internal
  *
  * Outputs the last generated timestamp to an output stream.
  */
void
fprintf_time(FILE * stream)
{
	if (0 > tv.tv_usec || tv.tv_usec > 1000000) {
		tv.tv_usec += (tv.tv_usec / 1000000);
		tv.tv_usec %= 1000000;
	}
	fprintf(stream, "%012ld.%06ld", tv.tv_sec, tv.tv_usec);
}

/** @brief Take a time stamp.
  * @internal
  *
  * Generates a timestamp.
  */
int
ftimestamp(void)
{
	while (gettimeofday(&tv, NULL) < 0)
		if (errno == EAGAIN || errno == EINTR || errno == ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not read timestamp: winging it %s", outpath);
			break;
		}
	return (0);
}

/** @brief Generate output file header.
  * @internal
  *
  * Generates a header for the output file.  This header is generated for each
  * output file opened and also generated when output files are rotated.
  */
void
stats_header(void)
{
	char buf[128] = "";
	struct utsname uts;

	ftimestamp();
	fprintf_time(stdout);
	fprintf(stdout, " # SLSTATSD $Id: ss7statsd.c,v 0.9.2.4 2008-04-29 07:11:33 brian Exp $ Output Header\n");
	uname(&uts);
	fprintf_time(stdout);
	fprintf(stdout, " # machine: %s %s %s %s %s\n", uts.sysname, uts.nodename, uts.release,
		uts.version, uts.machine);
	fprintf_time(stdout);
	if (outpath[0] != '\0')
		snprintf(buf, sizeof(buf), outpath);
	else
		snprintf(buf, sizeof(buf), "(stdout)");
	fprintf(stdout, " # original file name: %s\n", buf);
	fprintf_time(stdout);
	gethostname(buf, sizeof(buf));
	fprintf(stdout, " # host: %s\n", buf);
	fprintf_time(stdout);
	fprintf(stdout, " # date: %s\n", ctime(&tv.tv_sec));
}

void
stats_slmx_print_one_sl(int fd, struct slmux_ppa *slm, bool collect)
{
	int rtn;
	struct strioctl ic;
	struct {
		struct sl_pass pass;
		struct sl_stats stats;
	} ioc;

	ioc.pass.muxid = slm->slm_index;
	ioc.pass.cmd = collect ? SL_IOCCSTATS : SL_IOCGSTATS;
	ic.ic_cmd = SL_IOCCPASS;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(ioc);
	ic.ic_dp = (char *) &ioc;
	while ((rtn = ioctl(fd, I_STR, &ic)) == -1)
		if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not collect statistics for link.");
			break;
		}
	if (rtn == -1)
		return;
	ftimestamp();
	fprintf_time(stdout);
	fprintf(stdout, "%s {\n", collect ? "SL_IOCCSTATS" : "SL_IOCGSTATS");
	fprintf(stdout, "\tindex: %08X\n", slm->slm_index);
	fprintf(stdout, "\tgppa: %d:%d:%d:%d\n",
		(int) ((slm->slm_ppa >> 24) & 0xff),
		(int) ((slm->slm_ppa >> 16) & 0xff),
		(int) ((slm->slm_ppa >> 8) & 0xff), (int) ((slm->slm_ppa >> 0) & 0xff));
	fprintf(stdout, "\tclei: %.32s\n", slm->slm_clei);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\tobject_id: %08X\n", ioc.stats.header.object_id);
	fprintf(stdout, "\tcolperiod: %u\n", ioc.stats.header.colperiod);
	fprintf(stdout, "\ttimestamp: %u\n", ioc.stats.header.timestamp);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\tsl_dur_in_service: %u\n", ioc.stats.sl_dur_in_service);
	fprintf(stdout, "\tsl_fail_align_or_proving: %u\n", ioc.stats.sl_fail_align_or_proving);
	fprintf(stdout, "\tsl_nacks_received: %u\n", ioc.stats.sl_nacks_received);
	fprintf(stdout, "\tsl_dur_unavail: %u\n", ioc.stats.sl_dur_unavail);
	fprintf(stdout, "\tsl_dur_unavail_failed: %u\n", ioc.stats.sl_dur_unavail_failed);
	fprintf(stdout, "\tsl_dur_unavail_rpo: %u\n", ioc.stats.sl_dur_unavail_rpo);
	fprintf(stdout, "\tsl_sibs_sent: %u\n", ioc.stats.sl_sibs_sent);
	fprintf(stdout, "\tsl_tran_sio_sif_octets: %u\n", ioc.stats.sl_tran_sio_sif_octets);
	fprintf(stdout, "\tsl_retrans_octets: %u\n", ioc.stats.sl_retrans_octets);
	fprintf(stdout, "\tsl_tran_msus: %u\n", ioc.stats.sl_tran_msus);
	fprintf(stdout, "\tsl_recv_sio_sif_octets: %u\n", ioc.stats.sl_recv_sio_sif_octets);
	fprintf(stdout, "\tsl_recv_msus: %u\n", ioc.stats.sl_recv_msus);
	fprintf(stdout, "\tsl_cong_onset_ind: %u, %u, %u, %u\n",
		ioc.stats.sl_cong_onset_ind[0],
		ioc.stats.sl_cong_onset_ind[1],
		ioc.stats.sl_cong_onset_ind[2], ioc.stats.sl_cong_onset_ind[3]);
	fprintf(stdout, "\tsl_dur_cong_status: %u, %u, %u, %u\n",
		ioc.stats.sl_dur_cong_status[0],
		ioc.stats.sl_dur_cong_status[1],
		ioc.stats.sl_dur_cong_status[2], ioc.stats.sl_dur_cong_status[3]);
	fprintf(stdout, "}\n\n");
}

void
stats_slmx_print_one_sdt(int fd, struct slmux_ppa *slm, bool collect)
{
	int rtn;
	struct strioctl ic;
	struct {
		struct sdt_pass pass;
		struct sdt_stats stats;
	} ioc;

	ioc.pass.index = slm->slm_index;
	ioc.pass.cmd = collect ? SDT_IOCCSTATS : SDT_IOCGSTATS;
	ic.ic_cmd = SDT_IOCCPASS;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(ioc);
	ic.ic_dp = (char *) &ioc;
	while ((rtn = ioctl(fd, I_STR, &ic)) == -1)
		if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not collect statistics for terminal.");
			break;
		}
	if (rtn == -1)
		return;
	ftimestamp();
	fprintf_time(stdout);
	fprintf(stdout, "%s {\n", collect ? "SDT_IOCCSTATS" : "SDT_IOCGSTATS");
	fprintf(stdout, "\tindex: %08X\n", slm->slm_index);
	fprintf(stdout, "\tgppa: %d:%d:%d:%d\n",
		(int) ((slm->slm_ppa >> 24) & 0xff),
		(int) ((slm->slm_ppa >> 16) & 0xff),
		(int) ((slm->slm_ppa >> 8) & 0xff), (int) ((slm->slm_ppa >> 0) & 0xff));
	fprintf(stdout, "\tclei: %.32s\n", slm->slm_clei);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\tobject_id: %08X\n", ioc.stats.header.object_id);
	fprintf(stdout, "\tcolperiod: %u\n", ioc.stats.header.colperiod);
	fprintf(stdout, "\ttimestamp: %u\n", ioc.stats.header.timestamp);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\ttx_bytes: %u\n", ioc.stats.tx_bytes);
	fprintf(stdout, "\ttx_sus: %u\n", ioc.stats.tx_sus);
	fprintf(stdout, "\ttx_sus_repeated: %u\n", ioc.stats.tx_sus_repeated);
	fprintf(stdout, "\ttx_underruns: %u\n", ioc.stats.tx_underruns);
	fprintf(stdout, "\ttx_aborts: %u\n", ioc.stats.tx_aborts);
	fprintf(stdout, "\ttx_buffer_overflows: %u\n", ioc.stats.tx_buffer_overflows);
	fprintf(stdout, "\ttx_sus_in_error: %u\n", ioc.stats.tx_sus_in_error);
	fprintf(stdout, "\trx_bytes: %u\n", ioc.stats.rx_bytes);
	fprintf(stdout, "\trx_sus: %u\n", ioc.stats.rx_sus);
	fprintf(stdout, "\trx_sus_compressed: %u\n", ioc.stats.rx_sus_compressed);
	fprintf(stdout, "\trx_overruns: %u\n", ioc.stats.rx_overruns);
	fprintf(stdout, "\trx_aborts: %u\n", ioc.stats.rx_aborts);
	fprintf(stdout, "\trx_buffer_overflows: %u\n", ioc.stats.rx_buffer_overflows);
	fprintf(stdout, "\trx_sus_in_error: %u\n", ioc.stats.rx_sus_in_error);
	fprintf(stdout, "\trx_sync_transitions: %u\n", ioc.stats.rx_sync_transitions);
	fprintf(stdout, "\trx_bits_octet_counted: %u\n", ioc.stats.rx_bits_octet_counted);
	fprintf(stdout, "\trx_crc_errors: %u\n", ioc.stats.rx_crc_errors);
	fprintf(stdout, "\trx_frame_errors: %u\n", ioc.stats.rx_frame_errors);
	fprintf(stdout, "\trx_frame_overflows: %u\n", ioc.stats.rx_frame_overflows);
	fprintf(stdout, "\trx_frame_too_long: %u\n", ioc.stats.rx_frame_too_long);
	fprintf(stdout, "\trx_frame_too_short: %u\n", ioc.stats.rx_frame_too_short);
	fprintf(stdout, "\trx_residue_errors: %u\n", ioc.stats.rx_residue_errors);
	fprintf(stdout, "\trx_length_error: %u\n", ioc.stats.rx_length_error);
	fprintf(stdout, "\tcarrier_cts_lost: %u\n", ioc.stats.carrier_cts_lost);
	fprintf(stdout, "\tcarrier_dcd_lost: %u\n", ioc.stats.carrier_dcd_lost);
	fprintf(stdout, "\tcarrier_lost: %u\n", ioc.stats.carrier_lost);
	fprintf(stdout, "}\n\n");

}

void
stats_slmx_print_one_sdl(int fd, struct slmux_ppa *slm, bool collect)
{
	int rtn;
	struct strioctl ic;
	struct {
		struct sdl_pass pass;
		struct sdl_stats stats;
	} ioc;

	ioc.pass.index = slm->slm_index;
	ioc.pass.cmd = collect ? SDL_IOCCSTATS : SDL_IOCGSTATS;
	ic.ic_cmd = SDL_IOCCPASS;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(ioc);
	ic.ic_dp = (char *) &ioc;
	while ((rtn = ioctl(fd, I_STR, &ic)) == -1)
		if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not collect statistics for data link.");
			break;
		}
	if (rtn == -1)
		return;
	ftimestamp();
	fprintf_time(stdout);
	fprintf(stdout, "%s {\n", collect ? "SDL_IOCCSTATS" : "SDL_IOCGSTATS");
	fprintf(stdout, "\tindex: %08X\n", slm->slm_index);
	fprintf(stdout, "\tgppa: %d:%d:%d:%d\n",
		(int) ((slm->slm_ppa >> 24) & 0xff),
		(int) ((slm->slm_ppa >> 16) & 0xff),
		(int) ((slm->slm_ppa >> 8) & 0xff), (int) ((slm->slm_ppa >> 0) & 0xff));
	fprintf(stdout, "\tclei: %.32s\n", slm->slm_clei);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\tobject_id: %08X\n", ioc.stats.header.object_id);
	fprintf(stdout, "\tcolperiod: %u\n", ioc.stats.header.colperiod);
	fprintf(stdout, "\ttimestamp: %u\n", ioc.stats.header.timestamp);
	fprintf(stdout, "\t\n");
	fprintf(stdout, "\trx_octets: %u\n", ioc.stats.rx_octets);
	fprintf(stdout, "\ttx_octets: %u\n", ioc.stats.tx_octets);
	fprintf(stdout, "\trx_overruns: %u\n", ioc.stats.rx_overruns);
	fprintf(stdout, "\ttx_underruns: %u\n", ioc.stats.tx_underruns);
	fprintf(stdout, "\trx_buffer_overflows: %u\n", ioc.stats.rx_buffer_overflows);
	fprintf(stdout, "\ttx_buffer_overflows: %u\n", ioc.stats.tx_buffer_overflows);
	fprintf(stdout, "\tlead_cts_lost: %u\n", ioc.stats.lead_cts_lost);
	fprintf(stdout, "\tlead_dcd_lost: %u\n", ioc.stats.lead_dcd_lost);
	fprintf(stdout, "\tcarrier_lost: %u\n", ioc.stats.carrier_lost);
	fprintf(stdout, "}\n\n");
}

/** @brief Collect or report stats for one signalling link.
  * @param fd signalling link multiplexer file descriptor.
  * @param slm signalling link multiplexer ppa structure for link.
  * @param collect collect (true) or report (false)
  * @internal
  *
  * Collects or reports signalling link statistics for one signalling link at
  * the SL, SDT and SDL sub-levels.
  */
void
stats_slmx_print_one(int fd, struct slmux_ppa *slm, bool collect)
{
	stats_slmx_print_one_sl(fd, slm, collect);
	stats_slmx_print_one_sdt(fd, slm, collect);
	stats_slmx_print_one_sdl(fd, slm, collect);
}

/** @brief Collect SL-MUX statistics.
  * @internal
  *
  * Collect statistics for each signalling link known to the signalling link
  * multiplexing driver.  The statistics device is opened and closed on each
  * collection.  This uses the SLM_IOCLPPA input-output control on the
  * signallin link multiplexer to list all of the signalling links known to
  * the multiplexer and then uses the SL_IOCCPASS, SDT_IOCCPASS, and
  * SDL_IOCCPASS input-output controls to request statistics collection from
  * each of the signalling links linked beneath the SL-MUX driver.
  */
void
stats_slmx_collect(void)
{
	int fd;

	if (devslmx[0] == '\0')
		return;
	if ((fd = open(devslmx, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s.", devslmx);
		return;
	}
	do {
		struct slmux_ppa_list *slm;
		struct slmux_ppa *p;
		struct strioctl ic;
		int n, rtn, len;

		while ((n = ioctl(fd, SLM_IOCLPPA, NULL)) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not obtain number of links.");
				break;
			}
		if (n == -1)
			break;
		len = sizeof(*slm) + n * sizeof(slm->slm_list_array[0]);
		if ((slm = malloc(len)) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not allocate memory of length %d.", len);
			break;
		}
		slm->slm_list_num = n;
		ic.ic_cmd = SLM_IOCLPPA;
		ic.ic_timout = -1;
		ic.ic_len = len;
		ic.ic_dp = (char *) slm;
		while ((rtn = ioctl(fd, I_STR, &ic)) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not retrieve links.");
				break;
			}
		if (rtn == -1)
			break;
		for (n = 0, p = &slm->slm_list_array[0]; n < slm->slm_list_num; n++, p++)
			stats_slmx_print_one(fd, p, true);
	} while (0);
	close(fd);
}

void
stats_mtpx_collect(void)
{
	int fd;

	if (devmtpx[0] == '\0')
		return;
	if ((fd = open(devmtpx, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devmtpx);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_sccp_collect(void)
{
	int fd;

	if (devsccp[0] == '\0')
		return;
	if ((fd = open(devsccp, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devsccp);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_tcap_collect(void)
{
	int fd;

	if (devtcap[0] == '\0')
		return;
	if ((fd = open(devtcap, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devtcap);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_isup_collect(void)
{
	int fd;

	if (devisup[0] == '\0')
		return;
	if ((fd = open(devisup, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devisup);
		return;
	}
	do {
	} while (0);
	close(fd);
}

/** @brief Collect statistics.
  * @internal
  *
  * Collect statistics for each of the SL-MUX, MTP, SCCP, TCAP and ISUP
  * protocol components.  Collection differes from reporting in that
  * collection clears the counts as they are retrieved.
  */
void
stats_collect(void)
{
	stats_slmx_collect();
	stats_mtpx_collect();
	stats_sccp_collect();
	stats_tcap_collect();
	stats_isup_collect();
}

/** @brief Report SL-MUX statistics.
  * @internal
  *
  * Report statistics for each signalling link known to the signalling link
  * multiplexing driver.  The statistics device is opened and closed on each
  * reporting.  This uses the SLM_IOCLPPA input-output control on the
  * signallin link multiplexer to list all of the signalling links known to
  * the multiplexer and then uses the SL_IOCCPASS, SDT_IOCCPASS, and
  * SDL_IOCCPASS input-output controls to request statistics reporting from
  * each of the signalling links linked beneath the SL-MUX driver.
  */
void
stats_slmx_report(void)
{
	int fd;

	if (devslmx[0] == '\0')
		return;
	if ((fd = open(devslmx, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devslmx);
		return;
	}
	do {
		struct slmux_ppa_list *slm;
		struct slmux_ppa *p;
		struct strioctl ic;
		int n, rtn, len;

		while ((n = ioctl(fd, SLM_IOCLPPA, NULL)) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not obtain number of links.");
				break;
			}
		if (n == -1)
			break;
		len = sizeof(*slm) + n * sizeof(slm->slm_list_array[0]);
		if ((slm = malloc(len)) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not allocate memory of length %d.", len);
			break;
		}
		slm->slm_list_num = n;
		ic.ic_cmd = SLM_IOCLPPA;
		ic.ic_timout = -1;
		ic.ic_len = len;
		ic.ic_dp = (char *) slm;
		while ((rtn = ioctl(fd, I_STR, &ic)) == -1)
			if (errno != EAGAIN && errno != EINTR && errno != ERESTART) {
				syslog(LOG_ERR, "%m");
				syslog(LOG_ERR, "Could not retrieve links.");
				break;
			}
		if (rtn == -1)
			break;
		for (n = 0, p = &slm->slm_list_array[0]; n < slm->slm_list_num; n++, p++)
			stats_slmx_print_one(fd, p, false);
	} while (0);
	close(fd);
}

void
stats_mtpx_report(void)
{
	int fd;

	if (devmtpx[0] == '\0')
		return;
	if ((fd = open(devmtpx, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devmtpx);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_sccp_report(void)
{
	int fd;

	if (devsccp[0] == '\0')
		return;
	if ((fd = open(devsccp, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devsccp);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_tcap_report(void)
{
	int fd;

	if (devtcap[0] == '\0')
		return;
	if ((fd = open(devtcap, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devtcap);
		return;
	}
	do {
	} while (0);
	close(fd);
}

void
stats_isup_report(void)
{
	int fd;

	if (devisup[0] == '\0')
		return;
	if ((fd = open(devisup, O_NONBLOCK | O_RDWR)) == -1) {
		syslog(LOG_ERR, "%m");
		syslog(LOG_ERR, "Could not open device %s", devisup);
		return;
	}
	do {
	} while (0);
	close(fd);
}

/** @brief Report statistics.
  * @internal
  *
  * Report statistics for each of the SL-MUX, MTP, SCCP, TCAP and ISUP
  * protocol components.  Reporting differs from collection in that
  * reporting does not clear the counts as they are retrieved.
  */
void
stats_report(void)
{
	stats_slmx_report();
	stats_mtpx_report();
	stats_sccp_report();
	stats_tcap_report();
	stats_isup_report();
}

/** @brief SIGTERM signal action.
  * @internal
  *
  * SIGTERM results from the process being killed, normally by the init
  * script.  This action permits the daemon to flush its output and close
  * output and log files gracefully.
  */
void
stats_trm_action(void)
{
	syslog(LOG_WARNING, "Caught SIGTERM, shutting down");
	stats_exit(0);
}

/** @brief SIGHUP signal action.
  * @internal
  *
  * SIGHUP is expected to be sent by an external process that wishes to rotate
  * the output and log files generated by the program.  This action,
  * therefore, closes and reopens each of the output and log files.  This
  * permits logotate(8) to move the corresponding files, and then issue a
  * SIGHUP to the daemon to gracefully close and reopen files using the
  * original name.
  */
void
stats_hup_action(void)
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
		stats_header();
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
		stats_header();
	}
}

/** @brief SIGALRM signal action.
  * @internal
  *
  * SIGARLM results from the expiry of the interval timer.  The interval timer
  * is used to implement the collection interval, so each time that the timer
  * expires, a collections needs to be performed and the timer needs to be
  * reset for the next collection interval.
  */
void
stats_alm_action(void)
{
	stats_timer_start(interval);
	stats_collect();
}

/** @brief SIGUSR1 signal action.
  * @internal
  *
  * SIGUSR1 is expected to be generated by an external process that requests
  * that statistics be collected immediately.  The different between reporting
  * and collection is that reporting does not reset counts where as collection
  * does.
  */
void
stats_us1_action(void)
{
	stats_collect();
}

/** @brief SIGUSR2 signal action.
  * @internal
  *
  * SIGUSR2 is expected to be generated by an external process that requests
  * that statistics be reported immediately.  The different between reporting
  * and collection is that reporting does not reset counts where as collection
  * does.
  */
void
stats_us2_action(void)
{
	stats_report();
}

void
stats_open(void)
{
	pfd_num = 0;

	if (evtslmx[0] != '\0') {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Opening slmx event device.");
		if ((slmx_fd = open(evtslmx, O_NONBLOCK | O_RDWR)) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not open slmx event device.");
		} else
			pfd[pfd_num++].fd = slmx_fd;
	}
	if (evtmtpx[0] != '\0') {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Opening mtpx event device.");
		if ((mtpx_fd = open(evtmtpx, O_NONBLOCK | O_RDWR)) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not open mtpx event device.");
		} else
			pfd[pfd_num++].fd = mtpx_fd;
	}
	if (evtsccp[0] != '\0') {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Opening sccp event device.");
		if ((sccp_fd = open(evtsccp, O_NONBLOCK | O_RDWR)) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not open sccp event device.");
		} else
			pfd[pfd_num++].fd = sccp_fd;
	}
	if (evttcap[0] != '\0') {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Opening tcap event device.");
		if ((tcap_fd = open(evttcap, O_NONBLOCK | O_RDWR)) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not open tcap event device.");
		} else
			pfd[pfd_num++].fd = tcap_fd;
	}
	if (evtisup[0] != '\0') {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Opening isup event device.");
		if ((isup_fd = open(evtisup, O_NONBLOCK | O_RDWR)) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not open isup event device.");
		} else
			pfd[pfd_num++].fd = isup_fd;
	}
}

void
stats_close(void)
{
	if (isup_fd != -1) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Closing isup event device.");
		if (close(isup_fd) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Error closing isup event device.");
		}
		pfd[--pfd_num].fd = -1;
	}
	if (tcap_fd != -1) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Closing tcap event device.");
		if (close(tcap_fd) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Error closing tcap event device.");
		}
		pfd[--pfd_num].fd = -1;
	}
	if (sccp_fd != -1) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Closing sccp event device.");
		if (close(sccp_fd) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Error closing sccp event device.");
		}
		pfd[--pfd_num].fd = -1;
	}
	if (mtpx_fd != -1) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Closing mtpx event device.");
		if (close(mtpx_fd) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Error closing mtpx event device.");
		}
		pfd[--pfd_num].fd = -1;
	}
	if (slmx_fd != -1) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Closing slmx event device.");
		if (close(slmx_fd) == -1) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Error closing slmx event device.");
		}
		pfd[--pfd_num].fd = -1;
	}
}

void
stats_exit(int retval)
{
	stats_timer_stop();
	stats_block();
	stats_close();
	syslog(LOG_NOTICE, "Exiting %d", retval);
	fflush(stdout);
	fflush(stderr);
	closelog();
	exit(retval);
}

int
stats_wait_event(void)
{
	int n;

	for (;;) {
		if (stats_trm_signal) {
			stats_trm_signal = 0;
			stats_trm_action();
		}
		if (stats_alm_signal) {
			stats_alm_signal = 0;
			stats_alm_action();
		}
		if (stats_hup_signal) {
			stats_hup_signal = 0;
			stats_hup_action();
		}
		if (stats_us1_signal) {
			stats_us1_signal = 0;
			stats_us1_action();
		}
		if (stats_us2_signal) {
			stats_us2_signal = 0;
			stats_us2_action();
		}
		if (verbose > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, pfd_num, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			syslog(LOG_ERR, "%s: %m", __FUNCTION__);
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			stats_exit(1);
			return (-1);
		case 0:
			return (0);
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			for (n = 0; n < pfd_num; n++) {
				if (pfd[n].revents & (POLLIN | POLLPRI)) {
					/* FIXME: hook for events */
					syslog(LOG_ERR, "%s: device output", __FUNCTION__);
					stats_exit(1);
					return (-1);
				}
				if (pfd[n].revents & POLLNVAL) {
					syslog(LOG_ERR, "%s: device invalid", __FUNCTION__);
					stats_exit(1);
					return (-1);
				}
				if (pfd[n].revents & POLLHUP) {
					syslog(LOG_ERR, "%s: device hangup", __FUNCTION__);
					stats_exit(1);
					return (-1);
				}
				if (pfd[n].revents & POLLERR) {
					syslog(LOG_ERR, "%s: device error", __FUNCTION__);
					stats_exit(1);
					return (-1);
				}
			}
			break;
		default:
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			stats_exit(1);
			return (-1);
		}
	}
}

void
stats_enter(void)
{
	if (nomead) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			perror(NAME);
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		/* child continues */
		setsid();	/* become a session leader */
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror(NAME);
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		/* child continues */
		/* release current directory */
		if (chdir("/") == -1) {
			perror(NAME);
			exit(2);
		}
		umask(0);	/* clear file creation mask */
		/* rearrange file streams */
		fclose(stdin);
	}
	/* continue in foreground or background */
	openlog(NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON);
	if (nomead) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Redirecting stdout to file %s", outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stdout to %s", outpath);
			stats_exit(2);
		}
	}
	if (nomead) {
		if (verbose > 1)
			syslog(LOG_NOTICE, "Redirecting stderr to file %s", errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stderr to %s", errpath);
			stats_exit(2);
		}
	}
	stats_catch();
	stats_header();
	syslog(LOG_NOTICE, "Startup complete.");
}

void
stats(void)
{
	stats_enter();
	stats_open();
	stats_timer_start(interval);
	while (stats_wait_event() != -1) ;
	stats_timer_stop();
	stats_close();
	stats_exit(1);
}

void
option_error(int argc, char *argv[], int optind, const char *str, int retval)
{
	fprintf(stderr, "ERROR: %s: %s --", str, basename(argv[0]));
	while (optind < argc)
		fprintf(stderr, " %s", argv[optind++]);
	fprintf(stderr, "\n");
	usage(basename(argv[0]));
	exit(retval);
}

int
main(int argc, char *argv[])
{
	int c;
	long val;
	char *optstr;
	struct stat st;

	for (;;) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"interval",	required_argument,	NULL, 'i'},
			{"nodaemon",	no_argument,		NULL, 'n'},
			{"outpdir",	required_argument,	NULL, 'O'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"logfile",	required_argument,	NULL, 'l'},
			{"cfgfile",	optional_argument,	NULL, 'f'},
			{"slmx",	optional_argument,	NULL, 'L'},
			{"mtpx",	optional_argument,	NULL, 'M'},
			{"sccp",	optional_argument,	NULL, 'S'},
			{"tcap",	optional_argument,	NULL, 'T'},
			{"isup",	optional_argument,	NULL, 'I'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"?",		no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 },
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, ":w:i:nO:o:l:f:L::M::S::T::I::qd::v::hVC?",
				long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'i':	/* -i, --interval INTERVAL */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || optstr[0] != '\0' || val < 1000)
				option_error(argc, argv, optind - 1, "illegal argument", 2);
			interval = val;
			break;
		case 'n':	/* -n, --nodaemon */
			nomead = 0;
			break;
		case 'O':	/* -O, --outpdir DIRECTORY */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (stat(optarg, &st) == -1)
				option_error(argc, argv, optind - 1, strerror(errno), 2);
			if (!S_ISDIR(st.st_mode))
				option_error(argc, argv, optind - 1, "not a directory", 2);
			strncpy(outpdir, optarg, sizeof(outpdir));
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (optarg[0] != '/') {
				strncpy(outfile, optarg, sizeof(outfile));
				break;
			}
			strncpy(outpath, optarg, sizeof(outpath));
			break;
		case 'l':	/* -l, --logfile LOGFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (optarg[0] != '/') {
				strncpy(errfile, optarg, sizeof(errfile));
				break;
			}
			strncpy(errpath, optarg, sizeof(errpath));
			break;
		case 'f':	/* -f, --cfgfile [CFGFILE] */
			if (optarg != NULL)
				strncpy(cfgfile, optarg, sizeof(cfgfile));
			useconfig = 1;
			break;
		case 'L':	/* -L, --slmx [DEVNAME[,DEVNAME]] */
			if ((optstr = strsep(&optarg, ",")) != NULL) {
				strncpy(devslmx, optarg, sizeof(devslmx));
				if ((optstr = strsep(&optarg, ",")) != NULL)
					strncpy(evtslmx, optarg, sizeof(evtslmx));
				else
					strncpy(evtslmx, SS7STATSD_DEFAULT_EVTSLMX,
						sizeof(evtslmx));
			} else {
				strncpy(devslmx, SS7STATSD_DEFAULT_DEVSLMX, sizeof(devslmx));
				strncpy(evtslmx, SS7STATSD_DEFAULT_EVTSLMX, sizeof(evtslmx));
			}
			if (evtslmx[0] == '\0')
				strncpy(evtslmx, devslmx, sizeof(devslmx));
			break;
		case 'M':	/* -M, --mtpx [DEVNAME[,DEVNAME]] */
			if ((optstr = strsep(&optarg, ",")) != NULL) {
				strncpy(devmtpx, optarg, sizeof(devmtpx));
				if ((optstr = strsep(&optarg, ",")) != NULL)
					strncpy(evtmtpx, optarg, sizeof(evtmtpx));
				else
					strncpy(evtmtpx, SS7STATSD_DEFAULT_EVTMTPX,
						sizeof(evtmtpx));
			} else {
				strncpy(devmtpx, SS7STATSD_DEFAULT_DEVMTPX, sizeof(devmtpx));
				strncpy(evtmtpx, SS7STATSD_DEFAULT_EVTMTPX, sizeof(evtmtpx));
			}
			if (evtmtpx[0] == '\0')
				strncpy(evtmtpx, devmtpx, sizeof(devmtpx));
			break;
		case 'S':	/* -S, --sccp [DEVNAME[,DEVNAME]] */
			if ((optstr = strsep(&optarg, ",")) != NULL) {
				strncpy(devsccp, optarg, sizeof(devsccp));
				if ((optstr = strsep(&optarg, ",")) != NULL)
					strncpy(evtsccp, optarg, sizeof(evtsccp));
				else
					strncpy(evtsccp, SS7STATSD_DEFAULT_EVTSCCP,
						sizeof(evtsccp));
			} else {
				strncpy(devsccp, SS7STATSD_DEFAULT_DEVSCCP, sizeof(devsccp));
				strncpy(evtsccp, SS7STATSD_DEFAULT_EVTSCCP, sizeof(evtsccp));
			}
			if (evtsccp[0] == '\0')
				strncpy(evtsccp, devsccp, sizeof(devsccp));
			break;
		case 'T':	/* -T, --tcap [DEVNAME[,DEVNAME]] */
			if ((optstr = strsep(&optarg, ",")) != NULL) {
				strncpy(devtcap, optarg, sizeof(devtcap));
				if ((optstr = strsep(&optarg, ",")) != NULL)
					strncpy(evttcap, optarg, sizeof(evttcap));
				else
					strncpy(evttcap, SS7STATSD_DEFAULT_EVTTCAP,
						sizeof(evttcap));
			} else {
				strncpy(devtcap, SS7STATSD_DEFAULT_DEVTCAP, sizeof(devtcap));
				strncpy(evttcap, SS7STATSD_DEFAULT_EVTTCAP, sizeof(evttcap));
			}
			if (evttcap[0] == '\0')
				strncpy(evttcap, devtcap, sizeof(devtcap));
			break;
		case 'I':	/* -I, --isup [DEVNAME[,DEVNAME]] */
			if ((optstr = strsep(&optarg, ",")) != NULL) {
				strncpy(devisup, optarg, sizeof(devisup));
				if ((optstr = strsep(&optarg, ",")) != NULL)
					strncpy(evtisup, optarg, sizeof(evtisup));
				else
					strncpy(evtisup, SS7STATSD_DEFAULT_EVTISUP,
						sizeof(evtisup));
			} else {
				strncpy(devisup, SS7STATSD_DEFAULT_DEVISUP, sizeof(devisup));
				strncpy(evtisup, SS7STATSD_DEFAULT_EVTISUP, sizeof(evtisup));
			}
			if (evtisup[0] == '\0')
				strncpy(evtisup, devisup, sizeof(devisup));
			break;
		case 'q':	/* -q, --quiet */
			verbose -= verbose > 1 ? 1 : verbose;
			break;
		case 'd':	/* -d, --debug [LEVEL] */
			if (optarg != NULL) {
				val = strtol(optarg, &optstr, 0);
				if (optstr == optarg || optstr[0] != '\0' || val < 0)
					option_error(argc, argv, optind - 1, "illegal argument", 2);
				debug = val;
			}
			debug++;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg != NULL) {
				val = strtol(optarg, &optstr, 0);
				if (optstr == optarg || optstr[0] != '\0' || val < 0)
					option_error(argc, argv, optind - 1, "illegal argument", 2);
				verbose = val;
			}
			verbose++;
			break;
		case 'h':	/* -h, --help */
			help(basename(argv[0]));
			exit(0);
		case 'V':	/* -V, --version */
			version(basename(argv[0]));
			exit(0);
		case 'C':	/* -C, --copying */
			copying(basename(argv[0]));
			exit(0);
		case ':':	/* required argument */
			option_error(argc, argv, optind - 1, "required argument", 2);
		case '0':
		case '?':	/* unrecognized option */
			option_error(argc, argv, optind - 1, "unrecognized option", 2);
		default:	/* unrecognized switch */
			option_error(argc, argv, optind - 1, "syntax error", 2);
		}
	}
	if (optind < argc)
		option_error(argc, argv, optind, "extra arguments", 2);
	/* organize path names */
	if (outpath[0] == '\0') {
		if (outpdir[0] == '\0')
			strncpy(outpdir, SS7STATSD_DEFAULT_OUTPDIR, sizeof(outpdir));
		if (outfile[0] == '\0')
			strncpy(outfile, SS7STATSD_DEFAULT_OUTFILE, sizeof(outfile));
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
	}
	if (errpath[0] == '\0') {
		if (outpdir[0] == '\0')
			strncpy(outpdir, SS7STATSD_DEFAULT_OUTPDIR, sizeof(outpdir));
		if (errfile[0] == '\0')
			strncpy(errfile, SS7STATSD_DEFAULT_ERRFILE, sizeof(errfile));
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
	}
	if (cfgfile[0] == '\0') {
		strncpy(cfgfile, SS7STATSD_DEFAULT_CFGFILE, sizeof(cfgfile));
	}
	exit(0);
}

/** @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
