/*****************************************************************************

 @(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/02 10:58:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/03/02 10:58:16 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/02 10:58:16 $"

static char const ident[] =
    "$RCSfile: fdetach.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/02 10:58:16 $";

#define _XOPEN_SOURCE 600

/* 
 *  autopush(8)
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <stropts.h>

static int debug = 0;
static int output = 1;

static void version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
%1$s %2$s\n\
    Copyright (c) 2003  OpenSS7 Corporation.  All Rights Reserved.\n\
    Distributed under GPL Version 2, included here by reference.\n\
", argv[0], ident);
}

static void usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] path\n\
    %1$s {-h|--help}\n\
    %1$s {-v|--version}\n\
", argv[0]);
}

static void help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	usage(argc, argv);
	fprintf(stderr, "\
Arguments:\n\
    path\n\
        the path to the mounted STREAMS-special file\n\
Options:\n\
    -d, --debug\n\
        sets debugging verbosity\n\
    -q, --quiet\n\
        suppress output\n\
    -v, --version\n\
        prints the version and exits\n\
    -h, --help\n\
        prints this usage information and exits\n\
");
}

int main(int argc, char **argv)
{
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"debug", 0, 0, 'd'},
			{"quiet", 0, 0, 'q'},
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 'v'},
		};
		c = getopt_long_only(argc, argv, "dqhv", long_options, &option_index);
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'd':	/* -d, --debug */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			debug += 1;
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'h':	/* -h, --help */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'v':	/* -v, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: syntax error near '", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "'\n");
			} else {
				fprintf(stderr, "%s: missing option or argument", argv[0]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	if (optind >= argc) {
		if (debug)
			fprintf(stderr, "%s: missing path argument\n", argv[0]);
		goto syntax_error;
	}
	if (argc - optind != 1) {
		if (debug)
			fprintf(stderr, "%s: too many arguments\n", argv[0]);
		goto syntax_error;
	}
	if (debug) {
		fprintf(stderr, "%s: path = \"%s\"\n", argv[0], argv[optind]);
	}
	if (fdetach(argv[optind]) < 0) {
		if (output || debug)
			fprintf(stderr, "%s: fdetach(): %s\n", argv[0], strerror(errno));
		exit(1);
	}
	exit(0);
}
