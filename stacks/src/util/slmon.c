/*****************************************************************************

 @(#) $RCSfile: slmon.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $

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

 Last Modified $Date: 2007/01/15 11:33:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slmon.c,v $
 Revision 0.9.2.1  2007/01/15 11:33:57  brian
 - added new and old signalling link utilities

 *****************************************************************************/

#ident "@(#) $RCSfile: slmon.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $"

static char const ident[] = "$RCSfile: slmon.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $";

/*
 * This is a signalling link monitoring utiltiy for the SL-MUX multiplexing driver.  It purpose is
 * to open signalling link monitoring streams, /dev/sl-mon, on the SL-MUX driver, and attach them to
 * specific signalling links using the global PPA or CLEI for the signalling link.  The utility then
 * outputs configuration and monitoring information obtained from the signalling link service
 * interface and formats the information and outputs it to either standard output of a file.  When a
 * file is specified as an option, the utility closes standard input, output and error, and places
 * itself in the background as a daemon process.
 */

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

int output = 1;
int nomead = 0;

char clei[32] = "";
int ppa = 0;

int mon_fd = -1;

int link_state = 0;

char outfile[256] = "";
char errfile[256] = "";
char outpath[256] = "";
char errpath[256] = "";
char lnkname[256] = "";
char cfgfile[256] = "";
char outpdir[256] = "/var/log/slmon";
char devname[256] = "/dev/sl-mon";

int alm_signal = 0;
int hup_signal = 0;
int trm_signal = 0;
int usr_signal = 0;

#define BUFSIZE 512

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { sizeof(cbuf), 0, cbuf };
struct strbuf ctrl = { sizeof(dbuf), 0, dbuf };

struct strioctl ctl;

struct monconfig {
	lmi_option_t opt;
	sl_config_t sl;
	sdt_config_t sdt;
	sdl_config_t sdl;
} monconf;

void
sig_handler(int signum)
{
	switch (signum) {
	case SIGALRM:
		alm_signal = 1;
		break;
	case SIGUSR1:
		usr_signal = 1;
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
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, signum);
	sigprocmask(handler ? SIG_UNBLOCK : SIG_BLOCK, &mask, NULL);
	return 0;
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

int
usr_catch(void)
{
	return sig_register(SIGUSR1, sig_handler);
}

void
sig_catch(void)
{
	hup_catch();
	alm_catch();
	trm_catch();
	usr_catch();
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

int
usr_block(void)
{
	return sig_register(SIGUSR1, NULL);
}

void
sig_block(void)
{
	hup_block();
	alm_block();
	trm_block();
	usr_block();
}

int mon_stop(void);

int
mon_exit(int retval)
{
	syslog(LOG_NOTICE, "Exiting %d", retval);
	fflush(stdout);
	fflush(stderr);
	mon_stop();
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
}

int
ftimestamp(void)
{
	while (gettimeofday(&tv, NULL) < 0)
		if (errno == EAGAIN || errno == EINTR || errno == ERESTART) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not read timestamp: winging it%s", outpath);
			break;
		}
	return (0);
}

/* generate output header */
void
output_header(void)
{
	unsigned char buf[128] = "";
	struct utsname uts;

	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout, " # SS7MON $Id: slmon.c,v 0.9.2.1 2007/01/15 11:33:57 brian Exp $ Output File Header\n");
	uname(&uts);
	fprint_time(stdout);
	fprint(stdout, " # machine %s %s %s %s %s\n", uts.sysname, uts.nodename, uts.release,
	       uts.version, uts.machine);
	fprint_time(stdout);
	fprintf(stdout, " # device: %08x %s\n", ppa, clei);
	fprint_time(stdout);
	if (outpath[0] != '\0')
		snprintf(buf, sizeof(buf), outpath);
	else
		snprintf(buf, sizeof(buf), "(stdout)");
	fprintf(stdout, " # original file name; %s\n", buf);
	fprint_time(stdout);
	gethostname(buf, sizeof(buf));
	fprintf(stdout, " # host: %s\n", buf);
	fprint_time(stdout);
	fprintf(stdout, " # date: %s\n", ctime(&tv.tv_sec));
}

void
trm_action(void)
{
	syslog(LOG_WARNING, "Caught SIGTERM, shutting down");
	mon_exit(0);
}

int mon_config(void);
void output_config(void);

void
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
		mon_config();
		output_config();
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

void
alm_action(void)
{
	return (0);
}

void
usr_action(void)
{
	mon_config();
	output_config();
}

int
start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (alm_catch())
		return (-1);
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return (-1);
	alm_signal = 0;
	return (0);
}

int
stop_timer(void)
{
	return alm_block();
}

