/*****************************************************************************

 @(#) $RCSfile: strvf.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/12/15 20:20:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/12/15 20:20:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strvf.c,v $
 Revision 0.9.2.19  2007/12/15 20:20:21  brian
 - updates

 Revision 0.9.2.18  2007/08/13 22:46:37  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: strvf.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/12/15 20:20:21 $"

static char const ident[] =
    "$RCSfile: strvf.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/12/15 20:20:21 $";

#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>
#include <sys/strlog.h>

static int logging = 1;
static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

char outfile[256] = "";
char errfile[256] = "";
char outpath[256] = "";
char errpath[256] = "";
char basname[256] = "";
char outpdir[256] = "/var/log/streams";
char devname[256] = "";

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 3, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Options:\n\
    -n, --nologging\n\
        do not log, send output to stdout and stderr\n\
    -b, --basename BASENAME\n\
        file basename, default: 'strvf'\n\
    -O, --directory DIRECTORY\n\
        redirect output to files in DIRECTORY, default: /var/log/streams\n\
    -o, --outfile OUTFILE\n\
        redirect output to OUTFILE, default: ${BASENAME}.log\n\
    -e, --errfile ERRFILE\n\
        redirect errors to ERRFILE, default: ${BASENAME}.err\n\
    -D, --devname DEVNAME\n\
        test device to open, default: '/dev/echo'\n\
    -q, --quiet\n\
        suppress output\n\
    -d, --debug [LEVEL]\n\
        increment or set debug LEVEL (default: 0)\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL (default: 1)\n\
    -h, --help, -?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
", argv[0]);
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
the terms of the GNU Affero General Public License as published by the Free\n\
Software Foundation; Version 3 of the License.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the GNU  Affero  General Public License along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
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

static void
strvf_exit(int retval)
{
	if (retval) {
		fprintf(stderr, "--------------------------------------------------\n");
		fprintf(stderr, "-> Linux Fast-STREAMS is NOT operational.\n");
		fprintf(stderr, "--------------------------------------------------\n");
	} else {
		fprintf(stderr, "--------------------------------------------------\n");
		fprintf(stderr, "-> Linux Fast-STREAMS is installed and operational\n");
		fprintf(stderr, "--------------------------------------------------\n");
	}
	fflush(stderr);
	exit(retval);
}

static void
strvf(int argc, char *argv[])
{
	int fd;

	if (basname[0] == '\0')
		snprintf(basname, sizeof(basname), "strvf");
	if (logging || outfile[0] != '\0') {
		if (outfile[0] == '\0')
			snprintf(outfile, sizeof(outfile), "%s.log", basname);
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stdout to file %s", outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stdout to %s", outpath);
			exit(1);
		}
	}
	if (logging || errfile[0] != '\0') {
		if (errfile[0] == '\0')
			snprintf(errfile, sizeof(errfile), "%s.err", basname);
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
		if (output > 1)
			syslog(LOG_NOTICE, "Redirecting stderr to file %s", errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(LOG_ERR, "%m");
			syslog(LOG_ERR, "Could not redirect stderr to %s", errpath);
			exit(1);
		}
	}
	syslog(LOG_NOTICE, "Startup complete.");
	fprintf(stderr, "-> Logging results to %s\n", outpath);
	fprintf(stderr, "--------------------------------------------------\n");
	fprintf(stderr, "-> Verify Linux Fast-STREAMS installation.\n");
	fprintf(stderr, "-> Verify open, putmsg, getmsg, ioctl, and close\n");
	fprintf(stderr, "-> can be performed on a stream.\n");
	fprintf(stderr, "--------------------------------------------------\n");
	fflush(stderr);
	{
		fprintf(stdout, "Open device %s\n", devname);
		fflush(stdout);
		if ((fd = open(devname, O_RDWR)) < 0) {
			fprintf(stdout, "Open failed.\n");
			fflush(stdout);
			perror(__FUNCTION__);
			strvf_exit(1);
		}
		fprintf(stdout, "Open successful.\n");
		fflush(stdout);
	}
	{
		int ret, flags = 0;
		char ctlbuf[] = "\0\1\2\3\4\5\6\7";
		char datbuf[] = "\0\1\2\3\4\5\6\7";
		struct strbuf ctl = { sizeof(ctlbuf), sizeof(ctlbuf), ctlbuf };
		struct strbuf dat = { sizeof(datbuf), sizeof(datbuf), datbuf };

		fprintf(stdout, "Putmsg to device %s\n", devname);
		fflush(stdout);
		if ((ret = putmsg(fd, &ctl, &dat, flags)) < 0) {
			fprintf(stdout, "Putmsg failed.\n");
			fflush(stdout);
			perror(__FUNCTION__);
			strvf_exit(1);
		}
		fprintf(stdout, "Putmsg successful.\n");
		fflush(stdout);
	}
	{
		int ret, flags = 0;
		char ctlbuf[8];
		char datbuf[8];
		struct strbuf ctl = { sizeof(ctlbuf), sizeof(ctlbuf), ctlbuf };
		struct strbuf dat = { sizeof(datbuf), sizeof(datbuf), datbuf };

		fprintf(stdout, "Getmsg from device %s\n", devname);
		fflush(stdout);
		if ((ret = getmsg(fd, &ctl, &dat, &flags)) < 0) {
			fprintf(stdout, "Getmsg failed.\n");
			fflush(stdout);
			perror(__FUNCTION__);
			strvf_exit(1);
		}
		fprintf(stdout, "Getmsg successful.\n");
		fflush(stdout);
	}
	{
		int ret;
		char iocbuf[] = "\0\1\2\3\4\5\6\7";
		struct strioctl ioc = { I_STR, 0, sizeof(iocbuf), iocbuf };

		fprintf(stdout, "Ioctl of device %s\n", devname);
		fflush(stdout);
		if ((ret = ioctl(fd, I_STR, &ioc)) < 0) {
			fprintf(stdout, "Ioctl failed.\n");
			fflush(stdout);
			perror(__FUNCTION__);
			strvf_exit(1);
		}
		fprintf(stdout, "Ioctl successful.\n");
		fflush(stdout);
	}
	{
		fprintf(stdout, "Close device %s\n", devname);
		fflush(stdout);
		if (close(fd) < 0) {
			fprintf(stdout, "Close failed.\n");
			fflush(stdout);
			perror(__FUNCTION__);
			strvf_exit(1);
		}
		fprintf(stdout, "Close successful.\n");
		fflush(stdout);
	}
	strvf_exit(0);
}

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"nologging",	no_argument,		NULL, 'n'},
			{"basename",	required_argument,	NULL, 'b'},
			{"directory",	required_argument,	NULL, 'O'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"errfile",	required_argument,	NULL, 'e'},
			{"devname",	required_argument,	NULL, 'D'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "n:b:O:o:e:D:qd::v::hVC?W:", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "n:b:O:o:e:D:qdvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'n':	/* -n, --nologging */
			logging = 0;
			break;
		case 'b':
			strncpy(basname, optarg, sizeof(basname));
			break;
		case 'O':
			strncpy(outpdir, optarg, sizeof(outpdir));
			break;
		case 'o':
			strncpy(outfile, optarg, sizeof(outfile));
			break;
		case 'e':
			strncpy(errfile, optarg, sizeof(errfile));
			break;
		case 'D':
			strncpy(devname, optarg, sizeof(devname));
			break;
		case 'd':	/* -d, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'v':	/* -v, --verbose [level] */
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
			if (output || debug) {
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
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	strvf(argc, argv);
	exit(0);
}
