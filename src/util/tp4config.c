/*****************************************************************************

 @(#) File: src/util/tp4config.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 *****************************************************************************/

static char const ident[] = "src/util/tp4config.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

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

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */
static int dryrun = 0;			/* dry run */

struct tp4config {
	int inactivity;
	int retransmit;
	int upper;
	int lower;
	int window;
	int ack;
	int flow;
	int n;
	int credit;
	int initial;
	int maxtpdu;
	int maxtidu;
	int checksum;
	int extended;
	int propose;
	int decca;
	int transaction;
	int ts1;
	int ts2;
	int delay;
	int idle;
	int useflow;
	int ccitt;
};

static struct tp4config config = {
	.inactivity = 960000,
	.retransmit = 30000,
	.upper = 10000,
	.lower = 10,
	.window = 120000,
	.ack = 512,
	.flow = 208,
	.n = 4,
	.credit = 4,
	.initial = 1,
	.maxtpdu = 1024,
	.maxtidu = 504,
	.checksum = 0,
	.extended = 1,
	.propose = 1,
	.decca = 0,
	.transaction = 60000,
	.ts1 = 60000,
	.ts2 = 30000,
	.delay = 1000,
	.idle = 300000,
	.useflow = 1,
	.ccitt = 0,
};

static void
do_config(int argc, char *argv[], int start)
{
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
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
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2015  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, PACKAGE_ENVR " " PACKAGE_DATE);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
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
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Options:\n\
  Command:\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
  Common:\n\
    -i, --inactivity MILLISECONDS\n\
        inactivity timer (ms) [default: %2$d]\n\
    -r, --retransmit MILLISECONDS\n\
        retransmit timer (ms) [default: %3$d]\n\
    -u, --upper MILLISECONDS\n\
        upper retransmit time (ms) [default: %22$d]\n\
    -l, --lower MILLISECONDS\n\
        lower retransmit time (ms) [default: %23$d]\n\
    -w, --window MILLISECONDS\n\
        window timer (ms) [default: %4$d]\n\
    -a, --ack MILLISECONDS\n\
        acknowledge timer (ms) [default: %5$d]\n\
    -f, --flow MILLISECONDS\n\
        flow timer (ms) [default: %6$d]\n\
    -N, --n COUNT\n\
        transmit count [default: %7$d]\n\
    -c, --credit COUNT\n\
        credit count [default: %8$d]\n\
    -I, --initial COUNT\n\
        initial count [default: %9$d]\n\
    -M, --maxtpdu OCTETS\n\
        maximum TPDU size (octets) [default: %10$d]\n\
    -m, --maxtidu OCTETS\n\
        maximum TIDU size (octets) [default: %11$d]\n\
    -k, --checksum [FLAG], -K, --no-checksum\n\
        perform checksum [default: %12$d]\n\
    -e, --extended [FLAG], -E, --no-extended\n\
        extended sequence numbers [default: %13$d]\n\
    -p, --propose [FLAG], -P, --no-propose\n\
        propose extended seq no [default: %14$d]\n\
    -s, --transaction MILLISECONDS\n\
        transaction connectoin hold time [default: %15$d]\n\
    -1, --ts1 MILLISECONDS\n\
        TS1 time interval (ms) [default: %16$d]\n\
    -2, --ts2 MILLISECONDS\n\
        TS2 time interval (ms) [default: %17$d]\n\
    -y, --delay MILLISECONDS\n\
        disconnection delay (ms) [default: %18$d]\n\
    -L, --idle MILLISECONDS\n\
        maximum NC idle time (ms) [default: %19$d]\n\
    -z, --useflow [FLAG], -Z, --no-useflow\n\
        explicit flow control [default: %20$d]\n\
    -g, --ccitt [FLAG], -G, --no-ccitt\n\
        propose CCITT classes [default: %21$d]\n\
    -b, --decca [FLAG], -B, --no-decca\n\
        DEC congestion avoidance [default: %24$d]\n\
    -n, --dryrun\n\
        check but do not write [default: false]\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -d, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
", argv[0], config.inactivity, config.retransmit, config.window, config.ack, config.flow, config.n, config.credit, config.initial, config.maxtpdu, config.maxtidu, config.checksum, config.extended, config.propose, config.transaction, config.ts1, config.ts2, config.delay, config.idle, config.useflow, config.ccitt, config.upper, config.lower, config.decca);
}

#define COMMAND_DFLT  0
#define COMMAND_CNFG  1
#define COMMAND_HELP  2
#define COMMAND_VERS  3
#define COMMAND_COPY  4

