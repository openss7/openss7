/*****************************************************************************

 @(#) $RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/03/08 00:20:01 $

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

 Last Modified $Date: 2004/03/08 00:20:01 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/03/08 00:20:01 $"

static char const ident[] = "$RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/03/08 00:20:01 $";

#define _XOPEN_SOURCE 600

/* 
 * SVR 4.2 Utility: strconf - Queries stream configuration.
 */

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
#include <sys/ioctl.h>

static int debug = 0;
static int verbose = 1;

void
version(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Distributed under GPL Version 2, included here by reference.\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-h|--push} module_list\n\
    %1$s [options] (-p|--pop} [{-a|--all}|{-u|--upto} module_list]\n\
    %1$s [options] (-f|--file}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version\n\
    %1$s {-C, --copying\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-h|--push} module_list\n\
    %1$s [options] (-p|--pop} [{-a|--all}|{-u|--upto} module_list]\n\
    %1$s [options] (-f|--file}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version\n\
    %1$s {-C, --copying\n\
Options:\n\
    -l, --load\n\
        specifies that the listed drivers, modules or STREAMS executive are\n\
	to be loaded\n\
    -u, --unload\n\
        specifies that the listed drivers, modules or STREAMS executive are\n\
	to be unloaded\n\
    -Q, --query\n\
        specifies that the listed drivers, modules or STREAMS executive are\n\
	to be queried\n\
    -f, --file FILE\n\
        specifies configuration file from which to take configuration information\n\
    -d, --drivers DRIVER[,DRIVER]\n\
        specifies a comma separated list of drivers to load, unload or query\n\
    -m, --modules MODULE[,MODULE]\n\
        specifies a comma separated list of modules to load, unload or query\n\
    -v, --verbose\n\
	verbose output\n\
    -q, --quiet\n\
	suppress normal output\n\
    -h, --help, -?\n\
	print this usage message and exits\n\
    -V, --version\n\
	print the version and exits\n\
    -C, --copying\n\
	print the copyright and exits\n\
", argv[0]);
}

void
copying(int argc, char *argv[])
{
	if (verbose < 0)
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

enum { CMN_NONE, CMN_PUSH, CMN_POP, CMN_POPUPTO, CMN_POPALL, CMN_FILE };
int command = CMN_NONE;

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#define OPTS_MAX 256

char fbuf[PATH_MAX] = { '\0', };
char mbuf[OPTS_MAX] = { '\0', };

int flen = 0;
int mlen = 0;

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"push",	required_argument,	NULL, 'h'},
			{"pop",		no_argument,		NULL, 'p'},
			{"upto",	required_argument,	NULL, 'u'},
			{"all",		no_argument,		NULL, 'a'},
			{"file",	required_argument,	NULL, 'f'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'H'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
		};
		/* *INDENT-ON* */
		c = getopt_long_only(argc, argv, "h:pu:af:dqvHVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "h:pu:af:dqvHVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			goto bad_usage;
		case 'h':	/* -h, --push modulelist */
			if (command != CMN_NONE && command != CMN_FILE)
				goto bad_option;
			command = CMN_PUSH;
			mlen = strnlen(optarg, OPTS_MAX);
			strncpy(mbuf, optarg, OPTS_MAX);
			break;
		case 'p':	/* -p, --pop */
			if (command != CMN_NONE && command != CMN_FILE)
				goto bad_option;
			command = CMN_POP;
			break;
		case 'u':	/* -u, --upto module */
			if (command != CMN_POP)
				goto bad_option;
			mlen = strnlen(optarg, OPTS_MAX);
			strncpy(mbuf, optarg, OPTS_MAX);
			command = CMN_POPUPTO;
			break;
		case 'a':	/* -a, --all */
			if (command != CMN_POP)
				goto bad_option;
			command = CMN_POPALL;
			break;
		case 'f':	/* -f, --file filename */
			if (command != CMN_NONE)
				goto bad_option;
			command = CMN_FILE;
			flen = strnlen(optarg, PATH_MAX);
			strncpy(fbuf, optarg, PATH_MAX);
			break;
		case 'd':	/* -d, --debug */
			if (optarg == NULL) {
				debug++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'q':
			debug = 0;
			verbose = -1;
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
		case 'H':	/* -H, --help */
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
			if (verbose) {
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
	if (command == CMN_NONE)
		goto bad_nonopt;
	exit(0);
}