int
print_data(caddr_t ptr, size_t len)
{
	static unsigned char hexchar[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	int i;

	for (i = 0; i < len; i++) {
		fputc(hexchar[(ptr[i] >> 4) & 0x0f], stdout);
		fputc(hexchar[(ptr[i] >> 0) & 0x0f], stdout);
	}
}
int
decode_data(void)
{
	ftimestamp();
	fprint_time(stdout);
	fputc(' ', stdout);
	print_data((caddr_t) dbuf, data.len);
	fputc('\n', stdout);
	return (0);
}

int
decode_ctrl(void)
{
	union {
		union LMI_primitives lmi;
		union SL_primitives sl;
	} *p = (typeof(p)) cbuf;
	uint32_t prim = *(uint32_t *) cbuf;

	ftimestamp();
	if (output > 2)
		fprintf(stderr, "Got control message! %ld\n", prim);
	fprint_time(stdout);
	switch (prim) {
	case LMI_INFO_REQ:
		fprintf(stdout, "ctrl=LMI_INFO_REQ");
		break;
	case LMI_ATTACH_REQ:
		fprintf(stdout, "ctrl=LMI_ATTACH_REQ");
		fprintf(stdout, "{lmi_ppa=");
		print_data((caddr_t) p->lmi.attach_req.lmi_ppa,
			   ctrl.len - sizeof(p->lmi.attach_req));
		fputc(stdout, '}');
		break;
	case LMI_DETACH_REQ:
		fprintf(stdout, "ctrl=LMI_DETACH_REQ");
		break;
	case LMI_ENABLE_REQ:
		fprintf(stdout, "ctrl=LMI_ENABLE_REQ");
		fprintf(stdout, "{lmi_rem");
		print_data((caddr_t) p->lmi.enable_req.lmi_rem,
			   ctrl.len - sizeof(p->lmi.enable_req));
		fputc(stdout, '}');
		break;
	case LMI_DISABLE_REQ:
		fprintf(stdout, "ctrl=LMI_DISABLE_REQ");
		break;
	case LMI_OPTMGMT_REQ:
		fprintf(stdout, "ctrl=LMI_OPTMGMT_REQ");
		break;
	case LMI_INFO_ACK:
		fprintf(stdout, "ctrl=LMI_INFO_ACK");
		fprintf(stdout, "{lmi_version=%u", p->lmi.info_ack.lmi_version);
		fprintf(stdout, ",lmi_state=");
		switch (p->lmi.info_ack.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.info_ack.lmi_state);
			break;
		}
		fprintf(stdout, ",lmi_max_sdu=%u", p->lmi.info_ack.lmi_max_sdu);
		fprintf(stdout, ",lmi_min_sdu=%u", p->lmi.info_ack.lmi_min_sdu);
		fprintf(stdout, ",lmi_header_len=%u", p->lmi.info_ack.lmi_header_len);
		fprintf(stdout, ",lmi_ppa_style=");
		switch (p->lmi.info_ack.lmi_ppa_style) {
		case LMI_STYLE1:
			fprintf(stdout, "LMI_STYLE1");
			break;
		case LMI_STYLE2:
			fprintf(stdout, "LMI_STYLE2");
			break;
		default:
			fprintf(stdout, "%u", p->lmi.info_ack.lmi_ppa_style);
			break;
		}
		fprintf(stdout, ",lmi_ppa_addr=");
		print_data((caddr_t) p->lmi.info_ack.lmi_ppa_addr,
			   ctrl.len - sizeof(p->lmi.info_ack));
		fputc(stdout, '}');
		break;
	case LMI_OK_ACK:
		fprintf(stdout, "ctrl=LMI_OK_ACK");
		fprintf(stdout, "{lmi_correct_primitive=");
		switch (p->lmi.ok_ack.lmi_correct_primitive) {
		case LMI_INFO_REQ:
			fprintf(stdout, "LMI_INFO_REQ");
			break;
		case LMI_ATTACH_REQ:
			fprintf(stdout, "LMI_ATTACH_REQ");
			break;
		case LMI_DETACH_REQ:
			fprintf(stdout, "LMI_DETACH_REQ");
			break;
		case LMI_ENABLE_REQ:
			fprintf(stdout, "LMI_ENABLE_REQ");
			break;
		case LMI_DISABLE_REQ:
			fprintf(stdout, "LMI_DISABLE_REQ");
			break;
		case LMI_OPTMGMT_REQ:
			fprintf(stdout, "LMI_OPTMGMT_REQ");
			break;
		case LMI_INFO_ACK:
			fprintf(stdout, "LMI_INFO_ACK");
			break;
		case LMI_OK_ACK:
			fprintf(stdout, "LMI_OK_ACK");
			break;
		case LMI_ERROR_ACK:
			fprintf(stdout, "LMI_ERROR_ACK");
			break;
		case LMI_ENABLE_CON:
			fprintf(stdout, "LMI_ENABLE_CON");
			break;
		case LMI_DISABLE_CON:
			fprintf(stdout, "LMI_DISABLE_CON");
			break;
		case LMI_OPTMGMT_ACK:
			fprintf(stdout, "LMI_OPTMGMT_ACK");
			break;
		case LMI_ERROR_IND:
			fprintf(stdout, "LMI_ERROR_IND");
			break;
		case LMI_STATS_IND:
			fprintf(stdout, "LMI_STATS_IND");
			break;
		case LMI_EVENT_IND:
			fprintf(stdout, "LMI_EVENT_IND");
			break;
		default:
			fprintf(stdout, "%u", p->lmi.ok_ack.lmi_correct_primitive);
			break;
		}
		fprintf(stdout, ",lmi_state=");
		switch (p->lmi.ok_ack.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.ok_ack.lmi_state);
			break;
		}
		fputc(stdout, '}');
		break;
	case LMI_ERROR_ACK:
		fprintf(stdout, "ctrl=LMI_ERROR_ACK");
		fprintf(stdout, "{lmi_errno=\"%s\"", strerror(p->lmi.error_ack.lmi_errno));
		fprintf(stdout, ",lmi_reason=");
		switch (p->lmi.error_ack.lmi_reason) {
		case LMI_UNSPEC:
			fprintf(stdout, "LMI_UNSPEC");
			break;
		case LMI_BADADDRESS:
			fprintf(stdout, "LMI_BADADDRESS");
			break;
		case LMI_BADADDRTYPE:
			fprintf(stdout, "LMI_BADADDRTYPE");
			break;
		case LMI_BADDIAL:
			fprintf(stdout, "LMI_BADDIAL");
			break;
		case LMI_BADDIALTYPE:
			fprintf(stdout, "LMI_BADDIALTYPE");
			break;
		case LMI_BADDISPOSAL:
			fprintf(stdout, "LMI_BADDISPOSAL");
			break;
		case LMI_BADFRAME:
			fprintf(stdout, "LMI_BADFRAME");
			break;
		case LMI_BADPPA:
			fprintf(stdout, "LMI_BADPPA");
			break;
		case LMI_BADPRIM:
			fprintf(stdout, "LMI_BADPRIM");
			break;
		case LMI_DISC:
			fprintf(stdout, "LMI_DISC");
			break;
		case LMI_EVENT:
			fprintf(stdout, "LMI_EVENT");
			break;
		case LMI_FATALERR:
			fprintf(stdout, "LMI_FATALERR");
			break;
		case LMI_INITFAILED:
			fprintf(stdout, "LMI_INITFAILED");
			break;
		case LMI_NOTSUPP:
			fprintf(stdout, "LMI_NOTSUPP");
			break;
		case LMI_OUTSTATE:
			fprintf(stdout, "LMI_OUTSTATE");
			break;
		case LMI_PROTOSHORT:
			fprintf(stdout, "LMI_PROTOSHORT");
			break;
		case LMI_SYSERR:
			fprintf(stdout, "LMI_SYSERR");
			break;
		case LMI_WRITEFAIL:
			fprintf(stdout, "LMI_WRITEFAIL");
			break;
		case LMI_CRCERR:
			fprintf(stdout, "LMI_CRCERR");
			break;
		case LMI_DLE_EOT:
			fprintf(stdout, "LMI_DLE_EOT");
			break;
		case LMI_FORMAT:
			fprintf(stdout, "LMI_FORMAT");
			break;
		case LMI_HDLC_ABORT:
			fprintf(stdout, "LMI_HDLC_ABORT");
			break;
		case LMI_OVERRUN:
			fprintf(stdout, "LMI_OVERRUN");
			break;
		case LMI_TOOSHORT:
			fprintf(stdout, "LMI_TOOSHORT");
			break;
		case LMI_INCOMPLETE:
			fprintf(stdout, "LMI_INCOMPLETE");
			break;
		case LMI_BUSY:
			fprintf(stdout, "LMI_BUSY");
			break;
		case LMI_NOANSWER:
			fprintf(stdout, "LMI_NOANSWER");
			break;
		case LMI_CALLREJECT:
			fprintf(stdout, "LMI_CALLREJECT");
			break;
		case LMI_HDLC_IDLE:
			fprintf(stdout, "LMI_HDLC_IDLE");
			break;
		case LMI_HDLC_NOTIDLE:
			fprintf(stdout, "LMI_HDLC_NOTIDLE");
			break;
		case LMI_QUIESCENT:
			fprintf(stdout, "LMI_QUIESCENT");
			break;
		case LMI_RESUMED:
			fprintf(stdout, "LMI_RESUMED");
			break;
		case LMI_DSRTIMEOUT:
			fprintf(stdout, "LMI_DSRTIMEOUT");
			break;
		case LMI_LAN_COLLISIONS:
			fprintf(stdout, "LMI_LAN_COLLISIONS");
			break;
		case LMI_LAN_REFUSED:
			fprintf(stdout, "LMI_LAN_REFUSED");
			break;
		case LMI_LAN_NOSTATION:
			fprintf(stdout, "LMI_LAN_NOSTATION");
			break;
		case LMI_LOSTCTS:
			fprintf(stdout, "LMI_LOSTCTS");
			break;
		case LMI_DEVERR:
			fprintf(stdout, "LMI_DEVERR");
			break;
		default:
			fprintf(stdout, "%u", p->lmi.error_ack.lmi_reason);
			break;
		}
		fprintf(stdout, ",lmi_error_primitive=%u", p->lmi.error_ack.lmi_error_primitive);
		switch (p->lmi.error_ack.lmi_error_primitive) {
		case LMI_INFO_REQ:
			fprintf(stdout, "LMI_INFO_REQ");
			break;
		case LMI_ATTACH_REQ:
			fprintf(stdout, "LMI_ATTACH_REQ");
			break;
		case LMI_DETACH_REQ:
			fprintf(stdout, "LMI_DETACH_REQ");
			break;
		case LMI_ENABLE_REQ:
			fprintf(stdout, "LMI_ENABLE_REQ");
			break;
		case LMI_DISABLE_REQ:
			fprintf(stdout, "LMI_DISABLE_REQ");
			break;
		case LMI_OPTMGMT_REQ:
			fprintf(stdout, "LMI_OPTMGMT_REQ");
			break;
		case LMI_INFO_ACK:
			fprintf(stdout, "LMI_INFO_ACK");
			break;
		case LMI_OK_ACK:
			fprintf(stdout, "LMI_OK_ACK");
			break;
		case LMI_ERROR_ACK:
			fprintf(stdout, "LMI_ERROR_ACK");
			break;
		case LMI_ENABLE_CON:
			fprintf(stdout, "LMI_ENABLE_CON");
			break;
		case LMI_DISABLE_CON:
			fprintf(stdout, "LMI_DISABLE_CON");
			break;
		case LMI_OPTMGMT_ACK:
			fprintf(stdout, "LMI_OPTMGMT_ACK");
			break;
		case LMI_ERROR_IND:
			fprintf(stdout, "LMI_ERROR_IND");
			break;
		case LMI_STATS_IND:
			fprintf(stdout, "LMI_STATS_IND");
			break;
		case LMI_EVENT_IND:
			fprintf(stdout, "LMI_EVENT_IND");
			break;
		default:
			fprintf(stdout, "%u", p->lmi.error_ack.lmi_error_primitive);
			break;
		}
		fprintf(stdout, ",lmi_state=");
		switch (p->lmi.error_ack.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.error_ack.lmi_state);
			break;
		}
		fputc(stdout, '}');
		break;
	case LMI_ENABLE_CON:
		fprintf(stdout, "ctrl=LMI_ENABLE_CON");
		fprintf(stdout, "{lmi_state=");
		switch (p->lmi.enable_con.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.enable_con.lmi_state);
			break;
		}
		fputc(stdout, '}');
		break;
	case LMI_DISABLE_CON:
		fprintf(stdout, "ctrl=LMI_DISABLE_CON");
		fprintf(stdout, "{lmi_state=");
		switch (p->lmi.disable_con.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.disable_con.lmi_state);
			break;
		}
		break;
	case LMI_OPTMGMT_ACK:
		fprintf(stdout, "ctrl=LMI_OPTMGMT_ACK");
		break;
	case LMI_ERROR_IND:
		fprintf(stdout, "ctrl=LMI_ERROR_IND");
		fprintf(stdout, "{lmi_errno=\"%s\"", strerror(p->lmi.error_ind.lmi_errno));
		fprintf(stdout, ",lmi_reason=");
		switch (p->lmi.error_ind.lmi_reason) {
		case LMI_UNSPEC:
			fprintf(stdout, "LMI_UNSPEC");
			break;
		case LMI_BADADDRESS:
			fprintf(stdout, "LMI_BADADDRESS");
			break;
		case LMI_BADADDRTYPE:
			fprintf(stdout, "LMI_BADADDRTYPE");
			break;
		case LMI_BADDIAL:
			fprintf(stdout, "LMI_BADDIAL");
			break;
		case LMI_BADDIALTYPE:
			fprintf(stdout, "LMI_BADDIALTYPE");
			break;
		case LMI_BADDISPOSAL:
			fprintf(stdout, "LMI_BADDISPOSAL");
			break;
		case LMI_BADFRAME:
			fprintf(stdout, "LMI_BADFRAME");
			break;
		case LMI_BADPPA:
			fprintf(stdout, "LMI_BADPPA");
			break;
		case LMI_BADPRIM:
			fprintf(stdout, "LMI_BADPRIM");
			break;
		case LMI_DISC:
			fprintf(stdout, "LMI_DISC");
			break;
		case LMI_EVENT:
			fprintf(stdout, "LMI_EVENT");
			break;
		case LMI_FATALERR:
			fprintf(stdout, "LMI_FATALERR");
			break;
		case LMI_INITFAILED:
			fprintf(stdout, "LMI_INITFAILED");
			break;
		case LMI_NOTSUPP:
			fprintf(stdout, "LMI_NOTSUPP");
			break;
		case LMI_OUTSTATE:
			fprintf(stdout, "LMI_OUTSTATE");
			break;
		case LMI_PROTOSHORT:
			fprintf(stdout, "LMI_PROTOSHORT");
			break;
		case LMI_SYSERR:
			fprintf(stdout, "LMI_SYSERR");
			break;
		case LMI_WRITEFAIL:
			fprintf(stdout, "LMI_WRITEFAIL");
			break;
		case LMI_CRCERR:
			fprintf(stdout, "LMI_CRCERR");
			break;
		case LMI_DLE_EOT:
			fprintf(stdout, "LMI_DLE_EOT");
			break;
		case LMI_FORMAT:
			fprintf(stdout, "LMI_FORMAT");
			break;
		case LMI_HDLC_ABORT:
			fprintf(stdout, "LMI_HDLC_ABORT");
			break;
		case LMI_OVERRUN:
			fprintf(stdout, "LMI_OVERRUN");
			break;
		case LMI_TOOSHORT:
			fprintf(stdout, "LMI_TOOSHORT");
			break;
		case LMI_INCOMPLETE:
			fprintf(stdout, "LMI_INCOMPLETE");
			break;
		case LMI_BUSY:
			fprintf(stdout, "LMI_BUSY");
			break;
		case LMI_NOANSWER:
			fprintf(stdout, "LMI_NOANSWER");
			break;
		case LMI_CALLREJECT:
			fprintf(stdout, "LMI_CALLREJECT");
			break;
		case LMI_HDLC_IDLE:
			fprintf(stdout, "LMI_HDLC_IDLE");
			break;
		case LMI_HDLC_NOTIDLE:
			fprintf(stdout, "LMI_HDLC_NOTIDLE");
			break;
		case LMI_QUIESCENT:
			fprintf(stdout, "LMI_QUIESCENT");
			break;
		case LMI_RESUMED:
			fprintf(stdout, "LMI_RESUMED");
			break;
		case LMI_DSRTIMEOUT:
			fprintf(stdout, "LMI_DSRTIMEOUT");
			break;
		case LMI_LAN_COLLISIONS:
			fprintf(stdout, "LMI_LAN_COLLISIONS");
			break;
		case LMI_LAN_REFUSED:
			fprintf(stdout, "LMI_LAN_REFUSED");
			break;
		case LMI_LAN_NOSTATION:
			fprintf(stdout, "LMI_LAN_NOSTATION");
			break;
		case LMI_LOSTCTS:
			fprintf(stdout, "LMI_LOSTCTS");
			break;
		case LMI_DEVERR:
			fprintf(stdout, "LMI_DEVERR");
			break;
		default:
			fprintf(stdout, "%u", p->lmi.error_ind.lmi_reason);
			break;
		}
		fprintf(stdout, ",lmi_state=");
		switch (p->lmi.error_ind.lmi_state) {
		case LMI_UNATTACHED:
			fprintf(stdout, "LMI_UNATTACHED");
			break;
		case LMI_ATTACH_PENDING:
			fprintf(stdout, "LMI_ATTACH_PENDING");
			break;
		case LMI_UNUSABLE:
			fprintf(stdout, "LMI_UNUSABLE");
			break;
		case LMI_DISABLED:
			fprintf(stdout, "LMI_DISABLED");
			break;
		case LMI_ENABLE_PENDING:
			fprintf(stdout, "LMI_ENABLE_PENDING");
			break;
		case LMI_ENABLED:
			fprintf(stdout, "LMI_ENABLED");
			break;
		case LMI_DISABLE_PENDING:
			fprintf(stdout, "LMI_DISABLE_PENDING");
			break;
		case LMI_DETACH_PENDING:
			fprintf(stdout, "LMI_DETACH_PENDING");
			break;
		default:
			fprintf(stdout, "[%u]", p->lmi.error_ind.lmi_state);
			break;
		}
		fputc(stdout, '}');
		break;
	case LMI_STATS_IND:
		fprintf(stdout, "ctrl=LMI_STATS_IND");
		fprintf(stdout, "{lmi_interval=%u", p->lmi.stats_ind.lmi_interval);
		fprintf(stdout, ",lmi_timestamp=%u}", p->lmi.stats_ind.lmi_timestamp);
		fprintf(stdout, " data=");
		print_data(data.buf, data.len);
		break;
	case LMI_EVENT_IND:
		fprintf(stdout, "ctrl=LMI_EVENT_IND");
		fprintf(stdout, "{lmi_objectid=%u", p->lmi.event_ind.lmi_objectid);
		fprintf(stdout, ",lmi_timestamp=%u", p->lmi.event_ind.lmi_timestamp);
		fprintf(stdout, ",lmi_severity=%u}", p->lmi.event_ind.lmi_severity);
		break;
	case SL_PDU_REQ:
		fprintf(stdout, "ctrl=SL_PDU_REQ");
		fprintf(stdout, "{sl_mp=%u}", p->sl.pdu_req.sl_mp);
		fprintf(stdout, " data=");
		print_data(data.buf, data.len);
		break;
	case SL_EMERGENCY_REQ:
		fprintf(stdout, "ctrl=SL_EMERGENCY_REQ");
		break;
	case SL_EMERGENCY_CEASES_REQ:
		fprintf(stdout, "ctrl=SL_EMERGENCY_CEASES_REQ");
		break;
	case SL_START_REQ:
		fprintf(stdout, "ctrl=SL_START_REQ");
		break;
	case SL_STOP_REQ:
		fprintf(stdout, "ctrl=SL_STOP_REQ");
		break;
	case SL_RETRIEVE_BSNT_REQ:
		fprintf(stdout, "ctrl=SL_RETRIEVE_BSNT_REQ");
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		fprintf(stdout, "ctrl=SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		fprintf(stdout, "{sl_fsnc=%u}", p->sl.retrieval_req_and_fsnc);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		fprintf(stdout, "ctrl=SL_CLEAR_BUFFERS_REQ");
		break;
	case SL_CLEAR_RTB_REQ:
		fprintf(stdout, "ctrl=SL_CLEAR_RTB_REQ");
		break;
	case SL_CONTINUE_REQ:
		fprintf(stdout, "ctrl=SL_CONTINUE_REQ");
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		fprintf(stdout, "ctrl=SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		break;
	case SL_RESUME_REQ:
		fprintf(stdout, "ctrl=SL_RESUME_REQ");
		break;
	case SL_CONGESTION_DISCARD_REQ:
		fprintf(stdout, "ctrl=SL_CONGESTION_DISCARD_REQ");
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		fprintf(stdout, "ctrl=SL_CONGESTION_ACCEPT_REQ");
		break;
	case SL_NO_CONGESTION_REQ:
		fprintf(stdout, "ctrl=SL_NO_CONGESTION_REQ");
		break;
	case SL_POWER_ON_REQ:
		fprintf(stdout, "ctrl=SL_POWER_ON_REQ");
		break;
	case SL_OPTMGMT_REQ:
		fprintf(stdout, "ctrl=SL_OPTMGMT_REQ");
		break;
	case SL_NOTIFY_REQ:
		fprintf(stdout, "ctrl=SL_NOTIFY_REQ");
		break;
	case SL_PDU_IND:
		fprintf(stdout, "ctrl=SL_PDU_IND");
		fprintf(stdout, "{sl_mp=%u}", p->sl.pdu_ind.sl_mp);
		fprintf(stdout, " data=");
		print_data(data.buf, data.len);
		break;
	case SL_LINK_CONGESTED_IND:
		fprintf(stdout, "ctrl=SL_LINK_CONGESTED_IND");
		fprintf(stdout, "{sl_timestamp=%u", p->sl.link_cong_ind.sl_timestamp);
		fprintf(stdout, ",sl_cong_status=%u", p->sl.link_cong_ind.sl_cong_status);
		fprintf(stdout, ",sl_disc_status=%u}", p->sl.link_cong_ind.sl_disc_status);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		fprintf(stdout, "ctrl=SL_LINK_CONGESTION_CEASED_IND");
		fprintf(stdout, "{sl_timestamp=%u", p->sl.link_cong_ceased_ind.sl_timestamp);
		fprintf(stdout, ",sl_cong_status=%u", p->sl.link_cong_ceased_ind.sl_cong_status);
		fprintf(stdout, ",sl_disc_status=%u}", p->sl.link_cong_ceased_ind.sl_disc_status);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		fprintf(stdout, "ctrl=SL_RETRIEVED_MESSAGE_IND");
		fprintf(stdout, "{sl_mp=%u}", p->sl.retrieved_msg_ind.sl_mp);
		fprintf(stdout, " data=");
		print_data(data.buf, data.len);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		fprintf(stdout, "ctrl=SL_RETRIEVAL_COMPLETE_IND");
		fprintf(stdout, "{sl_mp=%u}", p->sl.retrieval_comp_ind.sl_mp);
		fprintf(stdout, " data=");
		print_data(data.buf, data.len);
		break;
	case SL_RB_CLEARED_IND:
		fprintf(stdout, "ctrl=SL_RB_CLEARED_IND");
		break;
	case SL_BSNT_IND:
		fprintf(stdout, "ctrl=SL_BSNT_IND");
		fprintf(stdout, "{sl_bsnt=%u}", p->sl.bsnt_ind.sl_bsnt);
		break;
	case SL_IN_SERVICE_IND:
		fprintf(stdout, "ctrl=SL_IN_SERVICE_IND");
		break;
	case SL_OUT_OF_SERVICE_IND:
		fprintf(stdout, "ctrl=SL_OUT_OF_SERVICE_IND");
		fprintf(stdout, "{sl_timestamp=%u", p->sl.out_of_service_ind.sl_timestamp);
		fprintf(stdout, ",sl_reason=%u}", p->sl.out_of_service_ind.sl_reason);
		switch (p->sl.out_of_service_ind.sl_reason) {
		case SL_FAIL_UNSPECIFIED:
			fprintf(stdout, "SL_FAIL_UNSPECIFIED");
			break;
		case SL_FAIL_CONG_TIMEOUT:
			fprintf(stdout, "SL_FAIL_CONG_TIMEOUT");
			break;
		case SL_FAIL_ACK_TIMEOUT:
			fprintf(stdout, "SL_FAIL_ACK_TIMEOUT");
			break;
		case SL_FAIL_ABNORMAL_BSNR:
			fprintf(stdout, "SL_FAIL_ABNORMAL_BSNR");
			break;
		case SL_FAIL_ABNORMAL_FIBR:
			fprintf(stdout, "SL_FAIL_ABNORMAL_FIBR");
			break;
		case SL_FAIL_SUERM_EIM:
			fprintf(stdout, "SL_FAIL_SUERM_EIM");
			break;
		case SL_FAIL_ALIGNMENT_NOT_POSSIBLE:
			fprintf(stdout, "SL_FAIL_ALIGNMENT_NOT_POSSIBLE");
			break;
		case SL_FAIL_RECEIVED_SIO:
			fprintf(stdout, "SL_FAIL_RECEIVED_SIO");
			break;
		case SL_FAIL_RECEIVED_SIN:
			fprintf(stdout, "SL_FAIL_RECEIVED_SIN");
			break;
		case SL_FAIL_RECEIVED_SIE:
			fprintf(stdout, "SL_FAIL_RECEIVED_SIE");
			break;
		case SL_FAIL_RECEIVED_SIOS:
			fprintf(stdout, "SL_FAIL_RECEIVED_SIOS");
			break;
		case SL_FAIL_T1_TIMEOUT:
			fprintf(stdout, "SL_FAIL_T1_TIMEOUT");
			break;
		default:
			fprintf(stdout, "SL_FAIL_UNKNOWN");
			break;
		}
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		fprintf(stdout, "ctrl=SL_REMOTE_PROCESSOR_OUTAGE_IND");
		fprintf(stdout, "{sl_timestamp=%u}", p->sl.rem_proc_out_ind.sl_timestamp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		fprintf(stdout, "ctrl=SL_REMOTE_PROCESSOR_RECOVERED_IND");
		fprintf(stdout, "{sl_timestamp=%u}", p->sl.rem_proc_recovered_ind.sl_timestamp);
		break;
	case SL_RTB_CLEARED_IND:
		fprintf(stdout, "ctrl=SL_RTB_CLEARED_IND");
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		fprintf(stdout, "ctrl=SL_RETRIEVAL_NOT_POSSIBLE_IND");
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		fprintf(stdout, "ctrl=SL_BSNT_NOT_RETRIEVABLE_IND");
		fprintf(stdout, "{sl_bsnt=%u}", p->sl.bsnt_not_retr_ind.sl_bsnt);
		break;
	case SL_OPTMGMT_ACK:
		fprintf(stdout, "ctrl=SL_OPTMGMT_ACK");
		break;
	case SL_NOTIFY_IND:
		fprintf(stdout, "ctrl=SL_NOTIFY_IND");
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		fprintf(stdout, "ctrl=SL_LOCAL_PROCESSOR_OUTAGE_IND");
		fprintf(stdout, "{sl_timestamp=%u}", p->sl.loc_proc_out_ind.sl_timestamp);
		break;
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		fprintf(stdout, "ctrl=SL_LOCAL_PROCESSOR_RECOVERED_IND");
		fprintf(stdout, "{sl_timestamp=%u}", p->sl.loc_proc_recovered_ind.sl_timestamp);
		break;
	default:
		return (-1);
	}
	fputc(stdout, '\n');
	return (prim);
}

int
wait_event(int wait)
{
	for (;;) {
		struct pollfd pfd[] = {
			{mon_fd, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
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
		}
		if (usr_signal) {
			usr_signal = 0;
			usr_action();
		}
		if (output > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, 1, wait)) {
		case -1:
			if (errno == EAGAIN || errno = EINTR || errno = ERESTART)
				continue;
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			syslog(LOG_ERR, "%s: %m", __FUNCTION__);
			mon_exit(1);
			return (-1);
		case 0:
			return (0);
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int ret, flags = 0;

				ctrl.maxlen = BUFSIZE;
				ctrl.len = 0;
				ctrl.buf = cbuf;
				data.maxlen = BUFSIZE;
				data.len = 0;
				data.buf = dbuf;
				if (getmsg(mon_fd, &ctrl, &data, &flags) < 0) {
					syslog(LOG_ERR, "%s: getmsg error", __FUNCTION__);
					syslog(LOG_ERR, "%s: %m", __FUNCTION__);
					mon_exit(1);
					break;
				}
				if (ctrl.len > 0)
					return decode_ctrl();
				if (data.len > 0)
					return decode_data();
				break;
			}
			if (pfd[0].revents & POLLNVAL) {
				syslog(LOG_ERR, "%s: device invalid", __FUNCTION__);
				mon_exit(1);
				return (-1);
			}
			if (pfd[0].revents & POLLHUP) {
				syslog(LOG_ERR, "%s: device hangup", __FUNCTION__);
				mon_exit(1);
				return (-1);
			}
			if (pfd[0].revents & POLLERR) {
				syslog(LOG_ERR, "%s: device error", __FUNCTION__);
				mon_exit(1);
				return (-1);
			}
			break;
		default:
			syslog(LOG_ERR, "%s: poll error", __FUNCTION__);
			mon_exit(1);
			return (-1);
		}
	}
}