int
main(int argc, char *argv[])
{
	int command = COMMAND_DFLT;
	int c, val;
	int start = 0;

	for (;;) {
#if defined _GNU_SOURCE
		int option_index = 0;
                /* *INDENT-OFF* */
                static struct option long_options[] = {
			{"inactivity",	required_argument,	NULL, 'i'},
			{"retransmit",	required_argument,	NULL, 'r'},
			{"upper",	required_argument,	NULL, 'u'},
			{"lower",	required_argument,	NULL, 'l'},
			{"window",	required_argument,	NULL, 'w'},
			{"ack",		required_argument,	NULL, 'a'},
			{"flow",	required_argument,	NULL, 'f'},
			{"n",		required_argument,	NULL, 'N'},
			{"credit",	required_argument,	NULL, 'c'},
			{"initial",	required_argument,	NULL, 'I'},
			{"maxtpdu",	required_argument,	NULL, 'M'},
			{"maxtidu",	required_argument,	NULL, 'm'},
			{"checksum",	optional_argument,	NULL, 'k'},
			{"no-checksum",	no_argument,		NULL, 'K'},
			{"extended",	optional_argument,	NULL, 'e'},
			{"no-extended",	no_argument,		NULL, 'E'},
			{"propose",	optional_argument,	NULL, 'p'},
			{"no-propose",	no_argument,		NULL, 'P'},
			{"transaction",	required_argument,	NULL, 's'},
			{"ts1",		required_argument,	NULL, '1'},
			{"ts2",		required_argument,	NULL, '2'},
			{"delay",	required_argument,	NULL, 'y'},
			{"idle",	required_argument,	NULL, 'L'},
			{"useflow",	optional_argument,	NULL, 'z'},
			{"no-useflow",	no_argument,		NULL, 'Z'},
			{"ccitt",	optional_argument,	NULL, 'g'},
			{"no-ccitt",	no_argument,		NULL, 'G'},
			{"dryrun",	no_argument,		NULL, 'n'},
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

		c = getopt_long_only(argc, argv,
				     "i:r:u:l:w:a:f:N:c:I:M:m:k::Ke::Ep::Ps:1:2:y:L:z::Zg::Gnd::v::hVC?W:",
				     long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "i:r:u:l:w:a:f:N:c:I:M:m:kKeEpPs:1:2:y:L:zZgGnqd:vhVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'i':	/* -i, --inactivity MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.inactivity = val;
			break;
		case 'r':	/* -r, --retransmit MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.retransmit = val;
			break;
		case 'u':	/* -u, --upper MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.upper = val;
			break;
		case 'l':	/* -l, --lower MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.lower = val;
			break;
		case 'w':	/* -w, --window MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.window = val;
			break;
		case 'a':	/* -a, --ack MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.ack = val;
			break;
		case 'f':	/* -f, --flow MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.flow = val;
			break;
		case 'N':	/* -N, --n COUNT */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.n = val;
			break;
		case 'c':	/* -c, --credit COUNT */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.credit = val;
			break;
		case 'I':	/* -I, --initial COUNT */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.initial = val;
			break;
		case 'M':	/* -M, --maxtpdu OCTETS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.maxtpdu = val;
			break;
		case 'm':	/* -m, --maxtidu OCTETS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.maxtidu = val;
			break;
		case 'k':	/* -k, --checksum [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.checksum = 1;
			else
				config.checksum = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'K':	/* -K, --no-checksum */
			command = COMMAND_CNFG;
			config.checksum = 0;
			break;
		case 'e':	/* -e, --extended [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.extended = 1;
			else
				config.extended = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'E':	/* -E, --no-extended */
			command = COMMAND_CNFG;
			config.extended = 0;
			break;
		case 'p':	/* -p, --propose [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.propose = 1;
			else
				config.propose = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'P':	/* -P, --no-propose */
			command = COMMAND_CNFG;
			config.propose = 0;
			break;
		case 'b':	/* -b, --decca [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.decca = 1;
			else
				config.decca = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'B':	/* -B, --no-decca */
			command = COMMAND_CNFG;
			config.decca = 0;
			break;
		case 's':	/* -s, --transaction MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.transaction = val;
			break;
		case '1':	/* -1, --ts1 MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.ts1 = val;
			break;
		case '2':	/* -2, --ts2 MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.ts2 = val;
			break;
		case 'y':	/* -y, --delay MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.delay = val;
			break;
		case 'L':	/* -L, --idle MS */
			command = COMMAND_CNFG;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.idle = val;
			break;
		case 'z':	/* -z, --useflow [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.useflow = 1;
			else
				config.useflow = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'Z':	/* -Z, --no-useflow */
			command = COMMAND_CNFG;
			config.useflow = 0;
			break;
		case 'g':	/* -g, --ccitt [FLAG] */
			command = COMMAND_CNFG;
			if (optarg == NULL)
				config.ccitt = 1;
			else
				config.ccitt = strtol(optarg, NULL, 0) ? 1 : 0;
			break;
		case 'G':	/* -G, --no-ccitt */
			command = COMMAND_CNFG;
			config.ccitt = 0;
			break;
		case 'n':	/* -n, --dryrun */
			dryrun = 1;
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'd':	/* -d, --debug [level] */
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
			command = COMMAND_HELP;
			break;
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			command = COMMAND_VERS;
			break;
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			command = COMMAND_COPY;
			break;
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
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
	if (optind < argc) {
		if (debug)
			fprintf(stderr, "%s: excess non-option arguments\n", argv[0]);
		goto bad_nonopt;
	}
	switch (command) {
	case COMMAND_DFLT:
	case COMMAND_CNFG:
		do_config(argc, argv, start);
		break;
	case COMMAND_HELP:
		help(argc, argv);
		break;
	case COMMAND_VERS:
		version(argc, argv);
		break;
	case COMMAND_COPY:
		copying(argc, argv);
		break;
	}
	exit(0);
}
