/*****************************************************************************

 @(#) $RCSfile: strace.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/07/17 08:06:46 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/17 08:06:46 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strace.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/07/17 08:06:46 $"

static char const ident[] =
    "$RCSfile: strace.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/07/17 08:06:46 $";

/* 
 *  AIX Utility: strace - Prints STREAMS trace messages.
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <time.h>
#include <stropts.h>
#include <sys/strlog.h>
#include <syslog.h>

static int debug = 0;
static int output = 1;

static void version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [MODULE UNIT PRIORITY] ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] [MODULE UNIT PRIORITY] ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    MODULE\n\
        the module ID of the module to trace, -1 for any module\n\
    UNIT\n\
	the unit ID (typically minor number) of the instance of the module\n\
	to trace, -1 for any unit\n\
    PRIORITY\n\
        the maximum priority of messages to display\n\
Options:\n\
    -q, --quiet\n\
        suppress output\n\
    -d, --debug [LEVEL]\n\
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
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>\n\
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

int main(int argc, char *argv[])
{
	int i, fd, count;
	struct trace_ids *tids;
	struct strioctl ic;
	while (1) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
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
		c = getopt_long_only(argc, argv, "qdvhVC?", long_options, &option_index);
#else
		c = getopt(argc, argv, "qdvhVC?");
#endif
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'd':	/* -d, --debug */
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
		case 'v':	/* -v, --verbose */
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
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (optind == argc) {
		count = 1;
		if ((tids = calloc(count, sizeof(struct trace_ids))) == NULL) {
			perror(argv[0]);
			exit(1);
		}
		tids->ti_mid = -1;
		tids->ti_sid = -1;
		tids->ti_level = -1;
		tids->ti_flags = -1;
	} else if (optind < argc && (argc - optind) % 3 == 0) {
		count = (argc - optind) / 3;
		if ((tids = calloc(count, sizeof(struct trace_ids))) == NULL) {
			perror(argv[0]);
			exit(1);
		}
		for (i = 0; i < count; i++) {
			if (strncmp(argv[optind], "all", 4)) {
				tids[i].ti_mid = -1;
			} else {
			}
			optind++;
			if (strncmp(argv[optind], "all", 4)) {
				tids[i].ti_sid = -1;
			} else {
			}
			optind++;
			if (strncmp(argv[optind], "all", 4)) {
				tids[i].ti_level = -1;
			} else {
			}
			optind++;
			tids[i].ti_flags = -1;
		}
	} else
		goto bad_nonopt;
	/* open log device */
	if ((fd = open("/dev/strlog", O_RDWR)) < 0) {
		if ((fd = open("/dev/streams/log", O_RDWR)) < 0) {
			if ((fd = open("/dev/log", O_RDWR)) < 0) {
				perror(argv[0]);
				exit(1);
			}
		}
	}
	/* set up log device for tracing */
	ic.ic_cmd = I_TRCLOG;
	ic.ic_timout = 0;
	ic.ic_len = count * sizeof(struct trace_ids);
	ic.ic_dp = (char *) tids;
	if (ioctl(fd, I_STR, ic) < 0) {
		perror(argv[0]);
		exit(1);
	}
	for (;;) {
		int ret, flags;
		char cbuf[1024];
		char dbuf[1024];
		struct strbuf ctl = { 1024, 1024, cbuf };
		struct strbuf dat = { 1024, 1024, dbuf };
		struct log_ctl *lc;
		if ((ret = getmsg(fd, &ctl, &dat, &flags)) < 0) {
			perror(argv[0]);
			exit(1);
		}
		if (ret != 0) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(ERANGE));
			exit(1);
		}
		lc = (struct log_ctl *) cbuf;
		if (ctl.len < sizeof(*lc))
			continue;
		if (dat.len <= 0)
			continue;
		fprintf(stdout, "%d", lc->seq_no);
		fprintf(stdout, " %s", ctime(&lc->ltime));
		fprintf(stdout, "%lu", (unsigned long) lc->ttime);
		fprintf(stdout, "%3d", lc->level);
		switch (lc->flags & (SL_ERROR | SL_FATAL | SL_NOTIFY)) {
		case 0:
			fprintf(stdout, "   ");
			break;
		case SL_ERROR:
			fprintf(stdout, "E  ");
			break;
		case SL_FATAL:
			fprintf(stdout, "F  ");
			break;
		case SL_NOTIFY:
			fprintf(stdout, "N  ");
			break;
		case SL_ERROR | SL_FATAL:
			fprintf(stdout, "EF ");
			break;
		case SL_ERROR | SL_NOTIFY:
			fprintf(stdout, "EN ");
			break;
		case SL_FATAL | SL_NOTIFY:
			fprintf(stdout, "FN ");
			break;
		case SL_ERROR | SL_FATAL | SL_NOTIFY:
			fprintf(stdout, "EFN");
			break;
		}
		fprintf(stdout, "%d", lc->mid);
		fprintf(stdout, "%d", lc->sid);
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	if (close(fd) < 0) {
		perror(argv[0]);
	}
	exit(0);
}