int
mon_open(void)
{
	if (output > 1)
		syslog(LOG_NOTICE, "opening %s", devname);
	if ((mon_fd = open(devname, O_NONBLOCK | O_RDWR)) < 0) {
		syslog(LOG_ERR, "%s: could not open devname: %s", __FUNCTION__, devname);
		syslog(LOG_ERR, "%s: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	if (output > 1)
		syslog(LOG_NOTICE, "configuring %s", devname);
	if (ioctl(mon_fd, I_SRDOPT, RMSGD) < 0) {
		syslog(LOG_ERR, "%s: could not configure devname: %s", __FUNCTION__, devname);
		syslog(LOG_ERR, "%s: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	return (0);
}

int
mon_attach(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	int ret;
	size_t ppa_len;
	caddr_t ppa_ptr;

	if (ppa != 0) {
		ppa_len = sizeof(ppa);
		ppa_ptr = (caddr_t) &ppa;
	} else if (clei[0] != '\0') {
		ppa_len = strnlen(clei, sizeof(clei));
		ppa_ptr = (caddr_t) clei;
	} else {
		syslog(LOG_ERR, "%s: no address to attach", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->attach_req) + ppa_len;
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_ATTACH_REQ;
	bcopy(&ppa, p->attach_req.lmi_ppa, ppa_len);
	if ((ret = putmsg(mon_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	for (;;) {
		switch ((ret = wait_event(100))) {
		case 0:
			syslog(LOG_ERR, "%s: error, no response to LMI_ATTACH_REQ", __FUNCTION__);
			mon_exit(1);
			return (-1);
		case LMI_OK_ACK:
			return (0);
		case LMI_ERROR_ACK:
			syslog(LOG_ERR, "%s: lmi_errno: %s", __FUNCTION__,
			       strerrno(p->error_ack.lmi_errno));
			syslog(LOG_ERR, "%s: lmi_reason: 0x%08x", __FUNCTION__,
			       p->error_ack.lmi_reason);
			syslog(LOG_ERR, "%s; lmi_error_primitive: %d", __FUNCTION__,
			       p->error_ack.lmi_error_primitive);
			mon_exit(1);
			return (-1);
		default:
			syslog(LOG_ERR, "%s: error, unexpected response to LMI_ATTACH_REQ %d",
			       __FUNCTION__, ret);
			mon_exit(1);
			return (-1);
		case (-1):
			return (-1);
		}
	}
}

int
mon_detach(void)
{
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	int ret;

	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(mon_fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		syslog(LOG_ERR, "%s: putmsg: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	for (;;) {
		switch ((ret = wait_event(100))) {
		case 0:
			syslog(LOG_ERR, "%s: error, no response to LMI_DETACH_REQ", __FUNCTION__);
			mon_exit(1);
			return (-1);
		case LMI_OK_ACK:
			return (0);
		case LMI_ERROR_ACK:
			syslog(LOG_ERR, "%s: lmi_errno: %s", __FUNCTION__,
			       strerrno(p->error_ack.lmi_errno));
			syslog(LOG_ERR, "%s: lmi_reason: 0x%08x", __FUNCTION__,
			       p->error_ack.lmi_reason);
			syslog(LOG_ERR, "%s; lmi_error_primitive: %d", __FUNCTION__,
			       p->error_ack.lmi_error_primitive);
			mon_exit(1);
			return (-1);
		default:
			syslog(LOG_ERR, "%s: error, unexpected response to LMI_DETACH_REQ %d",
			       __FUNCTION__, ret);
			mon_exit(1);
			return (-1);
		case (-1):
			return (-1);
		}
	}
}

void
show_sl_config(void)
{
	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t1: %u\n", monconf.sl.t1);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t2: %u\n", monconf.sl.t2);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t2l: %u\n", monconf.sl.t2l);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t2h: %u\n", monconf.sl.t2h);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t3: %u\n", monconf.sl.t3);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t4n: %u\n", monconf.sl.t4n);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t4e: %u\n", monconf.sl.t4e);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t5: %u\n", monconf.sl.t5);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t6: %u\n", monconf.sl.t6);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: t7: %u\n", monconf.sl.t7);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: rb_abate: %u\n", monconf.sl.rb_abate);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: rb_accept: %u\n", monconf.sl.rb_accept);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: rb_discard: %u\n", monconf.sl.rb_discard);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_abate_1: %u\n", monconf.sl.tb_abate_1);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_onset_1: %u\n", monconf.sl.tb_onset_1);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_discd_1: %u\n", monconf.sl.tb_discd_1);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_abate_2: %u\n", monconf.sl.tb_abate_2);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_onset_2: %u\n", monconf.sl.tb_onset_2);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_discd_2: %u\n", monconf.sl.tb_discd_2);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_abate_3: %u\n", monconf.sl.tb_abate_3);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_onset_3: %u\n", monconf.sl.tb_onset_3);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: tb_discd_3: %u\n", monconf.sl.tb_discd_3);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: N1: %u\n", monconf.sl.N1);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: N2: %u\n", monconf.sl.N2);
	fprint_time(stdout);
	fprintf(stdout, " # sl config: M: %u\n", monconf.sl.M);
}

