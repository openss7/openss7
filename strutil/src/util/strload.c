/*****************************************************************************

 @(#) $RCSfile: strload.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/01/14 21:12:00 $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/01/14 21:12:00 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strload.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/01/14 21:12:00 $"

static char const ident[] =
    "$RCSfile: strload.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/01/14 21:12:00 $";

/*
 * AIX Utility: Loads and configures STREAMS.
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

int debug = 0;
int output = 1;

void version(int argc, char *argv[])
{
	if (output < 0)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
	if (output < 0)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-l|--load}]\n\
    %1$s [options] {-u|--unload}\n\
    %1$s [options] {-Q|--query}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
	if (output < 0)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-l|--load}]\n\
    %1$s [options] {-u|--unload}\n\
    %1$s [options] {-Q|--query}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
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
    -q, --quiet\n\
        suppress normal output (equivalent to --debug=0)\n\
    -D, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --output [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0]);
}

static void copying(int argc, char *argv[])
{
	if (output < 1 && !debug)
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

enum { CMD_NONE, CMD_LOAD, CMD_UNLOAD, CMD_QUERY, };

int command = CMD_NONE;
int flen = 0, dlen = 0, mlen = 0;

int main(int argc, char *argv[])
{
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"load",	no_argument,		NULL, 'l'},
			{"unload",	no_argument,		NULL, 'u'},
			{"query",	no_argument,		NULL, 'Q'},
			{"file",	required_argument,	NULL, 'f'},
			{"drivers",	optional_argument,	NULL, 'd'},
			{"modules",	optional_argument,	NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "luQf:d::m::qD::v::hVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "luQf:d::m::qD::v::hVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'l':
			if (command != CMD_NONE)
				goto bad_option;
			command = CMD_LOAD;
			break;
		case 'u':
			if (command != CMD_NONE)
				goto bad_option;
			command = CMD_UNLOAD;
			break;
		case 'Q':
			if (command != CMD_NONE)
				goto bad_option;
			command = CMD_QUERY;
			break;
		case 'f':
			if (flen || (flen = strlen(optarg)) > PATH_MAX)
				goto bad_option;
			break;
		case 'd':
			if (dlen || (dlen = strlen(optarg)) > PATH_MAX)
				goto bad_option;
			break;
		case 'm':
			if (mlen || (mlen = strlen(optarg)) > PATH_MAX)
				goto bad_option;
			break;
		case 'q':
			output = 0;
			debug = 0;
			break;
		case 'D':
			if (optarg == NULL) {
				debug++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':
			if (optarg == NULL) {
				output++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
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
	if (command == CMD_NONE)
		command = CMD_LOAD;
	exit(0);
}
