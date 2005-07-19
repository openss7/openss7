/*****************************************************************************

 @(#) $RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/07/19 11:15:08 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/07/19 11:15:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strlog.c,v $
 Revision 0.9.2.3  2005/07/19 11:15:08  brian
 - added syslogd and friends

 Revision 0.9.2.2  2005/07/18 12:38:51  brian
 - standard indentation

 Revision 0.9.2.1  2005/07/18 06:18:54  brian
 - updating log facility

 *****************************************************************************/

#ident "@(#) $RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/07/19 11:15:08 $"

static char const ident[] =
    "$RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/07/19 11:15:08 $";

/*
 *  Linux Fast-STREAMS Utility: strlog - logs STREAMS error or trace messages
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <stropts.h>
#include <sys/strlog.h>
#define SYSLOG_NAMES
#include <syslog.h>
#include <sys/sad.h>

static int debug = 0;
static int output = 1;

static void
version(int argc, char **argv)
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

static void
usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [MESSAGE ...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] [MESSAGE ...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    MESSAGE\n\
        a message string to be sent to the logger\n\
        if no message string, and no -f option, read from stdin\n\
Options:\n\
    -M, --mid MID\n\
        module id for STREAMS logger\n\
    -S, --sid SID\n\
        unit id for STREAMS logger\n\
    -p, --priority PRIORITY\n\
        number or \"facility.level\" notation PRIORITY\n\
    -E, --error\n\
        generate error log (the default if nothing specified)\n\
    -T, --trace\n\
        generate trace log\n\
    -N, --notify\n\
        notify the system administrator (by mail)\n\
    -c, --console\n\
        display log to the console\n\
    -f, --file FILE\n\
        read messages to log from FILE\n\
    -s, --stderr\n\
        also log messages to standard output\n\
    -t, --tag TAG\n\
        for compatibility with logger(1); ignored\n\
    -i, --id\n\
        for compatibility with logger(1); ignored\n\
    -u, --socket DEVICE\n\
        for compatibility with logger(1); ignored\n\
    -d, --datagram\n\
        for compatibility with logger(1); ignored\n\
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

static void
copying(int argc, char *argv[])
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

void
log_line(short mid, short sid, char priority, int logflags, unsigned long seq, const char *line,
	 int stderr_also)
{
	if (stderr_also) {
		const char *flags = "???";
		struct timespec tv = { 0, 0 };
		clock_gettime(CLOCK_MONOTONIC, &tv);
		switch (logflags & (SL_TRACE | SL_FATAL | SL_NOTIFY)) {
		case 0:
			flags = "   ";
			break;
		case SL_NOTIFY:
			flags = "  N";
			break;
		case SL_FATAL:
			flags = " F ";
			break;
		case SL_FATAL | SL_NOTIFY:
			flags = " FN";
			break;
		case SL_TRACE:
			flags = "T  ";
			break;
		case SL_TRACE | SL_NOTIFY:
			flags = "T N";
			break;
		case SL_TRACE | SL_FATAL:
			flags = "TF ";
			break;
		case SL_TRACE | SL_FATAL | SL_NOTIFY:
			flags = "TFN";
			break;
		}
		fprintf(stderr, "%3lu %12lu %12lu %s %5hd %5hd %s\n", seq, time(NULL), tv.tv_sec,
			flags, mid, sid, line);
	}
}

int
main(int argc, char *argv[])
{
	int options;
	int my_logflags = 0;		/* user specified flags */
	int xflags = 0;			/* extra flags */
	int stderr_also = 0;		/* log also to stderr */
	int logflags = SL_ERROR;	/* logger flags */
	int priority = LOG_USER | LOG_DEBUG;	/* facility and level */
	short mid = 0;			/* module identifier */
	short sid = 0;			/* subunit identifier */
	char filename[256] = "";	/* file name */
	int use_file = 0;		/* use file in filename */
	int read_stdin = 0;		/* read from stdin */

	for (options = 0;; options++) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"mid",		required_argument,	NULL, 'M'},
			{"sid",		required_argument,	NULL, 'S'},
			{"priority",	required_argument,	NULL, 'p'},
			{"error",	no_argument,		NULL, 'E'},
			{"trace",	no_argument,		NULL, 'T'},
			{"notify",	no_argument,		NULL, 'N'},
			{"console",	no_argument,		NULL, 'c'},
			{"file",	required_argument,	NULL, 'f'},
			{"stderr",	no_argument,		NULL, 's'},
			{"tag",		required_argument,	NULL, 't'},
			{"id",		no_argument,		NULL, 'i'},
			{"socket",	required_argument,	NULL, 'u'},
			{"datagram",	no_argument,		NULL, 'd'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "M:S:p:ETNcf:st:iu:dqD::v::hVC?", long_options,
				     &option_index);
#else
		c = getopt(argc, argv, "M:S:p:ETNcf:st:iu:dqD::v::hVC?");