void
show_sdt_config(void)
{
	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: t8: %u\n", monconf.sdt.t8);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: Tin: %u\n", monconf.sdt.Tin);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: Tie: %u\n", monconf.sdt.Tie);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: T: %u\n", monconf.sdt.T);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: D: %u\n", monconf.sdt.D);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: Te: %u\n", monconf.sdt.Te);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: De: %u\n", monconf.sdt.De);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: Ue: %u\n", monconf.sdt.Ue);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: N: %u\n", monconf.sdt.N);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: m: %u\n", monconf.sdt.m);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: b: %u\n", monconf.sdt.b);
	fprint_time(stdout);
	fprintf(stdout, " # sdt config: f: ", monconf.sdt.f);
	switch (monconf.sdt.f) {
	case SDT_FLAGS_ONE:
		fprintf(stdout, "one\n");
		break;
	case SDT_FLAGS_SHARED:
		fprintf(stdout, "shared\n");
		break;
	case SDT_FLAGS_TWO:
		fprintf(stdout, "two\n");
		break;
	case SDT_FLAGS_THREE:
		fprintf(stdout, "three\n");
		break;
	default:
		fprintf(stdout, "%d\n", monconf.sdt.f);
		break;
	}
}

void
show_sdl_config(void)
{
	int i;

	ftimestamp();
	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifname: %s\n", monconf.sdl.ifname);

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifflags:");
	if (monconf.sdl.ifflags & SDL_IF_UP)
		fprintf(stdout, " up");
	if (monconf.sdl.ifflags & SDL_IF_RX_RUNNING)
		fprintf(stdout, " rx");
	if (monconf.sdl.ifflags & SDL_IF_TX_RUNNING)
		fprintf(stdout, " tx");
	fputc(stdout, '\n');
	fprint_timer(stdout);
	fprintf(stdout, " # sdl config: iftype:");
	switch (monconf.sdl.iftype) {
	case SDL_TYPE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_TYPE_V35:
		fprintf(stdout, " v35");
		break;
	case SDL_TYPE_DS0:
		fprintf(stdout, " ds0");
		break;
	case SDL_TYPE_DS0A:
		fprintf(stdout, " ds0a");
		break;
	case SDL_TYPE_E1:
		fprintf(stdout, " e1");
		break;
	case SDL_TYPE_T1:
		fprintf(stdout, " t1");
		break;
	case SDL_TYPE_J1:
		fprintf(stdout, " j1");
		break;
	case SDL_TYPE_ATM:
		fprintf(stdout, " atm");
		break;
	case SDL_TYPE_PACKET:
		fprintf(stdout, " packet");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.iftype);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifrate:");
	switch (monconf.sdl.ifrate) {
	case SDL_RATE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_RATE_DS0A:
		fprintf(stdout, " ds0a");
		break;
	case SDL_RATE_DS0:
		fprintf(stdout, " ds0");
		break;
	case SDL_RATE_T1:	/* SDL_RATE_J1 */
		fprintf(stdout, " t1/j1");
		break;
	case SDL_RATE_E1:
		fprintf(stdout, " e1");
		break;
	default:
		fprintf(stdout, " %u", monconf.sdl.ifrate);
		break;
	}
	fputc(stdout, '\n');
	fprintf(stdout, " # sdl config: ifgtype:");
	switch (monconf.sdl.ifgtype) {
	case SDL_GTYPE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_GTYPE_T1:
		fprintf(stdout, " t1");
		break;
	case SDL_GTYPE_E1:
		fprintf(stdout, " e1");
		break;
	case SDL_GTYPE_J1:
		fprintf(stdout, " j1");
		break;
	case SDL_GTYPE_ATM:
		fprintf(stdout, " atm");
		break;
	case SDL_GTYPE_ETH:
		fprintf(stdout, " eth");
		break;
	case SDL_GTYPE_IP:
		fprintf(stdout, " ip");
		break;
	case SDL_GTYPE_UDP:
		fprintf(stdout, " udp");
		break;
	case SDL_GTYPE_TCP:
		fprintf(stdout, " tcp");
		break;
	case SDL_GTYPE_RTP:
		fprintf(stdout, " rtp");
		break;
	case SDL_GTYPE_SCTP:
		fprintf(stdout, " sctp");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifgtype);
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifgrate:");
	switch (monconf.sdl.ifgrate) {
	case SDL_GRATE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_GRATE_T1:	/* SDL_GRATE_J1 */
		fprintf(stdout, " t1/j1");
		break;
	case SDL_GRATE_E1:
		fprintf(stdout, " e1");
		break;
	default:
		fprintf(stdout, " %u", monconf.sdl.ifgrate);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifmode:");
	switch (monconf.sdl.ifmode) {
	case SDL_MODE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_MODE_DSU:
		fprintf(stdout, " dsu");
		break;
	case SDL_MODE_CSU:
		fprintf(stdout, " csu");
		break;
	case SDL_MODE_DTE:
		fprintf(stdout, " dte");
		break;
	case SDL_MODE_DCE:
		fprintf(stdout, " dce");
		break;
	case SDL_MODE_CLIENT:
		fprintf(stdout, " client");
		break;
	case SDL_MODE_SERVER:
		fprintf(stdout, " server");
		break;
	case SDL_MODE_PEER:
		fprintf(stdout, " peer");
		break;
	case SDL_MODE_ECHO:
		fprintf(stdout, " echo");
		break;
	case SDL_MODE_REM_LB:
		fprintf(stdout, " rem-lb");
		break;
	case SDL_MODE_LOC_LB:
		fprintf(stdout, " loc-lb");
		break;
	case SDL_MODE_LB_ECHO:
		fprintf(stdout, " lb-echo");
		break;
	case SDL_MODE_TEST:
		fprintf(stdout, " test");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifmode);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifgmode:");
	switch (monconf.sdl.ifgmode) {
	case SDL_GMODE_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_GMODE_LOC_LB:
		fprintf(stdout, " loc-lb");
		break;
	case SDL_GMODE_REM_LB:
		fprintf(stdout, " rem-lb");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifgmode);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifgcrc:");
	switch (monconf.sdl.ifgcrc) {
	case SDL_GCRC_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_GCRC_CRC4:
		fprintf(stdout, " crc4");
		break;
	case SDL_GCRC_CRC5:
		fprintf(stdout, " crc5");
		break;
	case SDL_GCRC_CRC6:
		fprintf(stdout, " crc6");
		break;
	case SDL_GCRC_CRC6J:
		fprintf(stdout, " crc6j");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifgcrc);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifclock:");
	switch (monconf.sdl.ifclock) {
	case SDL_CLOCK_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_CLOCK_INT:
		fprintf(stdout, " int");
		break;
	case SDL_CLOCK_EXT:
		fprintf(stdout, " ext");
		break;
	case SDL_CLOCK_LOOP:
		fprintf(stdout, " loop");
		break;
	case SDL_CLOCK_MASTER:
		fprintf(stdout, " master");
		break;
	case SDL_CLOCK_SLAVE:
		fprintf(stdout, " slave");
		break;
	case SDL_CLOCK_DPLL:
		fprintf(stdout, " dpll");
		break;
	case SDL_CLOCK_ABR:
		fprintf(stdout, " abr");
		break;
	case SDL_CLOCK_SHAPER:
		fprintf(stdout, " shaper");
		break;
	case SDL_CLOCK_TICK:
		fprintf(stdout, " tick");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifclock);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifcoding:");
	switch (monconf.sdl.ifcoding) {
	case SDL_CODING_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_CODING_NRZ:
		fprintf(stdout, " nrz");
		break;
	case SDL_CODING_NRZI:
		fprintf(stdout, " nrzi");
		break;
	case SDL_CODING_AMI:
		fprintf(stdout, " ami");
		break;
	case SDL_CODING_B6ZS:
		fprintf(stdout, " b6zs");
		break;
	case SDL_CODING_B8ZS:
		fprintf(stdout, " b8zs");
		break;
	case SDL_CODING_HDB3:
		fprintf(stdout, " hdb3");
		break;
	case SDL_CODING_AAL1:
		fprintf(stdout, " aal1");
		break;
	case SDL_CODING_AAL2:
		fprintf(stdout, " aal2");
		break;
	case SDL_CODING_AAL5:
		fprintf(stdout, " aal5");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifcoding);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifframing:");
	switch (monconf.sdl.ifframing) {
	case SDL_FRAMING_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_FRAMING_CCS:
		fprintf(stdout, " ccs");
		break;
	case SDL_FRAMING_CAS:
		fprintf(stdout, " cas");
		break;
	case SDL_FRAMING_SF:
		fprintf(stdout, " sf");
		break;
	case SDL_FRAMING_ESF:
		fprintf(stdout, " esf");
		break;
	case SDL_FRAMING_D4:
		fprintf(stdout, " d4");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.ifframing);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifblksize: %u\n", monconf.sdl.ifblksize);

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifleads:");
	if (monconf.sdl.ifleads & SDL_LEAD_DTR)
		fprintf(stdout, " dtr");
	if (monconf.sdl.ifleads & SDL_LEAD_RTS)
		fprintf(stdout, " rts");
	if (monconf.sdl.ifleads & SDL_LEAD_DCD)
		fprintf(stdout, " dcd");
	if (monconf.sdl.ifleads & SDL_LEAD_CTS)
		fprintf(stdout, " cts");
	if (monconf.sdl.ifleads & SDL_LEAD_DSR)
		fprintf(stdout, " dsr");
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifbpv: %u\n", monconf.sdl.ifbpv);

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifalarms:");
	if (monconf.sdl.ifalarms & SDL_ALARM_RED)
		fprintf(stdout, " red");
	if (monconf.sdl.ifalarms & SDL_ALARM_BLU)
		fprintf(stdout, " blu");
	if (monconf.sdl.ifalarms & SDL_ALARM_YEL)
		fprintf(stdout, " yel");
	if (monconf.sdl.ifalarms & SDL_ALARM_REC)
		fprintf(stdout, " rec");
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifrxlevel: %u\n", monconf.sdl.ifrxlevel);

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: iftxlevel:");
	switch (monconf.sdl.iftxlevel) {
	case SDL_TXLEVEL_NONE:
		fprintf(stdout, " none");
		break;
	case SDL_TXLEVEL_DSX_133FT:	/* SDL_TXLEVEL_CSU_0DB *//* SDL_TXLEVEL_75OHM_NM */
		fprintf(stdout, " dsx(133ft)/csu(0dB)/75ohm(NM)");
		break;
	case SDL_TXLEVEL_DSX_266FT:	/* SDL_TXLEVEL_120OHM_NM */
		fprintf(stdout, " dsx(266ft)/120ohm(NM)");
		break;
	case SDL_TXLEVEL_DSX_399FT:	/* SDL_TXLEVEL_75OHM_PR */
		fprintf(stdout, " dsx(399ft)/75ohm(PR)");
		break;
	case SDL_TXLEVEL_DSX_533FT:	/* SDL_TXLEVEL_120OHM_PR */
		fprintf(stdout, " dsx(533ft)/120ohm(PR)");
		break;
	case SDL_TXLEVEL_DSX_666FT:	/* SDL_TXLEVEL_75OHM_HRL */
		fprintf(stdout, " dsx(666ft)/75ohm(HRL)");
		break;
	case SDL_TXLEVEL_CSU_8DB:	/* SDL_TXLEVEL_120OHM_HRL */
		fprintf(stdout, " csu(8dB)/120ohm(HRL)");
		break;
	case SDL_TXLEVEL_CSU_15DB:
		fprintf(stdout, " csu(15dB)");
		break;
	case SDL_TXLEVEL_CSU_23DB:
		fprintf(stdout, " csu(23dB)");
		break;
	case SDL_TXLEVEL_MON_0DB:
		fprintf(stdout, " mon(0dB)");
		break;
	case SDL_TXLEVEL_MON_12DB:
		fprintf(stdout, " mon(12dB)");
		break;
	case SDL_TXLEVEL_MON_20DB:	/* SDL_TXLEVEL_MON_26DB *//* SDL_TXLEVEL_MON_30DB */
		fprintf(stdout, " mon(20dB)/mon(26dB)/mon(30dB)");
		break;
	case SDL_TXLEVEL_MON_32DB:
		fprintf(stdout, " mon(32dB)");
		break;
	default:
		fprintf(stdout, " [%u]", monconf.sdl.iftxlevel);
		break;
	}
	fputc(stdout, '\n');

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifsync: %u\n", monconf.sdl.ifsync);

	fprint_time(stdout);
	fprintf(stdout, " # sdl config: ifsyncsrc:");
	for (i = 0; i < SDL_SYNCS; i++)
		fprintf(stdout, " %u", monconf.sdl.ifsyncsrc[i]);
	fputc(stdout, '\n');
}

void
output_config(void)
{
	show_sl_config();
	if (output > 1)
		show_sdt_config();
	if (output > 1)
		show_sdl_config();
}

int
mon_config(void)
{
	if (output > 1)
		syslog(LOG_NOTICE, "Reading signalling link options");
	ctl.ic_cmd = SL_IOCGOPTIONS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(monconf.opt);
	ctl.ic_dp = (char *) &monconf.opt;
	if (ioctl(mon_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SL_IOCGOPTIONS: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reading signalling link configuration");
	ctl.ic_cmd = SL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(monconf.sl);
	if (ioctl(mon_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SL_IOCGCONFIG: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reading signalling terminal configuration");
	ctl.ic_cmd = SDT_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(monconf.sdt);
	if (ioctl(mon_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDT_IOCGCONFIG: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	if (output > 1)
		syslog(LOG_NOTICE, "Reading signalling data link configuration");
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(monconf.sdl);
	if (ioctl(mon_fd, I_STR, &ctl) < 0) {
		syslog(LOG_ERR, "%s: ioctl: SDL_IOCGCONFIG: %m", __FUNCTION__);
		mon_exit(1);
		return (-1);
	}
	return (0);
}

int
mon_start(void)
{
	switch (link_state) {
	case 0:
		if (output > 2)
			syslog(LOG_NOTICE, "opening link");
		if (mon_open() != (0))
			return (-1);
		link_state = 1;
	case 1:
		if (output > 2)
			syslog(LOG_NOTICE, "attaching link");
		if (mon_attach() != (0))
			return (-1);
		link_state = 2;
	case 2:
		if (output > 2)
			syslog(LOG_NOTICE, "getting configuration");
		if (mon_config() != (0))
			return (-1);
		lin_state = 3;
	case 3:
	}
	return (0);
}

int
mon_stop(void)
{
	switch (link_state) {
	case 3:
		link_state = 2;
	case 2:
		if (output > 2)
			syslog(LOG_NOTICE, "detaching link");
		if (mon_detach() != (0))
			return (-1);
		link_state = 1;
	case 1:
		if (output > 2)
			syslog(LOG_NOTICE, "closing link");
		if (mon_close() != (0))
			return (-1);
		link_state = 0;
	case 0:
	}
	return (0);
}

void
mon_enter(void)
{
	if (nomead) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			perror("slmon");
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		setsid();	/* become a session leader */
		/* for once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror("slmon");
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
	openlog("slmon", LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON);
	if (lnkname[0] == '\0')
		snprintf(lnkname, sizeof(lnkname), "slmon%d", getpid());
	if (nomead || outfile[0] == '\0') {
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
			mon_exit(2);
		}
	}
	if (nomead || errfile[0] == '\0') {
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
			mon_exit(2);
		}
	}
	sig_catch();
	output_header();
	mon_config();
	output_config();
	syslog(LOG_NOTICE, "Startup complete.");
}

void
slmon(void)
{
	mon_enter();
	if (mon_start() == (0))
		while (wait_event(10000) != (-1)) ;
	mon_stop();
	mon_exit(1);
}

void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
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
");
}

void
version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2003-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permission.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {{-p, --ppa} PPA | {-c, --clei} CLEI}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] {{-p, --ppa} PPA | {-c, --clei} CLEI}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet                                 (default: off)\n\
        suppress output\n\
    -v, --verbose                               (default: off)\n\
        increase verbosity of output\n\
Command Options:\n\
    -p, --ppa PPA\n\
        specify PPA of device to monitor        (default: %2$d)\n\
    -c, --clei CLEI\n\
        specify CLEI of device to monitor       (default: %3$s)\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
    -C, --copying\n\
        prints the copying permissions and exits\n\
Capture Options:\n\
    -d, --daemon                                (default: off)\n\
        run in the background as a daemon\n\
    -O, --outpdir OUTPDIR                       (default: %4$s)\n\
        output directory for capute and errors\n\
    -n, --lnkname LNKKNAME                      (default: slmon$$)\n\
        link name\n\
    -o, --outfile OUTFILE                       (default: ${lnkname}.out)\n\
        redirect output to outfile\n\
    -l, --logfile LOGFILE                       (default: ${lnkname}.err)\n\
        redirect errors to errfile\n\
    -f, --cfgfile CFGFILE                       (default: slmon.conf)\n\
        configuration file name\n\
    -e, --defvice DEVNAME                       (defaule: %5$s)\n\
        device name to open\n\
", argv[0], ppa, clei, outpdir, devname);
}

int
main(int argc, char **argv)
{
	int c;
	int val;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"daemon",	no_argument,		NULL, 'd'},
			{"outpdir",	required_argument,	NULL, 'O'},
			{"lnkname",	required_argument,	NULL, 'n'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"logfile",	required_argument,	NULL, 'l'},
			{"cfgfile",	required_argument,	NULL, 'f'},
			{"device",	required_argument,	NULL, 'e'},
			{"ppa",		required_argument,	NULL, 'p'},
			{"clei",	required_argument,	NULL, 'c'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "dO:n:o:l:f:e:p:c:qvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'd':	/* -d, --daemon */
			nomead = 1;
			break;
		case 'O':	/* -O, --outpdir OUTPDIR */
			strncpy(outpdir, optarg, sizeof(outpdir) - 1);
			break;
		case 'n':	/* -n, --lnkname LNKNAME */
			strncpy(lnkname, optarg, sizeof(lnkname) - 1);
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'l':	/* -l, --logfile LOGFILE */
			strncpy(logfile, optarg, sizeof(logfile) - 1);
			break;
		case 'f':	/* -f, --cfgfile CFGFILE */
			strncpy(cfgfile, optarg, sizeof(cfgfile) - 1);
			break;
		case 'e':	/* -e, --device */
			strncpy(devname, optarg, sizeof(devname) - 1);
			break;
		case 'p':	/* -p, --ppa */
			if (clei[0] != '\0')
				goto bad_option;
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			ppa = val;
			break;
		case 'c':
			if (ppa != 0)
				goto bad_option;
			strncpy(clei, optarg, sizeof(clei) - 1);
			break;
		case 'f':
			strncpy(filename, optarg, sizeof(filename) - 1);
			break;
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --versbose */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			// syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	slmon();
	exit(4);
}