#endif
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 'M':	/* -M, --mid MID */
			if ((val = strtol(optarg, NULL, 0)) < 0 || val > 0xffffUL)
				goto bad_option;
			mid = val;
			continue;
		case 'S':	/* -S, --sid SID */
			if ((val = strtol(optarg, NULL, 0)) < 0 || val > 0xffffUL)
				goto bad_option;
			sid = val;
			continue;
		case 'p':	/* -p, --priority PRIORITTY */
		{
			CODE *fac = NULL;
			CODE *pri = NULL;

			if (isdigit(*optarg)) {
				/* its a number, use it */
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				for (fac = facilitynames; fac->c_name; fac++)
					if (LOG_FAC(val) == LOG_FAC(fac->c_val))
						break;
				if (!fac->c_name)
					goto bad_option;
				for (pri = prioritynames; pri->c_name; pri++)
					if (val == pri->c_val)
						break;
				if (!pri->c_name)
					goto bad_option;
				priority = val;
			} else {
				char facil_name[9];
				char level_name[9];

				if (sscanf(optarg, "%8[a-z].%8[a-z]", facil_name, level_name) != 2)
					goto bad_option;
				if (facil_name[0] == '\0')
					goto bad_option;
				for (fac = facilitynames; fac->c_name; fac++)
					if (strncmp(fac->c_name, facil_name, 9) == 0)
						break;
				if (!fac->c_name)
					goto bad_option;
				if (level_name[0] == '\0')
					goto bad_option;
				for (pri = prioritynames; pri->c_name; pri++)
					if (strncmp(pri->c_name, level_name, 9) == 0)
						break;
				if (!pri->c_name)
					goto bad_option;
			}
			/* things are ok now */
			priority = LOG_MAKEPRI(LOG_FAC(fac->c_val), pri->c_val);
			switch (LOG_PRI(priority)) {
			case LOG_EMERG:
				xflags |= SL_FATAL;
				break;
			case LOG_ALERT:
			case LOG_CRIT:
			case LOG_ERR:
				break;
			case LOG_WARNING:
				xflags |= SL_WARN;
				break;
			case LOG_NOTICE:
				xflags |= SL_NOTE;
				break;
			case LOG_INFO:
			case LOG_DEBUG:
				break;
			}
			continue;
		}
		case 'E':	/* -E, --error */
			my_logflags |= SL_ERROR;
			continue;
		case 'T':	/* -T, --trace */
			my_logflags |= SL_TRACE;
			continue;
		case 'N':	/* -N, --notify */
			my_logflags |= SL_NOTIFY | SL_ERROR;
			continue;
		case 'c':	/* -c, --console */
			my_logflags |= SL_CONSOLE | SL_ERROR;
			continue;
		case 'f':	/* -f, --file FILE */
			if (strnlen(optarg, sizeof(filename) + 1) > sizeof(filename))
				goto bad_option;
			strncpy(filename, optarg, sizeof(filename));
			if (filename[0] == '\0')
				goto bad_option;
			if (strncmp(filename, "-", sizeof(filename)) == 0) {
				filename[0] = '\0';
				read_stdin = 1;
			} else
				use_file = 1;
			continue;
		case 's':	/* -s, --stderr */
			stderr_also = 1;
			continue;
		case 't':	/* -t, --tag TAG */
		case 'i':	/* -i, --id */
		case 'u':	/* -u, --socket DEVICE */
		case 'd':	/* -d, --datagram */
			/* these four are ignored for compatibiltiy with logger(1) */
			continue;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			continue;
		case 'D':	/* -D, --debug */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
				continue;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			continue;
		case 'v':	/* -v, --verbose */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output++;
				continue;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			continue;
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
			if (output > 0 || debug > 0) {
				if (optind < argc) {
					const char *delim = "";

					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc) {
						fprintf(stderr, "%s%s", delim, argv[optind++]);
						delim = " ";
					}
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
	if (optind >= argc) {
		char buffer[1024];
		char *line;
		unsigned long lineno;
		FILE *f = stdin;

		/* no message argument provided */
		if (use_file == 0 && read_stdin == 0)
			/* nowhere to get messages from */
			goto bad_nonopt;
		if (use_file) {
			if ((f = fopen(filename, "r")) == NULL) {
				if (output || debug)
					perror(__FUNCTION__);
				exit(1);
			}
		}
		logflags = (my_logflags ? my_logflags : logflags) | xflags;
		/* read the file one line at a time */
		for (lineno = 1; (line = fgets(buffer, sizeof(buffer), f)) != NULL; lineno++) {
			size_t len;

			line[sizeof(buffer)] = '\0';
			/* find newline */
			len = strnlen(line, sizeof(buffer));
			/* chop the newline */
			if (len > 0 && line[len - 1] == '\n') {
				len--;
				line[len] = '\0';
			}
			log_line(mid, sid, priority, logflags, lineno, line, stderr_also);
		}
		fclose(f);
	} else {
		/* a message argument(s) was provided */
		if (read_stdin)
			goto bad_nonopt;
		logflags = (my_logflags ? my_logflags : logflags) | xflags;
		/* log messages one string at a time */
		for (; optind < argc; optind++)
			log_line(mid, sid, priority, logflags, (argc - optind), argv[optind],
				 stderr_also);
	}
	exit(0);
}
